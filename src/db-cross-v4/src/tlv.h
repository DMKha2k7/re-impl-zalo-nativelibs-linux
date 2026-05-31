#ifndef TLV_H
#define TLV_H

#include <napi.h>
#include <map>
#include <vector>
#include <cstdint>
#include <string>

namespace tlv {

class Tlv {
private:
    uint32_t type;
    uint32_t length;
    const uint8_t* value_ptr;

public:
    Tlv(uint32_t type, const uint8_t* val, uint32_t len)
        : type(type), length(len), value_ptr(val) {}

    uint32_t GetType() const { return type; }
    uint32_t GetLength() const { return length; }
    const uint8_t* GetValue() const { return value_ptr; }
};

class TlvBox {
private:
    std::vector<Tlv*> m_tlvs;
    std::vector<uint32_t> m_keys;
    std::vector<uint8_t> m_buffer;

public:
    TlvBox();
    ~TlvBox();

    bool Parse(const uint8_t* buffer, size_t size);
    const std::vector<uint32_t>& GetAllKey() const;
    bool GetBytesValue(uint32_t tag, void** out_val, uint32_t* out_len) const;
    bool GetIntValue(uint32_t tag, int* out_val) const;
    bool GetInt64Value(uint32_t tag, int64_t* out_val) const;
    std::vector<const Tlv*> GetValues(uint32_t tag) const;
};

// Network message parser routines
int parse_bin_net(Napi::Env env, Napi::Object& out_obj, const uint8_t* data, size_t size);
bool ParseAttachChatMsg(Napi::Env env, Napi::Object& out_obj, const uint8_t* data, size_t size);

} // namespace tlv

#endif // TLV_H
