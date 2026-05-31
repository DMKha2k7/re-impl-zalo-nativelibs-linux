#ifndef LZMA_DECODER_H
#define LZMA_DECODER_H

#include <vector>
#include <string>
#include <cstdint>
#include <tuple>

// Legacy LZMA decompression (5 bytes properties + 8 bytes uncompressed size + raw stream)
bool lzma_decompress_legacy(const std::vector<uint8_t>& input, std::vector<uint8_t>& output);

// Stream LZMA/XZ decompression from file stream to a list of target output files.
// files: vector of (filePath, fileSize)
bool lzma_decompress_stream(FILE* fp,
                            size_t stream_size,
                            const std::vector<std::tuple<std::string, size_t>>& files);

#endif // LZMA_DECODER_H
