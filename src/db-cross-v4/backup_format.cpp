#include "backup_format.h"
#include "lzma_decoder.h"
#include "xxhash.h"
#include <openssl/evp.h>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <iomanip>

namespace backup_format {

// Helper: base64 decoder
static std::string base64_decode(const std::string& in) {
    std::string out;
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) {
        T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;
    }

    int val = 0, valb = -8;
    for (char c : in) {
        if (T[static_cast<unsigned char>(c)] == -1) {
            if (c == '=') break;
            continue;
        }
        val = (val << 6) + T[static_cast<unsigned char>(c)];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

// Helper: MD5 hex digest string
static std::string md5_hex(const std::string& input) {
    unsigned char hash[16];
    unsigned int hash_len = 0;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) return "";

    EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (int i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// Helper: read 32-bit big endian integer
static inline uint32_t read_be32(const void* ptr) {
    const uint8_t* b = static_cast<const uint8_t*>(ptr);
    return (static_cast<uint32_t>(b[0]) << 24) |
           (static_cast<uint32_t>(b[1]) << 16) |
           (static_cast<uint32_t>(b[2]) << 8)  |
           static_cast<uint32_t>(b[3]);
}

static inline uint32_t read_file_be32(FILE* fp) {
    uint8_t b[4];
    if (std::fread(b, 1, 4, fp) != 4) return 0;
    return (static_cast<uint32_t>(b[0]) << 24) |
           (static_cast<uint32_t>(b[1]) << 16) |
           (static_cast<uint32_t>(b[2]) << 8)  |
           static_cast<uint32_t>(b[3]);
}



// ==================== VERSION 0 ====================

int decrypt_v0(const std::string& db_path, const std::string& key, const std::string& out_path_b64) {
    // 1. Decode outPath base64
    std::string out_path = base64_decode(out_path_b64);
    if (out_path.empty()) {
        return -2; // invalid output path
    }

    // 2. Open input database
    FILE* fp = fopen(db_path.c_str(), "rb");
    if (!fp) {
        return -3; // can't access input file
    }

    // 3. Read 4-byte prefix
    uint32_t prefix = 0;
    if (fread(&prefix, 1, 4, fp) != 4) {
        fclose(fp);
        return -4;
    }

    // 4. Read 32-byte XORed MD5
    std::vector<uint8_t> xor_md5(32);
    if (fread(xor_md5.data(), 1, 32, fp) != 32) {
        fclose(fp);
        return -5;
    }

    // 5. Read 16-byte AES IV
    std::vector<uint8_t> iv(16);
    if (fread(iv.data(), 1, 16, fp) != 16) {
        fclose(fp);
        return -6;
    }

    // 6. Read remaining ciphertext to end of file
    std::vector<uint8_t> ciphertext;
    uint8_t buf[4096];
    while (true) {
        size_t n = fread(buf, 1, sizeof(buf), fp);
        if (n == 0) break;
        ciphertext.insert(ciphertext.end(), buf, buf + n);
    }
    fclose(fp);

    if (ciphertext.empty()) {
        return -7;
    }

    // 7. Derivate AES key: MD5 hex of key, take first 16 bytes
    std::string key_md5 = md5_hex(key);
    if (key_md5.empty()) {
        return -8;
    }
    std::vector<uint8_t> aes_key(16);
    std::memcpy(aes_key.data(), key_md5.data(), 16);

    // 8. Decrypt ciphertext using AES-128-CBC
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -9;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, aes_key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -10;
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    std::vector<uint8_t> plaintext(ciphertext.size());
    int decrypted_len = 0;
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &decrypted_len, ciphertext.data(), ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return -11;
    }
    int final_len = 0;
    EVP_DecryptFinal_ex(ctx, plaintext.data() + decrypted_len, &final_len);
    plaintext.resize(decrypted_len + final_len);
    EVP_CIPHER_CTX_free(ctx);

    // 9. Decompress standard legacy LZMA stream
    std::vector<uint8_t> decompressed_db;
    if (!lzma_decompress_legacy(plaintext, decompressed_db)) {
        return -12; // decompression failed
    }

    // 10. Write output file
    std::filesystem::path p(out_path);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }

    FILE* out_fp = fopen(out_path.c_str(), "wb");
    if (!out_fp) {
        return -13; // failed to create output file
    }
    size_t written = fwrite(decompressed_db.data(), 1, decompressed_db.size(), out_fp);
    fclose(out_fp);

    if (written != decompressed_db.size()) {
        return -14;
    }

    return 0; // success
}

// ==================== VERSION 1 ====================

int decrypt_v1(const std::string& db_path, const std::string& key_str, const std::string& out_dir, std::string& out_error_msg) {
    // 1. Open input database in read/write binary mode for in-place decryption
    FILE* fp = fopen(db_path.c_str(), "r+b");
    if (!fp) {
        out_error_msg = "Can't open input file: " + db_path;
        return -1;
    }

    // Get input file size
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // 2. Decrypt file in-place using AES-256-CBC
    std::vector<uint8_t> key(32, 0);
    size_t copy_len = std::min(key_str.size(), size_t(32));
    std::memcpy(key.data(), key_str.data(), copy_len);

    std::vector<uint8_t> iv(16, 0);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        fclose(fp);
        out_error_msg = "Failed to create EVP_CIPHER_CTX";
        return -2;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        fclose(fp);
        out_error_msg = "Failed to initialize AES-256-CBC";
        return -3;
    }

