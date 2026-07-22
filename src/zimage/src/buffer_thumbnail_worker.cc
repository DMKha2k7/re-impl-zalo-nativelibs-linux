#include "buffer_thumbnail_worker.h"

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

BufferThumbnailWorker::BufferThumbnailWorker(
    const Napi::Function& callback,
    const Napi::Buffer<char>& input_buffer,
    int width,
    int height,
    std::string format,
    int quality)
    : Napi::AsyncWorker(callback),
      input_buffer_reference_(Napi::Persistent(input_buffer)),
      input_data_(input_buffer.Data()),
      input_length_(input_buffer.Length()),
      width_(width),
      height_(height),
      format_(std::move(format)),
      quality_(quality),
      output_data_(nullptr),
      output_length_(0) {}

BufferThumbnailWorker::~BufferThumbnailWorker() {
  ReleaseInputBuffer();
  if (output_data_ != nullptr) {
    g_free(output_data_);
  }
}

void BufferThumbnailWorker::Execute() {
  VipsImage* thumbnail = nullptr;
  VipsImage* flattened = nullptr;
  VipsArrayDouble* background = nullptr;

  if (vips_thumbnail_buffer(const_cast<char*>(input_data_), input_length_,
                            &thumbnail, width_, "height", height_, "size",
                            VIPS_SIZE_FORCE, "no_rotate", TRUE, "fail_on",
                            VIPS_FAIL_ON_NONE, nullptr) != 0) {
    SetError(VipsErrorMessage());
    return;
  }

  if (format_ == "jpeg" || format_ == "jpg") {
    // Xử lý kênh alpha đối với file JPEG (Flatten đè nền trắng)
    if (vips_image_hasalpha(thumbnail)) {
      const double white[] = {255.0, 255.0, 255.0};
      background = vips_array_double_new(white, G_N_ELEMENTS(white));
      if (background == nullptr ||
          vips_flatten(thumbnail, &flattened, "background", background,
                       nullptr) != 0) {
        if (background != nullptr) {
          vips_area_unref(reinterpret_cast<VipsArea*>(background));
        }
        ReleaseImage(thumbnail);
        SetError(VipsErrorMessage());
        return;
      }
      vips_area_unref(reinterpret_cast<VipsArea*>(background));
      ReleaseImage(thumbnail);
      thumbnail = flattened;
    }

    void* encoded = nullptr;
    if (vips_jpegsave_buffer(thumbnail, &encoded, &output_length_, "strip",
                             TRUE, "Q", quality_, nullptr) != 0) {
      ReleaseImage(thumbnail);
      SetError(VipsErrorMessage());
      return;
    }
    output_data_ = static_cast<char*>(encoded);
  } else if (format_ == "webp") {
    void* encoded = nullptr;
    if (vips_webpsave_buffer(thumbnail, &encoded, &output_length_, "strip",
                              TRUE, "Q", quality_, nullptr) != 0) {
      ReleaseImage(thumbnail);
      SetError(VipsErrorMessage());
      return;
    }
    output_data_ = static_cast<char*>(encoded);
  } else {
    // Mặc định xuất ra PNG nếu không thuộc các format trên
    void* encoded = nullptr;
    if (vips_image_write_to_buffer(
            thumbnail,
            ".png[compression=1,filter=none,palette=false,strip=true]",
            &encoded, &output_length_, nullptr) != 0) {
      ReleaseImage(thumbnail);
      SetError(VipsErrorMessage());
      return;
    }
    output_data_ = static_cast<char*>(encoded);
  }

  ReleaseImage(thumbnail);
}

void BufferThumbnailWorker::OnOK() {
  Napi::HandleScope scope(Env());
  char* data = output_data_;
  output_data_ = nullptr; // Chuyển giao quyền sở hữu bộ nhớ cho Node.js Buffer
  Napi::Buffer<char> output = Napi::Buffer<char>::New(
      Env(), data, output_length_, BufferThumbnailWorker::FreeOutputBuffer);
  ReleaseInputBuffer();
  Callback().Call({Env().Null(), output});
}

void BufferThumbnailWorker::OnError(const Napi::Error& error) {
  Napi::HandleScope scope(Env());
  ReleaseInputBuffer();
  Callback().Call({error.Value(), Env().Undefined()});
}

void BufferThumbnailWorker::FreeOutputBuffer(Napi::Env /*env*/, char* data) {
  g_free(data);
}

void BufferThumbnailWorker::ReleaseInputBuffer() {
  if (!input_buffer_reference_.IsEmpty()) {
    input_buffer_reference_.Reset();
  }
}