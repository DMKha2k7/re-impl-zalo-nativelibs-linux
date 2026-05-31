/**
 * zimage.node — Linux port of Zalo's zimage native addon
 *
 * RE of original macOS binary (libvips-based):
 *   Exports: thumbnail(args, callback), thumbnailFs(args, callback)
 *   Engine:  libvips 42 (C API: vips_thumbnail_buffer, vips_thumbnail)
 *
 * Build:  npm run build
 * Output: linux_x64/zimage.node + linux_x64/libvips.so.42
 */

#include <napi.h>
#include <vips/vips.h>
#include <string>
#include <vector>
#include <cstring>

// ─── Helpers ─────────────────────────────────────────────────────────────────

static std::string vips_error_str() {
    const char* e = vips_error_buffer();
    std::string s = e ? e : "unknown vips error";
    vips_error_clear();
    return s;
}

// Map format string → vips save function suffix
// supported: "jpeg"/"jpg" → JPEG, "png" → PNG, "webp" → WebP
// Default to JPEG when not recognized.
static std::string resolve_format(const std::string& fmt) {
    if (fmt == "png")  return "png";
    if (fmt == "webp") return "webp";
    return "jpeg";  // default
}

// ─── ThumbnailAsyncWorker: buffer → buffer ────────────────────────────────────

class ThumbnailWorker : public Napi::AsyncWorker {
public:
    ThumbnailWorker(Napi::Function& callback,
                    std::vector<uint8_t> input_buf,
                    int width, int height,
                    std::string format, int quality)
        : Napi::AsyncWorker(callback),
          input_(std::move(input_buf)),
          width_(width), height_(height),
          format_(std::move(format)), quality_(quality) {}

    ~ThumbnailWorker() override = default;

    void Execute() override {
        VipsImage* thumb = nullptr;

        // vips_thumbnail_buffer: resize from memory
        int ret = vips_thumbnail_buffer(
            input_.data(), input_.size(), &thumb,
            width_,
            "height",    height_,
            "size",      VIPS_SIZE_DOWN,
            nullptr
        );

        if (ret != 0 || !thumb) {
            SetError(vips_error_str());
            return;
        }

        // Save to buffer
        void*  out_buf = nullptr;
        size_t out_len = 0;
        std::string fmt = resolve_format(format_);

        if (fmt == "png") {
            ret = vips_pngsave_buffer(thumb, &out_buf, &out_len, nullptr);
        } else if (fmt == "webp") {
            ret = vips_webpsave_buffer(thumb, &out_buf, &out_len,
                                       "Q", quality_, nullptr);
        } else {
            // JPEG default
            ret = vips_jpegsave_buffer(thumb, &out_buf, &out_len,
                                       "Q", quality_, nullptr);
        }

        g_object_unref(thumb);

        if (ret != 0 || !out_buf) {
            SetError(vips_error_str());
            return;
        }

        // Copy output
        output_.assign(static_cast<uint8_t*>(out_buf),
                       static_cast<uint8_t*>(out_buf) + out_len);
        g_free(out_buf);
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        auto buf = Napi::Buffer<uint8_t>::Copy(Env(), output_.data(), output_.size());
        Callback().Call({Env().Null(), buf});
    }

    void OnError(const Napi::Error& e) override {
        Napi::HandleScope scope(Env());
        Callback().Call({e.Value(), Env().Undefined()});
    }

private:
    std::vector<uint8_t> input_;
    int width_, height_, quality_;
    std::string format_;
    std::vector<uint8_t> output_;
};

// ─── ThumbnailFsAsyncWorker: file path → file path ───────────────────────────

class ThumbnailFsWorker : public Napi::AsyncWorker {
public:
    ThumbnailFsWorker(Napi::Function& callback,
                      std::string input_path, std::string output_path,
                      int width, int height, int quality)
        : Napi::AsyncWorker(callback),
          input_path_(std::move(input_path)),
          output_path_(std::move(output_path)),
          width_(width), height_(height), quality_(quality) {}

    ~ThumbnailFsWorker() override = default;

    void Execute() override {
        VipsImage* thumb = nullptr;

        int ret = vips_thumbnail(
            input_path_.c_str(), &thumb,
            width_,
            "height", height_,
            "size",   VIPS_SIZE_DOWN,
            nullptr
        );

        if (ret != 0 || !thumb) {
            SetError("thumbnailFs: " + vips_error_str());
            return;
        }

        // Determine output format from extension
        std::string out = output_path_;
        std::string ext;
        auto dot = out.rfind('.');
        if (dot != std::string::npos) ext = out.substr(dot + 1);

        if (ext == "png") {
            ret = vips_image_write_to_file(thumb, out.c_str(), nullptr);
        } else if (ext == "webp") {
            ret = vips_image_write_to_file(thumb, out.c_str(), nullptr);
        } else {
            // JPEG default: pass quality option
            ret = vips_jpegsave(thumb, out.c_str(), "Q", quality_, nullptr);
        }

        vips_image_set_kill(thumb, TRUE);
        g_object_unref(thumb);

        if (ret != 0) {
            SetError("thumbnailFs save: " + vips_error_str());
        }
    }

