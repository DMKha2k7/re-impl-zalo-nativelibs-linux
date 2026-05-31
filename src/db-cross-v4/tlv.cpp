#include "tlv.h"
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace tlv {

TlvBox::TlvBox() {}

TlvBox::~TlvBox() {
    for (auto* tlv : m_tlvs) {
        delete tlv;
    }
}

bool TlvBox::Parse(const uint8_t* buffer, size_t size) {
    if (!buffer || size == 0) return false;
    
    // Copy to internal buffer
    m_buffer.assign(buffer, buffer + size);
    
    size_t offset = 0;
    while (offset < size) {
        if (offset + 8 > size) {
            // Buffer too small for header (tag + length)
            return false;
        }
        
        // Read tag (4 bytes, big-endian)
        uint32_t tag = (m_buffer[offset] << 24) | 
                       (m_buffer[offset + 1] << 16) | 
                       (m_buffer[offset + 2] << 8) | 
                       m_buffer[offset + 3];
                       
        // Read length (4 bytes, big-endian)
        uint32_t length = (m_buffer[offset + 4] << 24) | 
                          (m_buffer[offset + 5] << 16) | 
                          (m_buffer[offset + 6] << 8) | 
                          m_buffer[offset + 7];
                          
        if (offset + 8 + length > size) {
            // Value extends past end of buffer
            return false;
        }
        
        Tlv* tlv = new Tlv(tag, m_buffer.data() + offset + 8, length);
        m_tlvs.push_back(tlv);
        m_keys.push_back(tag);
        
        offset += 8 + length;
    }
    
    return true;
}

const std::vector<uint32_t>& TlvBox::GetAllKey() const {
    return m_keys;
}

bool TlvBox::GetBytesValue(uint32_t tag, void** out_val, uint32_t* out_len) const {
    for (const auto* tlv : m_tlvs) {
        if (tlv->GetType() == tag) {
            *out_val = const_cast<uint8_t*>(tlv->GetValue());
            *out_len = tlv->GetLength();
            return true;
        }
    }
    return false;
}

bool TlvBox::GetIntValue(uint32_t tag, int* out_val) const {
    for (const auto* tlv : m_tlvs) {
        if (tlv->GetType() == tag && tlv->GetLength() == 4) {
            const uint8_t* p = tlv->GetValue();
            *out_val = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
            return true;
        }
    }
    return false;
}

bool TlvBox::GetInt64Value(uint32_t tag, int64_t* out_val) const {
    for (const auto* tlv : m_tlvs) {
        if (tlv->GetType() == tag && tlv->GetLength() == 8) {
            const uint8_t* p = tlv->GetValue();
            *out_val = ((int64_t)p[0] << 56) | 
                       ((int64_t)p[1] << 48) | 
                       ((int64_t)p[2] << 40) | 
                       ((int64_t)p[3] << 32) |
                       ((int64_t)p[4] << 24) | 
                       ((int64_t)p[5] << 16) | 
                       ((int64_t)p[6] << 8) | 
                       p[7];
            return true;
        }
    }
    return false;
}

std::vector<const Tlv*> TlvBox::GetValues(uint32_t tag) const {
    std::vector<const Tlv*> results;
    for (const auto* tlv : m_tlvs) {
        if (tlv->GetType() == tag) {
            results.push_back(tlv);
        }
    }
    return results;
}

// Parse attachment message structure
bool ParseAttachChatMsg(Napi::Env env, Napi::Object& out_obj, const uint8_t* data, size_t size) {
    TlvBox box;
    if (!box.Parse(data, size)) {
        return false;
    }
    
    // We check all tags
    for (uint32_t tag : box.GetAllKey()) {
        switch (tag) {
            case 0x28: { // type (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("type", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x29: { // catId (int)
                int val = 0;
                if (box.GetIntValue(tag, &val)) {
                    out_obj.Set("catId", Napi::Number::New(env, val));
                }
                break;
            }
            case 0x2a: { // id (int)
                int val = 0;
                if (box.GetIntValue(tag, &val)) {
                    out_obj.Set("id", Napi::Number::New(env, val));
                }
                break;
            }
            case 0x2b: { // extInfo (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("extInfo", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x2c: { // childNumber (int)
                int val = 0;
                if (box.GetIntValue(tag, &val)) {
                    out_obj.Set("childNumber", Napi::Number::New(env, val));
                }
                break;
            }
            case 0x2d: { // action (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("action", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x2e: { // params (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("params", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x2f: { // title (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("title", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x30: { // href (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("href", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x31: { // thumb (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("thumb", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x32: { // description (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("description", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x42: { // remains (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("remains", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x43: { // zinstantData (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("zinstantData", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            case 0x44: { // zinstantMsg (string)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    out_obj.Set("zinstantMsg", Napi::String::New(env, reinterpret_cast<const char*>(val), len));
                }
                break;
            }
            default:
                break;
        }
    }
    
    return true;
}