    EVP_CIPHER_CTX_set_padding(ctx, 0);

    const size_t chunk_size = 64 * 1024;
    std::vector<uint8_t> in_buf(chunk_size);
    std::vector<uint8_t> out_buf(chunk_size);

    bool first_chunk = true;
    while (true) {
        size_t read_bytes = std::fread(in_buf.data(), 1, chunk_size, fp);
        if (read_bytes == 0) {
            break;
        }

        int decrypted_len = 0;
        if (EVP_DecryptUpdate(ctx, out_buf.data(), &decrypted_len, in_buf.data(), read_bytes) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            fclose(fp);
            out_error_msg = "AES DecryptUpdate failed";
            return -4;
        }

        // Seek back to overwrite
        if (fseek(fp, -static_cast<long>(read_bytes), SEEK_CUR) != 0) {
            EVP_CIPHER_CTX_free(ctx);
            fclose(fp);
            out_error_msg = "Failed to seek back for in-place write";
            return -5;
        }

        size_t written = std::fwrite(out_buf.data(), 1, decrypted_len, fp);
        if (written != static_cast<size_t>(decrypted_len)) {
            EVP_CIPHER_CTX_free(ctx);
            fclose(fp);
            out_error_msg = "Failed to write decrypted chunk in-place";
            return -6;
        }

        // Sanity check first chunk for "ZDB4.0" magic
        if (first_chunk) {
            first_chunk = false;
            if (decrypted_len < 6 || std::memcmp(out_buf.data(), "ZDB4.0", 6) != 0) {
                EVP_CIPHER_CTX_free(ctx);
                fclose(fp);
                out_error_msg = "Decrypted magic check failed. Wrong password key or invalid archive format";
                return -7;
            }
        }
    }

    int final_len = 0;
    EVP_DecryptFinal_ex(ctx, out_buf.data(), &final_len);
    EVP_CIPHER_CTX_free(ctx);

    // 3. Read metadata block and verify checksum
    // The file format at this point:
    // Offset 0-5: "ZDB4.0"
    // Offset 6-9: offset of compressed payload (big-endian)
    // Offset 10-13: XXHash32 checksum of metadata (big-endian)
    // Offset 14...offset: Metadata block
    if (fseek(fp, 6, SEEK_SET) != 0) {
        fclose(fp);
        out_error_msg = "Failed to seek to header metadata offset";
        return -8;
    }

    uint32_t payload_offset = read_file_be32(fp);
    uint32_t expected_checksum = read_file_be32(fp);

    if (payload_offset <= 14) {
        fclose(fp);
        out_error_msg = "Invalid payload offset in header: " + std::to_string(payload_offset);
        return -9;
    }

    size_t metadata_size = payload_offset - 14;
    std::vector<uint8_t> metadata_block(metadata_size);
    if (fread(metadata_block.data(), 1, metadata_size, fp) != metadata_size) {
        fclose(fp);
        out_error_msg = "Failed to read header metadata block";
        return -10;
    }

    // Verify XXHash32 checksum of metadata_block
    uint32_t calculated_checksum = XXHash32::calculate(metadata_block.data(), metadata_block.size(), 0);
    if (calculated_checksum != expected_checksum) {
        fclose(fp);
        out_error_msg = "XXHash32 verification failed for metadata block: calculated " + 
                        std::to_string(calculated_checksum) + ", expected " + std::to_string(expected_checksum);
        return -11;
    }

    // 4. Parse files metadata from the verified metadata block
    // Metadata block starts with numFiles (4 bytes big-endian)
    if (metadata_block.size() < 4) {
        fclose(fp);
        out_error_msg = "Metadata block too small to contain numFiles";
        return -12;
    }

    uint32_t num_files = read_be32(metadata_block.data());
    size_t offset = 4;

    std::vector<std::tuple<std::string, size_t>> files;

    for (uint32_t i = 0; i < num_files; ++i) {
        if (offset + 4 > metadata_block.size()) {
            fclose(fp);
            out_error_msg = "Metadata block truncated reading filename length for file " + std::to_string(i);
            return -13;
        }
        uint32_t fn_len = read_be32(metadata_block.data() + offset);
        offset += 4;

        if (offset + fn_len > metadata_block.size()) {
            fclose(fp);
            out_error_msg = "Metadata block truncated reading filename for file " + std::to_string(i);
            return -14;
        }
        std::string filename(reinterpret_cast<char*>(metadata_block.data() + offset), fn_len);
        offset += fn_len;

        if (offset + 4 > metadata_block.size()) {
            fclose(fp);
            out_error_msg = "Metadata block truncated reading size for file " + std::to_string(i);
            return -15;
        }
        uint32_t file_size = read_be32(metadata_block.data() + offset);
        offset += 4;

        // Build full path: out_dir + "/" + filename
        std::filesystem::path full_path = std::filesystem::path(out_dir) / filename;
        files.push_back({full_path.string(), file_size});
    }

    // 5. Decompress the stream using lzma_decompress_stream
    if (fseek(fp, payload_offset, SEEK_SET) != 0) {
        fclose(fp);
        out_error_msg = "Failed to seek to compressed payload offset";
        return -16;
    }

    size_t stream_size = file_size - payload_offset;
    if (!lzma_decompress_stream(fp, stream_size, files)) {
        fclose(fp);
        out_error_msg = "Stream decompression failed";
        return -17;
    }

    fclose(fp);
    return 0; // success
}

} // namespace backup_format
