#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <vector>
#include <cstdint>

// Decode a Base64 encoded string into a vector of bytes.
std::vector<uint8_t> base64_decode(const std::string& input);

// Encode a vector of bytes into a Base64 string.
std::string base64_encode(const std::vector<uint8_t>& input);

// Compute MD5 hash of a byte vector, returning a 32-char lowercase hex string.
std::string md5_hex(const std::vector<uint8_t>& data);

// Perform AES-128-CBC decryption.
bool aes_128_cbc_decrypt(const uint8_t* key, const uint8_t* iv,
                         const uint8_t* ciphertext, size_t ciphertext_len,
                         std::vector<uint8_t>& plaintext);

// Perform AES-256-CBC decryption.
bool aes_256_cbc_decrypt(const uint8_t* key, const uint8_t* iv,
                         const uint8_t* ciphertext, size_t ciphertext_len,
                         std::vector<uint8_t>& plaintext);

#endif // CRYPTO_H
