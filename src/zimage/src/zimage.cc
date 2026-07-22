#include <napi.h>

#include <vips/vips.h>
#include <vips/vector.h>
#include <vips/vips8>

#include <mutex>
#include <string>

#include "buffer_thumbnail_worker.h"
#include "file_thumbnail_worker.h"

namespace {

std::once_flag vips_initialization_once;
bool vips_initialized = false;
std::string vips_initialization_error;

void InitializeVipsOnce() {
  if (vips_init("sharp") != 0) {
    const char* message = vips_error_buffer();
    vips_initialization_error =
        message == nullptr || *message == '\0' ? "Failed to initialize libvips"
                                             : message;
    vips_error_clear();
    return;
  }

  vips_concurrency_set(2);
  vips_vector_set_enabled(true);
  vips_cache_set_max(100);
  vips_cache_set_max_mem(50 * 1024 * 1024);
  vips_cache_set_max_files(20);
  vips_initialized = true;
}

bool RequireArguments(const Napi::CallbackInfo& info) {
  if (info.Length() != 2 || !info[0].IsObject() || !info[1].IsFunction()) {
    Napi::TypeError::New(info.Env(), "Expected options object and callback")
        .ThrowAsJavaScriptException();
    return false;
  }
  return true;
}

Napi::Value Thumbnail(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (!RequireArguments(info)) {
    return env.Undefined();
  }

  Napi::Object options = info[0].As<Napi::Object>();
  if (!options.Has("buffer") || !options.Get("buffer").IsBuffer() ||
      !options.Has("width") || !options.Has("height") ||
      !options.Has("format")) {
    Napi::TypeError::New(env,
                         "options requires buffer, width, height, and format")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Value width = options.Get("width");
  Napi::Value height = options.Get("height");
  Napi::Value format = options.Get("format");
  if (!width.IsNumber() || !height.IsNumber() || !format.IsString()) {
    Napi::TypeError::New(env, "width and height must be numbers; format a string")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  auto* worker = new BufferThumbnailWorker(
      info[1].As<Napi::Function>(), options.Get("buffer").As<Napi::Buffer<char>>(),
      width.As<Napi::Number>().Int32Value(),
      height.As<Napi::Number>().Int32Value(), format.As<Napi::String>().Utf8Value());
  worker->Queue();
  return env.Undefined();
}

Napi::Value ThumbnailFs(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (!RequireArguments(info)) {
    return env.Undefined();
  }

  Napi::Object options = info[0].As<Napi::Object>();
  const char* required[] = {"inputPath", "outputPath", "width", "height"};
  for (const char* property : required) {
    if (!options.Has(property)) {
      Napi::TypeError::New(env, "options requires inputPath, outputPath, width, and height")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  Napi::Value input_path = options.Get("inputPath");
  Napi::Value output_path = options.Get("outputPath");
  Napi::Value width = options.Get("width");
  Napi::Value height = options.Get("height");
  if (!input_path.IsString() || !output_path.IsString() || !width.IsNumber() ||
      !height.IsNumber()) {
    Napi::TypeError::New(env,
                         "inputPath/outputPath must be strings; width/height numbers")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  auto* worker = new FileThumbnailWorker(
      info[1].As<Napi::Function>(), input_path.As<Napi::String>().Utf8Value(),
      output_path.As<Napi::String>().Utf8Value(),
      width.As<Napi::Number>().Int32Value(),
      height.As<Napi::Number>().Int32Value());
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
