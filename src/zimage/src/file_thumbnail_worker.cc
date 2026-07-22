#include "file_thumbnail_worker.h"

#include <glib-object.h>
#include <vips/vips8>

#include <algorithm>
#include <cctype>
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

// Hàm bổ trợ kiểm tra đuôi file không phân biệt hoa thường
bool HasExtension(const std::string& path, const std::string& ext) {
  if (path.length() < ext.length()) return false;
  std::string path_ext = path.substr(path.length() - ext.length());
  std::transform(path_ext.begin(), path_ext.end(), path_ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return path_ext == ext;
}

}  // namespace

FileThumbnailWorker::FileThumbnailWorker(const Napi::Function& callback,
                                         std::string input_path,
                                         std::string output_path,
                                         int width,
                                         int height,
                                         int quality)
    : Napi::AsyncWorker(callback),
      input_path_(std::move(input_path)),
      output_path_(std::move(output_path)),
      width_(width),
      height_(height),
      quality_(quality) {}

void FileThumbnailWorker::Execute() {
  VipsImage* thumbnail = nullptr;
  if (vips_thumbnail(input_path_.c_str(), &thumbnail, width_, "height",
                     height_, "size", VIPS_SIZE_FORCE, "no_rotate", TRUE,
                     "fail_on", VIPS_FAIL_ON_NONE, nullptr) != 0) {
    SetError(VipsErrorMessage());
    return;
  }

  std::string target_path = output_path_;
  // Nếu đường dẫn chưa chứa option cấu hình của libvips [option=val]
  if (target_path.find('[') == std::string::npos) {
    if (HasExtension(target_path, ".png")) {
      target_path += "[compression=1,filter=none,palette=false,strip=true]";
    } else if (HasExtension(target_path, ".jpg") || HasExtension(target_path, ".jpeg")) {
      target_path += "[Q=" + std::to_string(quality_) + ",strip=true]";
    } else if (HasExtension(target_path, ".webp")) {
      target_path += "[Q=" + std::to_string(quality_) + ",strip=true]";
    }
  }

  if (vips_image_write_to_file(thumbnail, target_path.c_str(), nullptr) != 0) {
    ReleaseImage(thumbnail);
    SetError(VipsErrorMessage());
    return;
  }

  ReleaseImage(thumbnail);
}

void FileThumbnailWorker::OnOK() {
  Napi::HandleScope scope(Env());
  Callback().Call({Env().Null(), Napi::String::New(Env(), output_path_)});
}

void FileThumbnailWorker::OnError(const Napi::Error& error) {
  Napi::HandleScope scope(Env());
  Callback().Call({error.Value(), Napi::String::New(Env(), output_path_)});
}