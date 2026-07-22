#include <napi.h>

#include <vips/vips.h>
#include <vips/vector.h>
#include <vips/vips8>

#include <algorithm>
#include <mutex>
#include <string>
#include <unistd.h>

#include "buffer_thumbnail_worker.h"
#include "file_thumbnail_worker.h"

namespace {

std::once_flag vips_initialization_once;
bool vips_initialized = false;
std::string vips_initialization_error;

void InitializeVipsOnce() {
  if (vips_init("zimage") != 0) {
    const char* message = vips_error_buffer();
    vips_initialization_error =
        message == nullptr || *message == '\0' ? "Failed to initialize libvips"
                                             : message;
    vips_error_clear();
    return;
  }

  vips_concurrency_set(1);
  vips_vector_set_enabled(true);

  long pages = sysconf(_SC_PHYS_PAGES);
  long page_size = sysconf(_SC_PAGE_SIZE);
  size_t total_memory_bytes = static_cast<size_t>(pages) * static_cast<size_t>(page_size);

  size_t max_cache_mem = total_memory_bytes / 10;
  size_t min_cache = 128 * 1024 * 1024;
  size_t max_cache = 1024 * 1024 * 1024;

  size_t allocated_cache_mem = std::clamp(max_cache_mem, min_cache, max_cache);

  vips_cache_set_max(100);
  vips_cache_set_max_mem(allocated_cache_mem);
  vips_cache_set_max_files(50);
  vips_initialized = true;
}

std::string GetStringProp(const Napi::Object& obj, const char* key1, const char* key2 = nullptr) {
  if (obj.Has(key1)) {
    Napi::Value val = obj.Get(key1);
    if (val.IsString()) return val.As<Napi::String>().Utf8Value();
  }
  if (key2 != nullptr && obj.Has(key2)) {
    Napi::Value val = obj.Get(key2);
    if (val.IsString()) return val.As<Napi::String>().Utf8Value();
  }
  return "";
}

int GetIntProp(const Napi::Object& obj, const char* key, int default_val = 0) {
  if (obj.Has(key)) {
    Napi::Value val = obj.Get(key);
    if (val.IsNumber()) return val.As<Napi::Number>().Int32Value();
    if (val.IsString()) {
      try {
        return std::stoi(val.As<Napi::String>().Utf8Value());
      } catch (...) {}
    }
  }
  return default_val;
}

Napi::Function GetCallback(const Napi::CallbackInfo& info) {
  if (info.Length() > 1 && info[1].IsFunction()) {
    return info[1].As<Napi::Function>();
  }
  return Napi::Function::New(info.Env(), [](const Napi::CallbackInfo&) {});
}

Napi::Value Thumbnail(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "Expected options object and callback")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Object options = info[0].As<Napi::Object>();
  if (!options.Has("buffer") || !options.Get("buffer").IsBuffer()) {
    Napi::TypeError::New(env, "options requires a valid buffer")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  int width = GetIntProp(options, "width", 0);
  int height = GetIntProp(options, "height", 0);
  int quality = GetIntProp(options, "quality", 80);
  std::string format = GetStringProp(options, "format");
  if (format.empty()) {
    format = "png";
  }

  Napi::Function callback = GetCallback(info);
  auto* worker = new BufferThumbnailWorker(
      callback, options.Get("buffer").As<Napi::Buffer<char>>(),
      width, height, format, quality);
  worker->Queue();
  return env.Undefined();
}

Napi::Value ThumbnailFs(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "Expected options object and callback")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Object options = info[0].As<Napi::Object>();
  std::string input_path = GetStringProp(options, "inputPath", "input_path");
  std::string output_path = GetStringProp(options, "outputPath", "output_path");
  int width = GetIntProp(options, "width", 0);
  int height = GetIntProp(options, "height", 0);
  int quality = GetIntProp(options, "quality", 80);

  if (input_path.empty() || output_path.empty()) {
    Napi::TypeError::New(env, "options requires inputPath and outputPath")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Function callback = GetCallback(info);
  auto* worker = new FileThumbnailWorker(
      callback, input_path, output_path, width, height, quality);
  worker->Queue();
  return env.Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  std::call_once(vips_initialization_once, InitializeVipsOnce);
  if (!vips_initialized) {
    Napi::Error::New(env, vips_initialization_error).ThrowAsJavaScriptException();
    return exports;
  }

  exports.Set("thumbnail", Napi::Function::New(env, Thumbnail));
  exports.Set("thumbnailFs", Napi::Function::New(env, ThumbnailFs));
  return exports;
}

}  // namespace

NODE_API_MODULE(zimage, Init)