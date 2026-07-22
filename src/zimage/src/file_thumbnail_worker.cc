#include "file_thumbnail_worker.h"

#include <glib-object.h>
#include <vips/vips8>

#include <string>

namespace {

std::string VipsErrorMessage() {
  const char* message = vips_error_buffer();
  std::string result = message == nullptr || *message == '\0'
                           ? "libvips image operation failed"
                           : message;
  vips_error_clear();
  return result;
}

void ReleaseImage(VipsImage* image) {
  if (image != nullptr) {
    g_object_unref(image);
  }
}

}  // namespace

FileThumbnailWorker::FileThumbnailWorker(const Napi::Function& callback,
                                         std::string input_path,
                                         std::string output_path,
                                         int width,
                                         int height)
    : Napi::AsyncWorker(callback),
      input_path_(std::move(input_path)),
      output_path_(std::move(output_path)),
      width_(width),
      height_(height) {}

void FileThumbnailWorker::Execute() {
  VipsImage* thumbnail = nullptr;
  if (vips_thumbnail(input_path_.c_str(), &thumbnail, width_, "height",
                     height_, "size", VIPS_SIZE_FORCE, nullptr) != 0) {
    SetError(VipsErrorMessage());
    return;
  }

  if (vips_image_write_to_file(thumbnail, output_path_.c_str(), nullptr) !=
      0) {
    ReleaseImage(thumbnail);
    SetError(VipsErrorMessage());
    return;
  }

  ReleaseImage(thumbnail);
}

void FileThumbnailWorker::OnOK() {
  Napi::HandleScope scope(Env());
  Callback().Call({Env().Null()});
}

void FileThumbnailWorker::OnError(const Napi::Error& error) {
  Napi::HandleScope scope(Env());
  Callback().Call({error.Value()});
}
