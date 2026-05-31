#include <napi.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>

// JXL Headers
#include <jxl/decode.h>
#include <jxl/encode.h>
#include <jxl/thread_parallel_runner.h>

// OpenCV Headers
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

// libjpeg Headers
#include <jpeglib.h>
#include <setjmp.h>

// Custom error manager for libjpeg
struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

static void my_error_exit(j_common_ptr cinfo) {
    my_error_mgr* myerr = (my_error_mgr*)cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    longjmp(myerr->setjmp_buffer, 1);
}

// Helpers
void clampSize(uint32_t width, uint32_t height, uint32_t max_width, uint32_t max_height, uint32_t* out_width, uint32_t* out_height) {
    if (width > max_width || height > max_height) {
        double aspect = (double)width / height;
        uint32_t new_height = (uint32_t)(max_width / aspect);
        if (new_height > max_height) {
            new_height = max_height;
        }
        *out_height = new_height;
        *out_width = (uint32_t)(new_height * aspect);
    } else {
        *out_width = width;
        *out_height = height;
    }
}

bool resizeImage(const std::vector<uint8_t>& src, int src_w, int src_h, int dst_w, int dst_h, std::vector<uint8_t>& dst) {
    if (src.empty() || src_w <= 0 || src_h <= 0 || dst_w <= 0 || dst_h <= 0) return false;
    try {
        cv::Mat src_mat(src_h, src_w, CV_8UC3, const_cast<uint8_t*>(src.data()));
        cv::Mat dst_mat;
        int interpolation = cv::INTER_LINEAR;
        if (dst_w < src_w || dst_h < src_h) {
            interpolation = cv::INTER_AREA;
        }
        cv::resize(src_mat, dst_mat, cv::Size(dst_w, dst_h), 0, 0, interpolation);
        dst.assign(dst_mat.data, dst_mat.data + dst_w * dst_h * 3);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "OpenCV resize exception: " << e.what() << std::endl;
        return false;
    }
}

bool encodeJPEG(const std::vector<uint8_t>& rgb_data, int width, int height, int quality, std::vector<uint8_t>& jpeg_data, const std::vector<uint8_t>& icc_profile) {
    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;
    
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_compress(&cinfo);
        return false;
    }
    
    jpeg_create_compress(&cinfo);
    
    unsigned char* outbuffer = nullptr;
    unsigned long outsize = 0;
    jpeg_mem_dest(&cinfo, &outbuffer, &outsize);
    
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    
    jpeg_start_compress(&cinfo, TRUE);
    
    if (!icc_profile.empty()) {
        jpeg_write_icc_profile(&cinfo, icc_profile.data(), icc_profile.size());
    }
    
    JSAMPROW row_pointer[1];
    int row_stride = width * 3;
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = const_cast<JSAMPROW>(&rgb_data[cinfo.next_scanline * row_stride]);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    
    jpeg_finish_compress(&cinfo);
    
    if (outbuffer && outsize > 0) {
        jpeg_data.assign(outbuffer, outbuffer + outsize);
        free(outbuffer);
    }
    
    jpeg_destroy_compress(&cinfo);
    return true;
}

struct JxlDecoderDeleter {
    void operator()(JxlDecoder* ptr) const { JxlDecoderDestroy(ptr); }
};
using JxlDecoderPtr = std::unique_ptr<JxlDecoder, JxlDecoderDeleter>;

struct JxlEncoderDeleter {
    void operator()(JxlEncoder* ptr) const { JxlEncoderDestroy(ptr); }
};
using JxlEncoderPtr = std::unique_ptr<JxlEncoder, JxlEncoderDeleter>;

struct JxlThreadParallelRunnerDeleter {
    void operator()(void* ptr) const { JxlThreadParallelRunnerDestroy(ptr); }
};
using JxlThreadParallelRunnerPtr = std::unique_ptr<void, JxlThreadParallelRunnerDeleter>;

