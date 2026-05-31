#include <napi.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdio>

// Bắt buộc bọc các header của FFmpeg trong extern "C" vì FFmpeg viết bằng C
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

class MP4Thumb : public Napi::ObjectWrap<MP4Thumb> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    MP4Thumb(const Napi::CallbackInfo& info);

private:
    // Các phương thức được export sang JavaScript
    Napi::Value GenerateThumbnail(const Napi::CallbackInfo& info);
    Napi::Value GenerateThumbnailAsync(const Napi::CallbackInfo& info);
    Napi::Value SetOutputPath(const Napi::CallbackInfo& info);
    Napi::Value Cancel(const Napi::CallbackInfo& info);

    std::string m_defaultOutputPath;
    bool m_cancelled;

    // Hàm lõi xử lý trích xuất và lưu ảnh bằng FFmpeg (Dùng chung cho cả Đồng bộ & Bất đồng bộ)
    static bool DoExtract(const std::string& input, const std::string& output, 
                          int maxWidth, int maxHeight, std::string& errorMsg, bool& cancelledRef);
    
    // Khai báo lớp Worker đảm nhận xử lý chạy ngầm (Bất đồng bộ)
    class ThumbnailWorker : public Napi::AsyncWorker {
    public:
        ThumbnailWorker(Napi::Function& callback, std::string input, std::string output, 
                        int width, int height, bool& cancelledRef)
            : Napi::AsyncWorker(callback), inputPath(input), outputPath(output), 
              maxWidth(width), maxHeight(height), cancelled(cancelledRef) {}

        void Execute() override {
            std::string errorMsg;
            if (!MP4Thumb::DoExtract(inputPath, outputPath, maxWidth, maxHeight, errorMsg, cancelled)) {
                SetError(errorMsg.empty() ? "Generation failed ngầm định." : errorMsg);
            }
        }

        void OnOK() override {
            Napi::HandleScope scope(Env());
            Callback().Call({Env().Null(), Napi::Boolean::New(Env(), true)});
        }
    private:
        std::string inputPath, outputPath;
        int maxWidth, maxHeight;
        bool& cancelled;
    };
};

// Khởi tạo và map Class C++ với JavaScript
Napi::Object MP4Thumb::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "MP4Thumb", {
        InstanceMethod("generateThumbnail", &MP4Thumb::GenerateThumbnail),
        InstanceMethod("generateThumbnailAsync", &MP4Thumb::GenerateThumbnailAsync),
        InstanceMethod("setOutputPath", &MP4Thumb::SetOutputPath),
        InstanceMethod("cancel", &MP4Thumb::Cancel)
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("MP4Thumb", func);
    return exports;
}

MP4Thumb::MP4Thumb(const Napi::CallbackInfo& info) : Napi::ObjectWrap<MP4Thumb>(info), m_cancelled(false) {}

Napi::Value MP4Thumb::SetOutputPath(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() > 0 && info[0].IsString()) {
        m_defaultOutputPath = info[0].As<Napi::String>().Utf8Value();
    }
    return env.Undefined();
}

Napi::Value MP4Thumb::Cancel(const Napi::CallbackInfo& info) {
    m_cancelled = true;
    return info.Env().Undefined();
}

