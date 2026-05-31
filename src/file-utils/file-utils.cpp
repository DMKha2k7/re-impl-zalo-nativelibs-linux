#include <napi.h>
#include <sys/statvfs.h>
#include <string>
#include <stdexcept>

// Toàn bộ các macro/bảng mã lỗi được quản lý tập trung dựa trên hàm GLOBAL_sub_I_errors trong file decompile
namespace Errors {
    const std::string WRONG_NUMBER_OF_ARGS = "DISKUSAGE_WRONG_NUMBER_OF_ARGS";
    const std::string INVALID_PATH_TYPE = "DISKUSAGE_WRONG_NUMBER_OF_ARGS: The \"path\" argument must be one of type string";
    const std::string STATVFS_FAILED = "Get diskusage failed";

    void ThrowError(Napi::Env env, const std::string& code, const std::string& msg = "") {
        std::string fullMsg = msg.empty() ? code : code + msg;
        Napi::Error::New(env, fullMsg).ThrowAsJavaScriptException();
    }
}

/**
 * Logic lõi tương ứng với hàm `FetchDiskUsage` trong file decompile.
 * Thực hiện gọi hàm hệ thống statvfs của Linux và tính toán dung lượng byte.
 */
void FetchDiskUsage(const std::string& path, double& available, double& free, double& total) {
    struct statvfs local_stat;
    
    // Gọi hàm hệ thống POSIX giống như _statvfs trong file decompile
    if (statvfs(path.c_str(), &local_stat) != 0) {
        throw std::runtime_error(Errors::STATVFS_FAILED);
    }

    // Công thức khôi phục từ RE: lấy số block nhân với kích thước block (f_frsize)
    // Dùng kiểu dữ liệu double để tương thích với kiểu Number của JavaScript (Napi::Number)
    total     = static_cast<double>(local_stat.f_blocks) * local_stat.f_frsize;
    free      = static_cast<double>(local_stat.f_bfree)  * local_stat.f_frsize;
    available = static_cast<double>(local_stat.f_bavail) * local_stat.f_frsize;
}

/**
 * Hàm Wrapper tương ứng với `RunFetchDiskUsage`.
 * Tiếp nhận CallbackInfo từ Node.js, kiểm tra tham số và trả về Object.
 */
Napi::Value RunFetchDiskUsage(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 1. Kiểm tra số lượng đối số (Khôi phục từ đoạn kiểm tra param_1 + 0x20 == 0)
    if (info.Length() < 1) {
        Errors::ThrowError(env, Errors::WRONG_NUMBER_OF_ARGS);
        return env.Undefined();
    }

    // 2. Kiểm tra kiểu dữ liệu của đối số (Khôi phục từ đoạn check _napi_typeof == 4)
    if (!info[0].IsString()) {
        Errors::ThrowError(env, Errors::INVALID_PATH_TYPE);
        return env.Undefined();
    }

    // 3. Trích xuất đường dẫn UTF-8 từ JavaScript String
    std::string path = info[0].As<Napi::String>().Utf8Value();

    double available = 0, free = 0, total = 0;

    // 4. Thực thi logic hệ thống và bẫy ngoại lệ C++ chuyển thành JS Exception
    try {
        FetchDiskUsage(path, available, free, total);
    } catch (const std::exception& e) {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
    }

    // 5. Khởi tạo và gán thuộc tính cho JavaScript Object trả về
    Napi::Object result = Napi::Object::New(env);
    result.Set("available", Napi::Number::New(env, available));
    result.Set("free",      Napi::Number::New(env, free));
    result.Set("total",     Napi::Number::New(env, total));

    return result;
}

/**
 * Hàm khởi tạo Module tương ứng với `initAll` / `GetDiskUsage::Init`.
 */
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    // Đăng ký hàm có tên 'getDiskUsage' trỏ tới hàm C++ RunFetchDiskUsage
    exports.Set(Napi::String::New(env, "getDiskUsage"), 
                Napi::Function::New(env, RunFetchDiskUsage));
    return exports;
}

// Macro đăng ký module với Node.js tương tự như _register_binding
NODE_API_MODULE(file_utils, Init)