bool decodeJXL(const uint8_t* jxl_data, size_t jxl_size, std::vector<uint8_t>& rgb_data, uint32_t& width, uint32_t& height, std::vector<uint8_t>& icc_profile, int max_threads) {
    JxlDecoderPtr dec(JxlDecoderCreate(nullptr));
    if (!dec) return false;
    
    JxlThreadParallelRunnerPtr runner(JxlThreadParallelRunnerCreate(nullptr, max_threads));
    if (!runner) return false;
    
    if (JXL_DEC_SUCCESS != JxlDecoderSetParallelRunner(dec.get(), JxlThreadParallelRunner, runner.get())) {
        return false;
    }
    
    if (JXL_DEC_SUCCESS != JxlDecoderSubscribeEvents(dec.get(), JXL_DEC_BASIC_INFO | JXL_DEC_COLOR_ENCODING | JXL_DEC_FULL_IMAGE)) {
        return false;
    }
    
    JxlDecoderSetInput(dec.get(), jxl_data, jxl_size);
    JxlDecoderCloseInput(dec.get());
    
    JxlPixelFormat format = {3, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
    
    for (;;) {
        JxlDecoderStatus status = JxlDecoderProcessInput(dec.get());
        if (status == JXL_DEC_ERROR || status == JXL_DEC_NEED_MORE_INPUT) {
            return false;
        }
        if (status == JXL_DEC_BASIC_INFO) {
            JxlBasicInfo info;
            if (JXL_DEC_SUCCESS != JxlDecoderGetBasicInfo(dec.get(), &info)) {
                return false;
            }
            width = info.xsize;
            height = info.ysize;
        } else if (status == JXL_DEC_COLOR_ENCODING) {
            size_t icc_size;
            if (JXL_DEC_SUCCESS == JxlDecoderGetICCProfileSize(dec.get(), JXL_COLOR_PROFILE_TARGET_DATA, &icc_size)) {
                icc_profile.resize(icc_size);
                if (JXL_DEC_SUCCESS != JxlDecoderGetColorAsICCProfile(dec.get(), JXL_COLOR_PROFILE_TARGET_DATA, icc_profile.data(), icc_profile.size())) {
                    icc_profile.clear();
                }
            }
        } else if (status == JXL_DEC_NEED_IMAGE_OUT_BUFFER) {
            size_t buffer_size;
            if (JXL_DEC_SUCCESS != JxlDecoderImageOutBufferSize(dec.get(), &format, &buffer_size)) {
                return false;
            }
            rgb_data.resize(buffer_size);
            if (JXL_DEC_SUCCESS != JxlDecoderSetImageOutBuffer(dec.get(), &format, rgb_data.data(), rgb_data.size())) {
                return false;
            }
        } else if (status == JXL_DEC_FULL_IMAGE) {
            // Decoded frame!
        } else if (status == JXL_DEC_SUCCESS) {
            break;
        }
    }
    return true;
}

bool encodeJXL(const uint8_t* rgb_data, uint32_t width, uint32_t height, std::vector<uint8_t>& jxl_data, int max_threads) {
    JxlEncoderPtr enc(JxlEncoderCreate(nullptr));
    if (!enc) return false;
    
    JxlThreadParallelRunnerPtr runner(JxlThreadParallelRunnerCreate(nullptr, max_threads));
    if (!runner) return false;
    
    if (JXL_ENC_SUCCESS != JxlEncoderSetParallelRunner(enc.get(), JxlThreadParallelRunner, runner.get())) {
        return false;
    }
    
    JxlBasicInfo info;
    JxlEncoderInitBasicInfo(&info);
    info.xsize = width;
    info.ysize = height;
    info.num_color_channels = 3;
    info.bits_per_sample = 8;
    
    if (JXL_ENC_SUCCESS != JxlEncoderSetBasicInfo(enc.get(), &info)) {
        return false;
    }
    
    JxlColorEncoding color_encoding;
    JxlColorEncodingSetToSRGB(&color_encoding, false);
    if (JXL_ENC_SUCCESS != JxlEncoderSetColorEncoding(enc.get(), &color_encoding)) {
        return false;
    }
    
    JxlEncoderFrameSettings* settings = JxlEncoderFrameSettingsCreate(enc.get(), nullptr);
    if (!settings) return false;
    
    JxlEncoderSetFrameLossless(settings, false);
    JxlEncoderSetFrameDistance(settings, 1.0); // Visual distance
    JxlEncoderFrameSettingsSetOption(settings, JXL_ENC_FRAME_SETTING_EFFORT, 4);
    JxlEncoderFrameSettingsSetOption(settings, JXL_ENC_FRAME_SETTING_DECODING_SPEED, 1);
    
    JxlPixelFormat format = {3, JXL_TYPE_UINT8, JXL_NATIVE_ENDIAN, 0};
    if (JXL_ENC_SUCCESS != JxlEncoderAddImageFrame(settings, &format, rgb_data, width * height * 3)) {
        return false;
    }
    
    JxlEncoderCloseInput(enc.get());
    
    jxl_data.resize(64 * 1024);
    uint8_t* next_out = jxl_data.data();
    size_t avail_out = jxl_data.size();
    
    for (;;) {
        JxlEncoderStatus status = JxlEncoderProcessOutput(enc.get(), &next_out, &avail_out);
        if (status == JXL_ENC_NEED_MORE_OUTPUT) {
            size_t offset = next_out - jxl_data.data();
            jxl_data.resize(jxl_data.size() * 2);
            next_out = jxl_data.data() + offset;
            avail_out = jxl_data.size() - offset;
        } else if (status == JXL_ENC_SUCCESS) {
            jxl_data.resize(next_out - jxl_data.data());
            break;
        } else {
            return false;
        }
    }
    
    return true;
}

// ---------------------------------------------------------
// ASYNC WORKERS
// ---------------------------------------------------------

// 1. GetJxlInfoAsyncWorker
class GetJxlInfoAsyncWorker : public Napi::AsyncWorker {
public:
    GetJxlInfoAsyncWorker(const Napi::Buffer<uint8_t>& buffer, const Napi::Function& callback)
        : Napi::AsyncWorker(callback), width(0), height(0), orientation(0), status_code(1) {
        input_data.assign(buffer.Data(), buffer.Data() + buffer.Length());
    }

    void Execute() override {
        JxlDecoderPtr dec(JxlDecoderCreate(nullptr));
        if (!dec) {
            status_code = 5002;
            SetError("Failed to create decoder");
            return;
        }
        if (JXL_DEC_SUCCESS != JxlDecoderSubscribeEvents(dec.get(), JXL_DEC_BASIC_INFO)) {
            status_code = 5002;
            SetError("Failed to subscribe decoder events");
            return;
        }
        JxlDecoderSetInput(dec.get(), input_data.data(), input_data.size());
        JxlDecoderCloseInput(dec.get());
        JxlDecoderStatus status = JxlDecoderProcessInput(dec.get());
        if (status == JXL_DEC_BASIC_INFO) {
            JxlBasicInfo info;
            if (JXL_DEC_SUCCESS == JxlDecoderGetBasicInfo(dec.get(), &info)) {
                width = info.xsize;
                height = info.ysize;
                orientation = info.orientation;
            } else {
                status_code = 5005;
                SetError("JxlDecoderGetBasicInfo failed");
            }
        } else if (status == JXL_DEC_SUCCESS) {
            status_code = 5004;
            SetError("JXL basic info not found");
        } else if (status == JXL_DEC_ERROR) {
            status_code = 5003;
            SetError("Decoder error");
        } else {
            status_code = 5006;
            SetError("Failed to parse JXL basic info");
        }
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        Napi::Object obj = Napi::Object::New(Env());
        obj.Set("width", Napi::Number::New(Env(), width));
        obj.Set("height", Napi::Number::New(Env(), height));
        obj.Set("orientation", Napi::Number::New(Env(), orientation));
        Callback().Call({Env().Null(), obj, Napi::Number::New(Env(), status_code)});
    }

    void OnError(const Napi::Error& err) override {
        Napi::HandleScope scope(Env());
        Callback().Call({err.Value(), Env().Undefined(), Napi::Number::New(Env(), status_code)});
    }

private:
    std::vector<uint8_t> input_data;
    uint32_t width;
    uint32_t height;
    uint32_t orientation;
    int status_code;
};

// 2. JxlToJpegAsyncWorker
class JxlToJpegAsyncWorker : public Napi::AsyncWorker {
public:
    JxlToJpegAsyncWorker(const Napi::Buffer<uint8_t>& buffer, int quality, int out_width, int out_height, int max_threads, const Napi::Function& callback)
        : Napi::AsyncWorker(callback), quality(quality), out_width(out_width), out_height(out_height), max_threads(max_threads), status_code(1) {
        input_data.assign(buffer.Data(), buffer.Data() + buffer.Length());
    }

    void Execute() override {
        std::vector<uint8_t> rgb;
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> icc;
        if (!decodeJXL(input_data.data(), input_data.size(), rgb, width, height, icc, max_threads)) {
            status_code = 2003; // Matches decompile error status
            SetError("Failed to decode JXL image");
            return;
        }

        std::vector<uint8_t> resized_rgb;
        int target_width = width;
        int target_height = height;

        if (out_width > 0 || out_height > 0) {
            uint32_t cw = width, ch = height;
            clampSize(width, height, out_width > 0 ? out_width : width, out_height > 0 ? out_height : height, &cw, &ch);
            target_width = cw;
            target_height = ch;
            if (cw != width || ch != height) {
                if (!resizeImage(rgb, width, height, cw, ch, resized_rgb)) {
                    status_code = 4004; // Resize failure status
                    SetError("Failed to resize image");
                    return;
                }
            }
        }

        const std::vector<uint8_t>& final_rgb = resized_rgb.empty() ? rgb : resized_rgb;
        if (!encodeJPEG(final_rgb, target_width, target_height, quality, jpeg_output, icc)) {
            status_code = 3001; // JPEG encode failure status
            SetError("Failed to encode JPEG image");
            return;
        }
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        Napi::Buffer<uint8_t> data = Napi::Buffer<uint8_t>::Copy(Env(), jpeg_output.data(), jpeg_output.size());
        Callback().Call({Env().Null(), data, Napi::Number::New(Env(), status_code)});
    }

    void OnError(const Napi::Error& err) override {
        Napi::HandleScope scope(Env());
        Callback().Call({err.Value(), Env().Undefined(), Napi::Number::New(Env(), status_code)});
    }

private:
    std::vector<uint8_t> input_data;
    int quality;
    int out_width;
    int out_height;
    int max_threads;
    int status_code;
    std::vector<uint8_t> jpeg_output;
};

// 3. BitmapToJxlAsyncWorker
class BitmapToJxlAsyncWorker : public Napi::AsyncWorker {
public:
    BitmapToJxlAsyncWorker(const Napi::Buffer<uint8_t>& buffer, int width, int height, int max_threads, const Napi::Function& callback)
        : Napi::AsyncWorker(callback), width(width), height(height), max_threads(max_threads), status_code(1) {
        input_data.assign(buffer.Data(), buffer.Data() + buffer.Length());
    }

    void Execute() override {
        if (!encodeJXL(input_data.data(), width, height, jxl_output, max_threads)) {
            status_code = 1006;
            SetError("Failed to compress JXL image");
        }
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        Napi::Buffer<uint8_t> data = Napi::Buffer<uint8_t>::Copy(Env(), jxl_output.data(), jxl_output.size());
        Callback().Call({Env().Null(), data, Napi::Number::New(Env(), status_code)});
    }

    void OnError(const Napi::Error& err) override {
        Napi::HandleScope scope(Env());
        Callback().Call({err.Value(), Env().Undefined(), Napi::Number::New(Env(), status_code)});
    }

private:
    std::vector<uint8_t> input_data;
    int width;
    int height;
    int max_threads;
    int status_code;
    std::vector<uint8_t> jxl_output;
};

// 4. ResizeJxlAsyncWorker
class ResizeJxlAsyncWorker : public Napi::AsyncWorker {
public:
    ResizeJxlAsyncWorker(const Napi::Buffer<uint8_t>& buffer, int out_width, int out_height, int max_threads, const Napi::Function& callback)
        : Napi::AsyncWorker(callback), out_width(out_width), out_height(out_height), max_threads(max_threads), status_code(1) {
        input_data.assign(buffer.Data(), buffer.Data() + buffer.Length());
    }

    void Execute() override {
        std::vector<uint8_t> rgb;
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> icc;
        if (!decodeJXL(input_data.data(), input_data.size(), rgb, width, height, icc, max_threads)) {
            status_code = 2003;
            SetError("Failed to decode JXL image");
            return;
        }

        std::vector<uint8_t> resized_rgb;
        uint32_t target_width = width;
        uint32_t target_height = height;

        // Custom clamp resizing logic matching decompile
        if (out_width > 0 || out_height > 0) {
            clampSize(width, height, out_width > 0 ? out_width : width, out_height > 0 ? out_height : height, &target_width, &target_height);
            if (target_width != width || target_height != height) {
                if (!resizeImage(rgb, width, height, target_width, target_height, resized_rgb)) {
                    status_code = 4004;
                    SetError("Failed to resize image");
                    return;
                }
            }
        }

        const std::vector<uint8_t>& final_rgb = resized_rgb.empty() ? rgb : resized_rgb;
        if (!encodeJXL(final_rgb.data(), target_width, target_height, jxl_output, max_threads)) {
            status_code = 1006;
            SetError("Failed to compress resized image back to JXL");
        }
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        Napi::Buffer<uint8_t> data = Napi::Buffer<uint8_t>::Copy(Env(), jxl_output.data(), jxl_output.size());
        Callback().Call({Env().Null(), data, Napi::Number::New(Env(), status_code)});
    }

    void OnError(const Napi::Error& err) override {
        Napi::HandleScope scope(Env());
        Callback().Call({err.Value(), Env().Undefined(), Napi::Number::New(Env(), status_code)});
    }

private:
    std::vector<uint8_t> input_data;
    int out_width;
    int out_height;
    int max_threads;
    int status_code;
    std::vector<uint8_t> jxl_output;
};

// 5. JxlDecompressMultiAsyncWorker
struct MultiTask {
    int width = -1;
    int height = -1;
    int maxWidth = -1;
    int maxHeight = -1;
    std::string outputPath = "";
};

struct MultiResult {
    std::vector<uint8_t> data;
    std::string outputPath = "";
    int width = 0;
    int height = 0;
};

class JxlDecompressMultiAsyncWorker : public Napi::AsyncWorker {
public:
    JxlDecompressMultiAsyncWorker(const std::vector<uint8_t>& buffer, const std::string& localPath, const std::vector<MultiTask>& tasks, int quality, int max_threads, const Napi::Function& callback)
        : Napi::AsyncWorker(callback), input_data(buffer), local_path(localPath), tasks(tasks), quality(quality), max_threads(max_threads), status_code(1) {}

    void Execute() override {
        std::vector<uint8_t> raw_jxl;
        if (!local_path.empty()) {
            std::ifstream file(local_path, std::ios::binary);
            if (!file.is_open()) {
                status_code = 2001; // Failed to open file
                SetError("Failed to open local JXL file");
                return;
            }
            raw_jxl.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        } else {
            raw_jxl = input_data;
        }

        if (raw_jxl.empty()) {
            status_code = 2002; // Empty file/buffer
            SetError("Input JXL data is empty");
            return;
        }

        std::vector<uint8_t> rgb;
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> icc;
        if (!decodeJXL(raw_jxl.data(), raw_jxl.size(), rgb, width, height, icc, max_threads)) {
            status_code = 2003;
            SetError("Failed to decode JXL image");
            return;
        }

        for (const auto& task : tasks) {
            uint32_t target_w = width;
            uint32_t target_h = height;
            
            // Determine target dimensions
            if (task.width > 0 && task.height > 0) {
                target_w = task.width;
                target_h = task.height;
            } else if (task.maxWidth > 0 || task.maxHeight > 0) {
                clampSize(width, height, task.maxWidth > 0 ? task.maxWidth : width, task.maxHeight > 0 ? task.maxHeight : height, &target_w, &target_h);
            }

            std::vector<uint8_t> task_rgb;
            if (target_w != width || target_h != height) {
                if (!resizeImage(rgb, width, height, target_w, target_h, task_rgb)) {
                    status_code = 4004;
                    SetError("Failed to resize image for task");
                    return;
                }
            }

            const std::vector<uint8_t>& final_rgb = task_rgb.empty() ? rgb : task_rgb;
            std::vector<uint8_t> jpeg_out;
            if (!encodeJPEG(final_rgb, target_w, target_h, quality, jpeg_out, icc)) {
                status_code = 3001;
                SetError("Failed to encode JPEG for task");
                return;
            }

            MultiResult res;
            res.width = target_w;
            res.height = target_h;
            res.outputPath = task.outputPath;

            if (!task.outputPath.empty()) {
                std::ofstream outfile(task.outputPath, std::ios::binary);
                if (outfile.is_open()) {
                    outfile.write(reinterpret_cast<const char*>(jpeg_out.data()), jpeg_out.size());
                } else {
                    status_code = 3002; // Write file failed
                    SetError("Failed to write output JPEG to " + task.outputPath);
                    return;
                }
            } else {
                res.data = std::move(jpeg_out);
            }
            results.push_back(res);
        }
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        Napi::Array arr = Napi::Array::New(Env(), results.size());
        for (size_t i = 0; i < results.size(); ++i) {
            Napi::Object obj = Napi::Object::New(Env());
            if (results[i].outputPath.empty()) {
                Napi::Buffer<uint8_t> buf = Napi::Buffer<uint8_t>::Copy(Env(), results[i].data.data(), results[i].data.size());
                obj.Set("data", buf);
                obj.Set("size", Napi::Number::New(Env(), results[i].data.size()));
            } else {
                obj.Set("data", Env().Undefined());
                obj.Set("size", Napi::Number::New(Env(), 0));
            }
            obj.Set("outputPath", Napi::String::New(Env(), results[i].outputPath));
            obj.Set("width", Napi::Number::New(Env(), results[i].width));
            obj.Set("height", Napi::Number::New(Env(), results[i].height));
            arr.Set(i, obj);
        }
        Callback().Call({Env().Null(), arr, Napi::Number::New(Env(), status_code)});
    }

    void OnError(const Napi::Error& err) override {
        Napi::HandleScope scope(Env());
        Callback().Call({err.Value(), Env().Undefined(), Napi::Number::New(Env(), status_code)});
    }

private:
    std::vector<uint8_t> input_data;
    std::string local_path;
    std::vector<MultiTask> tasks;
    int quality;
    int max_threads;
    int status_code;
    std::vector<MultiResult> results;
};

// ---------------------------------------------------------
// NAPI WRAPPERS
// ---------------------------------------------------------

Napi::Value jxlToJpegWrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Napi::Object opts = info[0].As<Napi::Object>();
    Napi::Function cb = info[1].As<Napi::Function>();

    Napi::Buffer<uint8_t> buf = opts.Get("buffer").As<Napi::Buffer<uint8_t>>();
    float quality_f = opts.Has("quality") ? opts.Get("quality").As<Napi::Number>().FloatValue() : 0.95f;
    int quality = static_cast<int>(quality_f * 100.0f);
    if (quality <= 0) quality = 95;

    int out_width = opts.Has("outputWidth") ? opts.Get("outputWidth").As<Napi::Number>().Int32Value() : -1;
    int out_height = opts.Has("outputHeight") ? opts.Get("outputHeight").As<Napi::Number>().Int32Value() : -1;
    int max_threads = opts.Has("maxThreads") ? opts.Get("maxThreads").As<Napi::Number>().Int32Value() : 8;

    auto* worker = new JxlToJpegAsyncWorker(buf, quality, out_width, out_height, max_threads, cb);
    worker->Queue();
    return env.Undefined();
}