// Xử lý đồng bộ (Synchronous)
Napi::Value MP4Thumb::GenerateThumbnail(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "String input path expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string inputPath = info[0].As<Napi::String>().Utf8Value();
    std::string outputPath = m_defaultOutputPath;
    int maxWidth = 0, maxHeight = 0;

    if (info.Length() > 1 && info[1].IsString()) {
        outputPath = info[1].As<Napi::String>().Utf8Value();
    }
    if (info.Length() > 2 && info[2].IsObject()) {
        Napi::Object opts = info[2].As<Napi::Object>();
        if (opts.Has("maxWidth")) maxWidth = opts.Get("maxWidth").As<Napi::Number>().Int32Value();
        if (opts.Has("maxHeight")) maxHeight = opts.Get("maxHeight").As<Napi::Number>().Int32Value();
    }

    if (outputPath.empty()) {
        Napi::Error::New(env, "Output path is empty.").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string errorMsg;
    m_cancelled = false;
    if (!DoExtract(inputPath, outputPath, maxWidth, maxHeight, errorMsg, m_cancelled)) {
        Napi::Error::New(env, errorMsg).ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, true);
}

// Xử lý bất đồng bộ (Asynchronous) thông qua Thread Pool của libuv
Napi::Value MP4Thumb::GenerateThumbnailAsync(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    std::string inputPath = info[0].As<Napi::String>().Utf8Value();
    std::string outputPath = m_defaultOutputPath;
    int maxWidth = 0, maxHeight = 0;
    Napi::Function callback;

    // Phân tách động các tham số truyền vào từ JS
    if (info[1].IsString()) {
        outputPath = info[1].As<Napi::String>().Utf8Value();
        if (info[2].IsObject()) {
            Napi::Object opts = info[2].As<Napi::Object>();
            if (opts.Has("maxWidth")) maxWidth = opts.Get("maxWidth").As<Napi::Number>().Int32Value();
            if (opts.Has("maxHeight")) maxHeight = opts.Get("maxHeight").As<Napi::Number>().Int32Value();
            callback = info[3].As<Napi::Function>();
        } else {
            callback = info[2].As<Napi::Function>();
        }
    } else if (info[1].IsObject()) {
        Napi::Object opts = info[1].As<Napi::Object>();
        if (opts.Has("maxWidth")) maxWidth = opts.Get("maxWidth").As<Napi::Number>().Int32Value();
        if (opts.Has("maxHeight")) maxHeight = opts.Get("maxHeight").As<Napi::Number>().Int32Value();
        callback = info[2].As<Napi::Function>();
    }

    m_cancelled = false;
    ThumbnailWorker* worker = new ThumbnailWorker(callback, inputPath, outputPath, maxWidth, maxHeight, m_cancelled);
    worker->Queue(); // Đẩy vào luồng ngầm xử lý
    return env.Undefined();
}

// LÕI THUẬT TOÁN KỸ THUẬT NGƯỢC (FFmpeg API)
bool MP4Thumb::DoExtract(const std::string& input, const std::string& output, 
                         int maxWidth, int maxHeight, std::string& errorMsg, bool& cancelledRef) {
    AVFormatContext* pFormatCtx = nullptr;
    if (avformat_open_input(&pFormatCtx, input.c_str(), nullptr, nullptr) != 0) {
        errorMsg = "Không thể mở file MP4 đầu vào.";
        return false;
    }

    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        errorMsg = "Không tìm thấy thông tin stream.";
        avformat_close_input(&pFormatCtx);
        return false;
    }

    int videoStreamIdx = -1;
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIdx = i;
            break;
        }
    }

    if (videoStreamIdx == -1) {
        errorMsg = "Không tìm thấy luồng dữ liệu Video.";
        avformat_close_input(&pFormatCtx);
        return false;
    }

    AVCodecParameters* pCodecPar = pFormatCtx->streams[videoStreamIdx]->codecpar;
    const AVCodec* pCodec = avcodec_find_decoder(pCodecPar->codec_id);
    AVCodecContext* pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pCodecPar);

    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        errorMsg = "Không thể mở bộ giải mã.";
        avcodec_free_context(&pCodecCtx);
        avformat_close_input(&pFormatCtx);
        return false;
    }

    // RE Rule: Nhảy qua 1 giây đầu tiên để tránh khung hình đen (Black Frame) thường gặp ở đầu video
    int64_t seek_target = av_rescale_q(1ULL * AV_TIME_BASE, AV_TIME_BASE_Q, pFormatCtx->streams[videoStreamIdx]->time_base);
    av_seek_frame(pFormatCtx, videoStreamIdx, seek_target, AVSEEK_FLAG_BACKWARD);

    AVPacket* pPacket = av_packet_alloc();
    AVFrame* pFrame = av_frame_alloc();
    bool frameDecoded = false;

    while (av_read_frame(pFormatCtx, pPacket) >= 0) {
        if (cancelledRef) {
            errorMsg = "Tiến trình bị hủy bởi người dùng.";
            break;
        }
        if (pPacket->stream_index == videoStreamIdx) {
            if (avcodec_send_packet(pCodecCtx, pPacket) == 0) {
                if (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
                    frameDecoded = true;
                    av_packet_unref(pPacket);
                    break;
                }
            }
        }
        av_packet_unref(pPacket);
    }

    if (!frameDecoded) {
        if (errorMsg.empty()) errorMsg = "Không giải mã được khung hình nào.";
        av_frame_free(&pFrame);
        av_packet_free(&pPacket);
        avcodec_free_context(&pCodecCtx);
        avformat_close_input(&pFormatCtx);
        return false;
    }

    // Tính toán kích thước thu nhỏ (giữ nguyên Tỷ lệ khía cạnh - Aspect Ratio)
    int targetWidth = pCodecCtx->width;
    int targetHeight = pCodecCtx->height;
    if (maxWidth > 0 && maxHeight > 0) {
        double scale = std::min((double)maxWidth / targetWidth, (double)maxHeight / targetHeight);
        if (scale < 1.0) {
            targetWidth = (int)(targetWidth * scale);
            targetHeight = (int)(targetHeight * scale);
        }
    }
    targetWidth = (targetWidth >> 1) << 1;   // Đảm bảo kích thước chẵn để tránh lỗi codec
    targetHeight = (targetHeight >> 1) << 1;

    // Cấu hình không gian màu và tỷ lệ (SwsScale)
    AVFrame* pFrameYUV = av_frame_alloc();
    pFrameYUV->width = targetWidth;
    pFrameYUV->height = targetHeight;
    pFrameYUV->format = AV_PIX_FMT_YUV420P;
    av_image_alloc(pFrameYUV->data, pFrameYUV->linesize, targetWidth, targetHeight, AV_PIX_FMT_YUV420P, 32);

    SwsContext* swsCtx = sws_getContext(
        pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        targetWidth, targetHeight, AV_PIX_FMT_YUV420P,
        SWS_BICUBIC, nullptr, nullptr, nullptr
    );
    sws_scale(swsCtx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

    // Sử dụng Bộ mã hóa MJPEG nội bộ của FFmpeg để ghi ảnh đầu ra (.jpg)
    const AVCodec* pJpegCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    bool writeSuccess = false;
    if (pJpegCodec) {
        AVCodecContext* pJpegCtx = avcodec_alloc_context3(pJpegCodec);
        pJpegCtx->width = targetWidth;
        pJpegCtx->height = targetHeight;
        pJpegCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
        pJpegCtx->time_base = {1, 25};

        if (avcodec_open2(pJpegCtx, pJpegCodec, nullptr) == 0) {
            AVPacket* pJpegPacket = av_packet_alloc();
            pFrameYUV->pts = 0;
            if (avcodec_send_frame(pJpegCtx, pFrameYUV) == 0) {
                if (avcodec_receive_packet(pJpegCtx, pJpegPacket) == 0) {
                    FILE* f = fopen(output.c_str(), "wb");
                    if (f) {
                        fwrite(pJpegPacket->data, 1, pJpegPacket->size, f);
                        fclose(f);
                        writeSuccess = true;
                    }
                }
            }
            av_packet_free(&pJpegPacket);
            avcodec_free_context(&pJpegCtx);
        }
    }

    if (!writeSuccess && errorMsg.empty()) errorMsg = "Lỗi khi nén ảnh lưu dạng JPEG.";

    // Giải phóng bộ nhớ sạch sẽ
    sws_freeContext(swsCtx);
    av_freep(&pFrameYUV->data[0]);
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    av_packet_free(&pPacket);
    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return writeSuccess;
}

// Đăng ký Module gốc với Node.js runtime
Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    return MP4Thumb::Init(env, exports);
}
NODE_API_MODULE(mp4thumb, InitAll)