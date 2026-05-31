#include <napi.h>
#include "tlv.h"
#include "backup_format.h"

// 1. getVersion method
Napi::Value GetVersion(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    return Napi::String::New(env, "1.0.0");
}

// 2. parseBinNet method
Napi::Value ParseBinNetMethod(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsBuffer()) {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Buffer<uint8_t> buf = info[0].As<Napi::Buffer<uint8_t>>();

    Napi::Object out_obj = Napi::Object::New(env);
    tlv::parse_bin_net(env, out_obj, buf.Data(), buf.Length());
    return out_obj;
}

// 3. decompressAndDecryptDb method (Version 0)
Napi::Value DecompressAndDecryptDbMethod(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Object res_obj = Napi::Object::New(env);

    if (info.Length() < 3) {
        res_obj.Set("result", Napi::Number::New(env, 1));
        res_obj.Set("error_message", Napi::String::New(env, "Wrong number of arguments"));
        return res_obj;
    }

    if (!info[0].IsString() || !info[1].IsString() || !info[2].IsString()) {
        res_obj.Set("result", Napi::Number::New(env, 2));
        res_obj.Set("error_message", Napi::String::New(env, "Wrong arguments"));
        return res_obj;
    }

    std::string db_path = info[0].As<Napi::String>().Utf8Value();
    std::string key = info[1].As<Napi::String>().Utf8Value();
    std::string out_path = info[2].As<Napi::String>().Utf8Value();

    int err = backup_format::decrypt_v0(db_path, key, out_path);
    if (err == 0) {
        res_obj.Set("result", Napi::Number::New(env, 0));
    } else {
        res_obj.Set("result", Napi::Number::New(env, err)); // Wait, the decompile sets result to uVar6 (err) or similar
        res_obj.Set("inner_error", Napi::Number::New(env, err));
        res_obj.Set("error_message", Napi::String::New(env, "Something went wrong while decrypting"));
    }

    return res_obj;
}

// 4. decompressAndDecryptDb_V2 method (Version 1)
Napi::Value DecompressAndDecryptDbV2Method(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Object res_obj = Napi::Object::New(env);

    if (info.Length() < 4) {
        res_obj.Set("result", Napi::Number::New(env, 1));
        res_obj.Set("error_message", Napi::String::New(env, "Wrong number of arguments"));
        return res_obj;
    }

    if (!info[0].IsString() || !info[1].IsString() || !info[2].IsString() || !info[3].IsFunction()) {
        res_obj.Set("result", Napi::Number::New(env, 2));
        res_obj.Set("error_message", Napi::String::New(env, "Wrong arguments"));
        return res_obj;
    }

    std::string db_path = info[0].As<Napi::String>().Utf8Value();
    std::string key = info[1].As<Napi::String>().Utf8Value();
    std::string out_dir = info[2].As<Napi::String>().Utf8Value();
    // 4th argument info[3] is the callback function (ignored but verified)

    std::string error_msg;
    int err = backup_format::decrypt_v1(db_path, key, out_dir, error_msg);
    if (err == 0) {
        res_obj.Set("result", Napi::Number::New(env, 0));
    } else {
        res_obj.Set("result", Napi::Number::New(env, err));
        res_obj.Set("inner_error", Napi::Number::New(env, err));
        res_obj.Set("error_message", Napi::String::New(env, error_msg));
    }

    return res_obj;
}

// Module registration
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("getVersion", Napi::Function::New(env, GetVersion));
    exports.Set("parseBinNet", Napi::Function::New(env, ParseBinNetMethod));
    exports.Set("decompressAndDecryptDb", Napi::Function::New(env, DecompressAndDecryptDbMethod));
    exports.Set("decompressAndDecryptDb_V2", Napi::Function::New(env, DecompressAndDecryptDbV2Method));
    return exports;
}

NODE_API_MODULE(db_cross_v4_native, Init)
