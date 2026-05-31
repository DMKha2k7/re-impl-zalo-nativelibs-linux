#ifndef BACKUP_FORMAT_H
#define BACKUP_FORMAT_H

#include <string>

namespace backup_format {

// Decrypt and decompress Version 0 database backup.
// Returns 0 on success, or non-zero error code.
int decrypt_v0(const std::string& db_path, const std::string& key, const std::string& out_path);

// Decrypt and decompress Version 1 database backup.
// Returns 0 on success, or non-zero error code.
int decrypt_v1(const std::string& db_path, const std::string& key, const std::string& out_dir, std::string& out_error_msg);

} // namespace backup_format

#endif // BACKUP_FORMAT_H
