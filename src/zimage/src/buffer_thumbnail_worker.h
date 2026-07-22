#ifndef ZIMAGE_BUFFER_THUMBNAIL_WORKER_H_
#define ZIMAGE_BUFFER_THUMBNAIL_WORKER_H_

#include <napi.h>

#include <cstddef>
#include <string>

class BufferThumbnailWorker final : public Napi::AsyncWorker {
 public:
  BufferThumbnailWorker(const Napi::Function& callback,
                        const Napi::Buffer<char>& input_buffer,
                        int width,
                        int height,
                        std::string format,
                        int quality);
  ~BufferThumbnailWorker() override;

  void Execute() override;
  void OnOK() override;
  void OnError(const Napi::Error& error) override;

 private:
  static void FreeOutputBuffer(Napi::Env env, char* data);
  void ReleaseInputBuffer();

  Napi::Reference<Napi::Buffer<char>> input_buffer_reference_;
  const char* input_data_;
  std::size_t input_length_;
  int width_;
  int height_;
  std::string format_;
  int quality_;
  char* output_data_;
  std::size_t output_length_;
};

#endif  // ZIMAGE_BUFFER_THUMBNAIL_WORKER_H_