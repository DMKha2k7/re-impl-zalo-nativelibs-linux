#include <napi.h>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <sys/statvfs.h>

// Error definitions and helper functions for error handling
namespace Errors {
const std::string WRONG_NUMBER_OF_ARGS = "DISKUSAGE_WRONG_NUMBER_OF_ARGS";
// Đã sửa lỗi typo tiêu đề lỗi ở đây
const std::string INVALID_PATH_TYPE =
    "DISKUSAGE_INVALID_PATH_TYPE: The \"path\" argument must be of type string";
const std::string STATVFS_FAILED = "Get diskusage failed for path: ";

void ThrowError(Napi::Env env, const std::string &code,
                const std::string &msg = "") {
  std::string fullMsg = msg.empty() ? code : code + msg;
  Napi::Error::New(env, fullMsg).ThrowAsJavaScriptException();
}
} // namespace Errors

/**
 * Core logic to fetch system disk usage.
 * Invokes the system statvfs API and calculates capacity metrics.
 */
void FetchDiskUsage(const std::string &path, double &available, double &free,
                    double &total) {
  struct statvfs local_stat;

  // Call the POSIX statvfs system function
  if (statvfs(path.c_str(), &local_stat) != 0) {
    throw std::runtime_error(Errors::STATVFS_FAILED + path);
  }

  // fallback: If f_frsize is 0 or invalid on some special file systems,
  // we use f_bsize (fundamental block size) to calculate,
  // avoid multiplying by 0 results in wrong result.
  uint64_t blksize =
      (local_stat.f_frsize > 0) ? local_stat.f_frsize : local_stat.f_bsize;

  // Perform multiplication in uint64_t data type first to prevent overflow,
  // then cast the result to double to return to JavaScript Number.
  total =
      static_cast<double>(static_cast<uint64_t>(local_stat.f_blocks) * blksize);
  free =
      static_cast<double>(static_cast<uint64_t>(local_stat.f_bfree) * blksize);
  available =
      static_cast<double>(static_cast<uint64_t>(local_stat.f_bavail) * blksize);
}

/**
 * Wrapper function for RunFetchDiskUsage.
 * Validates inputs from Node.js and returns the disk usage object.
 */
Napi::Value RunFetchDiskUsage(const Napi::CallbackInfo &info) {
  Napi::Env env = info.Env();

  // 1. Validate argument count
  if (info.Length() < 1) {
    Errors::ThrowError(env, Errors::WRONG_NUMBER_OF_ARGS);
    return env.Undefined();
  }

  // 2. Validate argument type
  if (!info[0].IsString()) {
    Errors::ThrowError(env, Errors::INVALID_PATH_TYPE);
    return env.Undefined();
  }

  // 3. Extract the path string from JavaScript argument
  std::string path = info[0].As<Napi::String>().Utf8Value();

  double available = 0, free = 0, total = 0;

  // 4. Execute system logic and throw any errors as JavaScript exceptions
  try {
    FetchDiskUsage(path, available, free, total);
  } catch (const std::exception &e) {
    Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
    return env.Undefined();
  }

  // 5. Initialize and populate JavaScript return object
  Napi::Object result = Napi::Object::New(env);
  result.Set("available", Napi::Number::New(env, available));
  result.Set("free", Napi::Number::New(env, free));
  result.Set("total", Napi::Number::New(env, total));

  return result;
}

/**
 * Module initialization function.
 */
Napi::Object Init(Napi::Env env, Napi::Object exports) {
  // Register 'getDiskUsage' function mapping to C++ RunFetchDiskUsage
  exports.Set(Napi::String::New(env, "getDiskUsage"),
              Napi::Function::New(env, RunFetchDiskUsage));
  return exports;
}

// Register module with Node-API
NODE_API_MODULE(file_utils, Init)