// Parse outer binary network structure
int parse_bin_net(Napi::Env env, Napi::Object& out_obj, const uint8_t* data, size_t size) {
    TlvBox box;
    if (!box.Parse(data, size)) {
        return -1;
    }
    
    // Create nested data object
    Napi::Object data_obj = Napi::Object::New(env);
    
    // Set root properties
    out_obj.Set("dataSource", Napi::Number::New(env, 1));
    out_obj.Set("data", data_obj);
    
    for (uint32_t tag : box.GetAllKey()) {
        switch (tag) {
            case 5: { // property (nested object)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    TlvBox prop_box;
                    if (prop_box.Parse(reinterpret_cast<const uint8_t*>(val), len)) {
                        Napi::Object prop_obj = Napi::Object::New(env);
                        for (uint32_t ptag : prop_box.GetAllKey()) {
                            switch (ptag) {
                                case 30: { // size
                                    int pval = 0;
                                    if (prop_box.GetIntValue(ptag, &pval)) {
                                        prop_obj.Set("size", Napi::Number::New(env, pval));
                                    }
                                    break;
                                }
                                case 31: { // subType
                                    int pval = 0;
                                    if (prop_box.GetIntValue(ptag, &pval)) {
                                        prop_obj.Set("subType", Napi::Number::New(env, pval));
                                    }
                                    break;
                                }
                                case 32: { // type
                                    int pval = 0;
                                    if (prop_box.GetIntValue(ptag, &pval)) {
                                        prop_obj.Set("type", Napi::Number::New(env, pval));
                                    }
                                    break;
                                }
                                case 33: { // ext
                                    void* pval = nullptr;
                                    uint32_t plen = 0;
                                    if (prop_box.GetBytesValue(ptag, &pval, &plen)) {
                                        prop_obj.Set("ext", Napi::String::New(env, reinterpret_cast<const char*>(pval), plen));
                                    }
                                    break;
                                }
                                case 34: { // color
                                    int pval = 0;
                                    if (prop_box.GetIntValue(ptag, &pval)) {
                                        prop_obj.Set("color", Napi::Number::New(env, pval));
                                    }
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                        data_obj.Set("property", prop_obj);
                    }
                }
                break;
            }
            case 6: { // attachs (array of attachment objects)
                std::vector<const Tlv*> attach_tlvs = box.GetValues(tag);
                Napi::Array attach_arr = Napi::Array::New(env);
                uint32_t idx = 0;
                for (const auto* attach_tlv : attach_tlvs) {
                    Napi::Object attach_obj = Napi::Object::New(env);
                    if (ParseAttachChatMsg(env, attach_obj, attach_tlv->GetValue(), attach_tlv->GetLength())) {
                        attach_arr.Set(idx++, attach_obj);
                    }
                }
                data_obj.Set("attachs", attach_arr);
                break;
            }
            case 7: { // quote (nested quote object)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    TlvBox quote_box;
                    if (quote_box.Parse(reinterpret_cast<const uint8_t*>(val), len)) {
                        Napi::Object quote_obj = Napi::Object::New(env);
                        for (uint32_t qtag : quote_box.GetAllKey()) {
                            switch (qtag) {
                                case 80: { // ownerId
                                    int qval = 0;
                                    if (quote_box.GetIntValue(qtag, &qval)) {
                                        quote_obj.Set("ownerId", Napi::Number::New(env, qval));
                                    }
                                    break;
                                }
                                case 81: { // cliMsgId
                                    int64_t qval = 0;
                                    if (quote_box.GetInt64Value(qtag, &qval)) {
                                        quote_obj.Set("cliMsgId", Napi::Number::New(env, static_cast<double>(qval)));
                                    }
                                    break;
                                }
                                case 82: { // globalMsgId
                                    int64_t qval = 0;
                                    if (quote_box.GetInt64Value(qtag, &qval)) {
                                        quote_obj.Set("globalMsgId", Napi::Number::New(env, static_cast<double>(qval)));
                                    }
                                    break;
                                }
                                case 83: { // cliMsgType
                                    int qval = 0;
                                    if (quote_box.GetIntValue(qtag, &qval)) {
                                        quote_obj.Set("cliMsgType", Napi::Number::New(env, qval));
                                    }
                                    break;
                                }
                                case 84: { // ts
                                    int64_t qval = 0;
                                    if (quote_box.GetInt64Value(qtag, &qval)) {
                                        quote_obj.Set("ts", Napi::Number::New(env, static_cast<double>(qval)));
                                    }
                                    break;
                                }
                                case 85: { // ttl
                                    int64_t qval = 0;
                                    if (quote_box.GetInt64Value(qtag, &qval)) {
                                        quote_obj.Set("ttl", Napi::Number::New(env, static_cast<double>(qval)));
                                    }
                                    break;
                                }
                                case 86: { // msg
                                    void* qval = nullptr;
                                    uint32_t qlen = 0;
                                    if (quote_box.GetBytesValue(qtag, &qval, &qlen)) {
                                        quote_obj.Set("msg", Napi::String::New(env, reinterpret_cast<const char*>(qval), qlen));
                                    }
                                    break;
                                }
                                case 87: { // attach
                                    void* qval = nullptr;
                                    uint32_t qlen = 0;
                                    if (quote_box.GetBytesValue(qtag, &qval, &qlen)) {
                                        quote_obj.Set("attach", Napi::String::New(env, reinterpret_cast<const char*>(qval), qlen));
                                    }
                                    break;
                                }
                                case 88: { // fromD
                                    void* qval = nullptr;
                                    uint32_t qlen = 0;
                                    if (quote_box.GetBytesValue(qtag, &qval, &qlen)) {
                                        quote_obj.Set("fromD", Napi::String::New(env, reinterpret_cast<const char*>(qval), qlen));
                                    }
                                    break;
                                }
                                case 90: { // quoteStatus
                                    void* qval = nullptr;
                                    uint32_t qlen = 0;
                                    if (quote_box.GetBytesValue(qtag, &qval, &qlen)) {
                                        quote_obj.Set("quoteStatus", Napi::String::New(env, reinterpret_cast<const char*>(qval), qlen));
                                    }
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                        data_obj.Set("quote", quote_obj);
                    }
                }
                break;
            }
            case 8: { // mentions (array of mention objects)
                std::vector<const Tlv*> mention_tlvs = box.GetValues(tag);
                Napi::Array mention_arr = Napi::Array::New(env);
                uint32_t idx = 0;
                for (const auto* mention_tlv : mention_tlvs) {
                    TlvBox m_box;
                    if (m_box.Parse(mention_tlv->GetValue(), mention_tlv->GetLength())) {
                        Napi::Object m_obj = Napi::Object::New(env);
                        for (uint32_t mtag : m_box.GetAllKey()) {
                            switch (mtag) {
                                case 100: { // type
                                    int mval = 0;
                                    if (m_box.GetIntValue(mtag, &mval)) {
                                        m_obj.Set("type", Napi::Number::New(env, mval));
                                    }
                                    break;
                                }
                                case 101: { // uid
                                    int mval = 0;
                                    if (m_box.GetIntValue(mtag, &mval)) {
                                        m_obj.Set("uid", Napi::Number::New(env, mval));
                                    }
                                    break;
                                }
                                case 102: { // pos
                                    int mval = 0;
                                    if (m_box.GetIntValue(mtag, &mval)) {
                                        m_obj.Set("pos", Napi::Number::New(env, mval));
                                    }
                                    break;
                                }
                                case 103: { // len
                                    int mval = 0;
                                    if (m_box.GetIntValue(mtag, &mval)) {
                                        m_obj.Set("len", Napi::Number::New(env, mval));
                                    }
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                        mention_arr.Set(idx++, m_obj);
                    }
                }
                data_obj.Set("mentions", mention_arr);
                break;
            }
            case 11: { // reference (nested object)
                void* val = nullptr;
                uint32_t len = 0;
                if (box.GetBytesValue(tag, &val, &len)) {
                    TlvBox ref_box;
                    if (ref_box.Parse(reinterpret_cast<const uint8_t*>(val), len)) {
                        Napi::Object ref_obj = Napi::Object::New(env);
                        for (uint32_t rtag : ref_box.GetAllKey()) {
                            switch (rtag) {
                                case 170: { // type
                                    int rval = 0;
                                    if (ref_box.GetIntValue(rtag, &rval)) {
                                        ref_obj.Set("type", Napi::Number::New(env, rval));
                                    }
                                    break;
                                }
                                case 171: { // data (string)
                                    void* rval = nullptr;
                                    uint32_t rlen = 0;
                                    if (ref_box.GetBytesValue(rtag, &rval, &rlen)) {
                                        ref_obj.Set("data", Napi::String::New(env, reinterpret_cast<const char*>(rval), rlen));
                                    }
                                    break;
                                }
                                default:
                                    break;
                            }
                        }
                        data_obj.Set("reference", ref_obj);
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    
    return 0;
}

} // namespace tlv