Napi::Value bitmapToJxlWrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Napi::Object opts = info[0].As<Napi::Object>();
    Napi::Function cb = info[1].As<Napi::Function>();

    Napi::Buffer<uint8_t> buf = opts.Get("buffer").As<Napi::Buffer<uint8_t>>();
    int width = opts.Get("width").As<Napi::Number>().Int32Value();
    int height = opts.Get("height").As<Napi::Number>().Int32Value();
    int max_threads = opts.Has("maxThreads") ? opts.Get("maxThreads").As<Napi::Number>().Int32Value() : 8;

    auto* worker = new BitmapToJxlAsyncWorker(buf, width, height, max_threads, cb);
    worker->Queue();
    return env.Undefined();
}

Napi::Value getJxlInfoWrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Napi::Object opts = info[0].As<Napi::Object>();
    Napi::Function cb = info[1].As<Napi::Function>();

    Napi::Buffer<uint8_t> buf = opts.Get("buffer").As<Napi::Buffer<uint8_t>>();

    auto* worker = new GetJxlInfoAsyncWorker(buf, cb);
    worker->Queue();
    return env.Undefined();
}

Napi::Value resizeJxlWrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Napi::Object opts = info[0].As<Napi::Object>();
    Napi::Function cb = info[1].As<Napi::Function>();

    Napi::Buffer<uint8_t> buf = opts.Get("buffer").As<Napi::Buffer<uint8_t>>();
    int width = opts.Get("width").As<Napi::Number>().Int32Value();
    int height = opts.Get("height").As<Napi::Number>().Int32Value();
    int max_threads = opts.Has("maxThreads") ? opts.Get("maxThreads").As<Napi::Number>().Int32Value() : 8;

    auto* worker = new ResizeJxlAsyncWorker(buf, width, height, max_threads, cb);
    worker->Queue();
    return env.Undefined();
}

