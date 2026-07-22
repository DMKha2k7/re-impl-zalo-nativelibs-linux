#ifndef ZIMAGE_FILE_THUMBNAIL_WORKER_H_
#define ZIMAGE_FILE_THUMBNAIL_WORKER_H_

#include <napi.h>

#include <string>

class FileThumbnailWorker final : public Napi::AsyncWorker {
 public:
  FileThumbnailWorker(const Napi::Function& callback,
                      std::string input_path,
                      std::string output_path,
                      int width,
                      int height);

  void Execute() override;
  void OnOK() override;
  void OnError(const Napi::Error& error) override;

 private:
  std::string input_path_;
  std::string output_path_;
  int width_;
  int height_;
};

#endif  // ZIMAGE_FILE_THUMBNAIL_WORKER_H_