    void OnOK() override {
        Napi::HandleScope scope(Env());
        Callback().Call({Env().Null()});
    }

    void OnError(const Napi::Error& e) override {
        Napi::HandleScope scope(Env());
        Callback().Call({e.Value()});
    }

private:
    std::string input_path_, output_path_;
    int width_, height_, quality_;
};

// ─── JS: thumbnail({buffer, width, height, format, quality}, callback) ────────

Napi::Value Thumbnail(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "thumbnail(args: object, callback: function)")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }

    auto args     = info[0].As<Napi::Object>();
    auto callback = info[1].As<Napi::Function>();

    // buffer
    if (!args.Has("buffer") || !args.Get("buffer").IsBuffer()) {
        Napi::TypeError::New(env, "args.buffer must be a Buffer")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    auto buf = args.Get("buffer").As<Napi::Buffer<uint8_t>>();

    // width / height
    int width  = args.Has("width")  && args.Get("width").IsNumber()
                 ? args.Get("width").As<Napi::Number>().Int32Value()  : 256;
    int height = args.Has("height") && args.Get("height").IsNumber()
                 ? args.Get("height").As<Napi::Number>().Int32Value() : 256;

    // format (optional)
    std::string format = "jpeg";
    if (args.Has("format") && args.Get("format").IsString()) {
        format = args.Get("format").As<Napi::String>().Utf8Value();
    }

    // quality (optional)
    int quality = 80;
    if (args.Has("quality") && args.Get("quality").IsNumber()) {
        quality = args.Get("quality").As<Napi::Number>().Int32Value();
    }

    // Copy buffer data
    std::vector<uint8_t> input(buf.Data(), buf.Data() + buf.ByteLength());

    auto* worker = new ThumbnailWorker(callback, std::move(input),
                                       width, height, format, quality);
    worker->Queue();
    return env.Undefined();
}

// ─── JS: thumbnailFs({inputPath, outputPath, width, height, quality}, callback)

Napi::Value ThumbnailFs(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 2 || !info[0].IsObject() || !info[1].IsFunction()) {
        Napi::TypeError::New(env, "thumbnailFs(args: object, callback: function)")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }

    auto args     = info[0].As<Napi::Object>();
    auto callback = info[1].As<Napi::Function>();

    // inputPath / outputPath
    if (!args.Has("inputPath") || !args.Get("inputPath").IsString()) {
        Napi::TypeError::New(env, "args.inputPath must be a string")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }
    if (!args.Has("outputPath") || !args.Get("outputPath").IsString()) {
        Napi::TypeError::New(env, "args.outputPath must be a string")
            .ThrowAsJavaScriptException();
        return env.Undefined();
    }

    std::string input_path  = args.Get("inputPath").As<Napi::String>().Utf8Value();
    std::string output_path = args.Get("outputPath").As<Napi::String>().Utf8Value();

    int width   = args.Has("width")   && args.Get("width").IsNumber()
                  ? args.Get("width").As<Napi::Number>().Int32Value()   : 256;
    int height  = args.Has("height")  && args.Get("height").IsNumber()
                  ? args.Get("height").As<Napi::Number>().Int32Value()  : 256;
    int quality = args.Has("quality") && args.Get("quality").IsNumber()
                  ? args.Get("quality").As<Napi::Number>().Int32Value() : 80;

    auto* worker = new ThumbnailFsWorker(callback,
                                         std::move(input_path),
                                         std::move(output_path),
                                         width, height, quality);
    worker->Queue();
    return env.Undefined();
}

// ─── Module init ─────────────────────────────────────────────────────────────

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    // Initialize vips (safe to call multiple times)
    if (vips_init("zimage") != 0) {
        // Non-fatal: vips may already be initialized
        vips_error_clear();
    }

    // Match original: no caching
    vips_cache_set_max(0);
    vips_cache_set_max_mem(0);
    vips_cache_set_max_files(0);

    exports.Set("thumbnail",   Napi::Function::New(env, Thumbnail));
    exports.Set("thumbnailFs", Napi::Function::New(env, ThumbnailFs));
    return exports;
}

NODE_API_MODULE(zimage, Init)
