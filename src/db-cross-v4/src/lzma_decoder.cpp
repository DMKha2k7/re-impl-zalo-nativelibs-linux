#include "lzma_decoder.h"
#include <lzma.h>
#include <algorithm>
#include <filesystem>
#include <cstdio>
#include <cstring>

bool lzma_decompress_legacy(const std::vector<uint8_t>& input, std::vector<uint8_t>& output) {
    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_ret ret = lzma_alone_decoder(&strm, UINT64_MAX);
    if (ret != LZMA_OK) {
        return false;
    }

    strm.next_in = input.data();
    strm.avail_in = input.size();

    const size_t buf_size = 64 * 1024;
    std::vector<uint8_t> out_buf(buf_size);

    output.clear();

    while (true) {
        strm.next_out = out_buf.data();
        strm.avail_out = buf_size;

        ret = lzma_code(&strm, LZMA_RUN);
        
        size_t written = buf_size - strm.avail_out;
        if (written > 0) {
            output.insert(output.end(), out_buf.begin(), out_buf.begin() + written);
        }

        if (ret == LZMA_STREAM_END) {
            break;
        }

        if (ret != LZMA_OK) {
            lzma_end(&strm);
            return false;
        }
    }

    lzma_end(&strm);
    return true;
}

bool lzma_decompress_stream(FILE* fp,
                            size_t stream_size,
                            const std::vector<std::tuple<std::string, size_t>>& files) {
    if (!fp) return false;

    lzma_stream strm = LZMA_STREAM_INIT;
    lzma_ret ret = lzma_stream_decoder(&strm, UINT64_MAX, 0);
    if (ret != LZMA_OK) {
        return false;
    }

    const size_t in_buf_size = 64 * 1024;
    const size_t out_buf_size = 64 * 1024;
    std::vector<uint8_t> in_buf(in_buf_size);
    std::vector<uint8_t> out_buf(out_buf_size);

    size_t remaining_stream = stream_size;
    size_t current_file_idx = 0;
    size_t current_file_written = 0;
    FILE* out_fp = nullptr;

    strm.avail_in = 0;
    strm.next_in = nullptr;

    bool success = true;

    while (success) {
        if (strm.avail_in == 0 && remaining_stream > 0) {
            size_t to_read = std::min(in_buf_size, remaining_stream);
            size_t read_bytes = fread(in_buf.data(), 1, to_read, fp);
            if (read_bytes == 0) {
                break;
            }
            strm.next_in = in_buf.data();
            strm.avail_in = read_bytes;
            remaining_stream -= read_bytes;
        }

        strm.next_out = out_buf.data();
        strm.avail_out = out_buf_size;

        ret = lzma_code(&strm, remaining_stream == 0 && strm.avail_in == 0 ? LZMA_FINISH : LZMA_RUN);

        size_t decompressed = out_buf_size - strm.avail_out;
        if (decompressed > 0) {
            size_t offset = 0;
            while (offset < decompressed) {
                if (current_file_idx >= files.size()) {
                    break;
                }

                const auto& [file_path, file_size] = files[current_file_idx];
                
                if (!out_fp) {
                    std::filesystem::path p(file_path);
                    if (p.has_parent_path()) {
                        std::filesystem::create_directories(p.parent_path());
                    }
                    out_fp = fopen(file_path.c_str(), "wb");
                    if (!out_fp) {
                        success = false;
                        break;
                    }
                    current_file_written = 0;
                }

                size_t to_write = std::min(decompressed - offset, file_size - current_file_written);
                if (to_write > 0) {
                    fwrite(out_buf.data() + offset, 1, to_write, out_fp);
                    current_file_written += to_write;
                    offset += to_write;
                }

                if (current_file_written >= file_size) {
                    fclose(out_fp);
                    out_fp = nullptr;
                    current_file_idx++;
                }
            }
        }

        if (ret == LZMA_STREAM_END) {
            break;
        }

        if (ret != LZMA_OK) {
            success = false;
            break;
        }
    }

    if (out_fp) {
        fclose(out_fp);
    }
    lzma_end(&strm);

    return success;
}