Napi::Value jxlDecompressMultiWrapper(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    Napi::Object opts = info[0].As<Napi::Object>();
    Napi::Function cb = info[1].As<Napi::Function>();

    std::vector<uint8_t> buf;
    if (opts.Has("buffer")) {
        Napi::Buffer<uint8_t> nbuf = opts.Get("buffer").As<Napi::Buffer<uint8_t>>();
        buf.assign(nbuf.Data(), nbuf.Data() + nbuf.Length());
    }

    std::string local_path = opts.Has("localPath") ? opts.Get("localPath").As<Napi::String>().Utf8Value() : "";
    float quality_f = opts.Has("quality") ? opts.Get("quality").As<Napi::Number>().FloatValue() : 0.95f;
    int max_threads = opts.Has("maxThreads") ? opts.Get("maxThreads").As<Napi::Number>().Int32Value() : 8;
    int quality = static_cast<int>(quality_f * 100.0f);
    if (quality <= 0) quality = 95;

    std::vector<MultiTask> tasks;
    if (opts.Has("tasks")) {
        Napi::Array ntasks = opts.Get("tasks").As<Napi::Array>();
        for (uint32_t i = 0; i < ntasks.Length(); ++i) {
            Napi::Object tobj = ntasks.Get(i).As<Napi::Object>();
            MultiTask t;
            t.width = tobj.Has("width") ? tobj.Get("width").As<Napi::Number>().Int32Value() : -1;
            t.height = tobj.Has("height") ? tobj.Get("height").As<Napi::Number>().Int32Value() : -1;
            t.maxWidth = tobj.Has("maxWidth") ? tobj.Get("maxWidth").As<Napi::Number>().Int32Value() : -1;
            t.maxHeight = tobj.Has("maxHeight") ? tobj.Get("maxHeight").As<Napi::Number>().Int32Value() : -1;
            t.outputPath = tobj.Has("outputPath") ? tobj.Get("outputPath").As<Napi::String>().Utf8Value() : "";
            tasks.push_back(t);
        }
    }

    auto* worker = new JxlDecompressMultiAsyncWorker(buf, local_path, tasks, quality, max_threads, cb);
    worker->Queue();
    return env.Undefined();
}

Napi::Value moduleReadyWrapper(const Napi::CallbackInfo& info) {
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("jxlToJpeg", Napi::Function::New(env, jxlToJpegWrapper));
    exports.Set("bitmapToJxl", Napi::Function::New(env, bitmapToJxlWrapper));
    exports.Set("getJxlInfo", Napi::Function::New(env, getJxlInfoWrapper));
    exports.Set("resizeJxl", Napi::Function::New(env, resizeJxlWrapper));
    exports.Set("resizeJxlLimit", Napi::Function::New(env, resizeJxlWrapper)); // map to same function
    exports.Set("jxlDecompressMulti", Napi::Function::New(env, jxlDecompressMultiWrapper));
    exports.Set("moduleReady", Napi::Function::New(env, moduleReadyWrapper));
    return exports;
}

NODE_API_MODULE(jxl, Init)
