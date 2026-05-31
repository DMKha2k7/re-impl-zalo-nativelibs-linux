#![deny(clippy::all)]

use std::collections::HashMap;
use std::path::Path;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::{Arc, Mutex};

use globset::{Glob, GlobSet, GlobSetBuilder};
use napi_derive::napi;

// ─── Job Registry ────────────────────────────────────────────────────────────

static JOB_COUNTER: std::sync::atomic::AtomicU64 = std::sync::atomic::AtomicU64::new(1);

lazy_static::lazy_static! {
    static ref JOB_REGISTRY: Mutex<HashMap<u64, Arc<AtomicBool>>> = Mutex::new(HashMap::new());
}

fn new_job() -> (u64, Arc<AtomicBool>) {
    let id = JOB_COUNTER.fetch_add(1, Ordering::SeqCst);
    let flag = Arc::new(AtomicBool::new(false));
    JOB_REGISTRY.lock().unwrap().insert(id, flag.clone());
    (id, flag)
}

fn remove_job(id: u64) {
    JOB_REGISTRY.lock().unwrap().remove(&id);
}

// ─── cancelJob ───────────────────────────────────────────────────────────────

/// Cancel a running async job by its id.
/// Returns true if the job was found and cancelled, false otherwise.
#[napi]
pub fn cancel_job(job_id: f64) -> bool {
    let id = job_id as u64;
    let registry = JOB_REGISTRY.lock().unwrap();
    if let Some(flag) = registry.get(&id) {
        flag.store(true, Ordering::SeqCst);
        true
    } else {
        false
    }
}

// ─── Filesystem detection ─────────────────────────────────────────────────────

fn detect_fs_type_linux(path: &str) -> String {
    use std::ffi::CString;

    let cpath = match CString::new(path) {
        Ok(s) => s,
        Err(_) => return "unknown".to_string(),
    };

    // Use statfs(2) via libc
    let mut buf: libc::statfs64 = unsafe { std::mem::zeroed() };
    let ret = unsafe { libc::statfs64(cpath.as_ptr(), &mut buf) };
    if ret != 0 {
        return "unknown".to_string();
    }

    // f_type magic numbers (Linux-specific)
    match buf.f_type as u64 {
        0xEF53 => "ext4".to_string(),
        0x52654973 => "reiserfs".to_string(),
        0x58465342 => "xfs".to_string(),
        0x9123683E => "btrfs".to_string(),
        0x6969 => "nfs".to_string(),
        0xFF534D42 | 0xFE534D42 => "cifs".to_string(),
        0x4d44 => "vfat".to_string(),
        0x2011BAB0 => "exfat".to_string(),
        0x5346544e => "ntfs".to_string(),
        0x01021994 => "tmpfs".to_string(),
        0xF2F52010 => "f2fs".to_string(),
        0x65735546 => "fuse".to_string(),
        0x28CD3D45 => "cramfs".to_string(),
        0x73717368 => "squashfs".to_string(),
        magic => format!("0x{:x}", magic),
    }
}

fn detect_fs_type_name_max(path: &str) -> (String, i64) {
    let fs_type = detect_fs_type_linux(path);

    let name_max: i64 = {
        use std::ffi::CString;
        let cpath = CString::new(path).unwrap_or_default();
        let mut sv: libc::statvfs64 = unsafe { std::mem::zeroed() };
        let r = unsafe { libc::statvfs64(cpath.as_ptr(), &mut sv) };
        if r == 0 { sv.f_namemax as i64 } else { 255 }
    };

    (fs_type, name_max)
}

#[napi(object)]
pub struct FilesystemInfo {
    pub path: String,
    pub filesystem_type: String,
    pub volume_name: String,
    pub max_filename_length: i64,
    pub supports_case_sensitive_names: bool,
    pub supports_unicode_filenames: bool,
    pub supports_compression: bool,
    pub supports_encryption: bool,
}

fn do_detect_filesystem(path: String) -> FilesystemInfo {
    let (fs_type, name_max) = detect_fs_type_name_max(&path);

    let case_sensitive = !matches!(
        fs_type.as_str(),
        "vfat" | "exfat" | "ntfs" | "fat16"
    );
    let unicode = !matches!(fs_type.as_str(), "vfat" | "fat16");
    let compression = matches!(fs_type.as_str(), "btrfs" | "ntfs" | "squashfs" | "cramfs");

    FilesystemInfo {
        path,
        filesystem_type: fs_type,
        volume_name: String::new(),
        max_filename_length: name_max,
        supports_case_sensitive_names: case_sensitive,
        supports_unicode_filenames: unicode,
        supports_compression: compression,
        supports_encryption: false,
    }
}

/// Detect filesystem properties for a given path synchronously.
#[napi]
pub fn detect_filesystem_sync(path: String) -> FilesystemInfo {
    do_detect_filesystem(path)
}

/// Detect filesystem properties for a given path asynchronously.
#[napi]
pub async fn detect_filesystem_async(path: String) -> napi::Result<FilesystemInfo> {
    let result = tokio::task::spawn_blocking(move || do_detect_filesystem(path))
        .await
        .map_err(|e| napi::Error::from_reason(e.to_string()))?;
    Ok(result)
}

// ─── Hardlinks detection ──────────────────────────────────────────────────────

fn do_detect_hardlinks(path: &str) -> bool {
    use std::ffi::CString;
    let cpath = match CString::new(path) {
        Ok(s) => s,
        Err(_) => return false,
    };

    let mut buf: libc::statfs64 = unsafe { std::mem::zeroed() };
    let ret = unsafe { libc::statfs64(cpath.as_ptr(), &mut buf) };
    if ret != 0 {
        return false;
    }

    // Filesystems that do NOT support hardlinks
    let no_hardlink = [
        0x4d44u64,       // MSDOS/FAT
        0x2011BAB0u64,   // exFAT
        0x5346544eu64,   // NTFS (limited)
        0x9660u64,       // ISO9660
        0x65735546u64,   // FUSE (unknown, assume no)
        0x73717368u64,   // SquashFS (read-only)
        0x28CD3D45u64,   // CramFS (read-only)
    ];

    !no_hardlink.contains(&(buf.f_type as u64))
}

/// Detect whether the filesystem at path supports hardlinks (synchronous).
#[napi]
pub fn detect_hardlinks_sync(path: String) -> bool {
    do_detect_hardlinks(&path)
}

/// Detect whether the filesystem at path supports hardlinks (asynchronous).
#[napi]
pub async fn detect_hardlinks_async(path: String) -> napi::Result<bool> {
    let result = tokio::task::spawn_blocking(move || do_detect_hardlinks(&path))
        .await
        .map_err(|e| napi::Error::from_reason(e.to_string()))?;
    Ok(result)
}

// ─── Directory size ───────────────────────────────────────────────────────────

fn do_get_directory_size(path: &Path, cancel: Option<&Arc<AtomicBool>>) -> u64 {
    let mut total: u64 = 0;
    let walker = walkdir::WalkDir::new(path)
        .follow_links(false)
        .into_iter()
        .filter_map(|e| e.ok());

    for entry in walker {
        if let Some(c) = cancel {
            if c.load(Ordering::Relaxed) {
                break;
            }
        }
        if entry.file_type().is_file() {
            if let Ok(meta) = entry.metadata() {
                total += meta.len();
            }
        }
    }
    total
}

/// Compute total size of all files in a directory recursively (synchronous).
#[napi]
pub fn get_directory_size_sync(path: String) -> f64 {
    do_get_directory_size(Path::new(&path), None) as f64
}

/// Compute total size of all files in a directory recursively (asynchronous).
/// Returns a Promise<f64>.
#[napi]
pub async fn get_directory_size_async(path: String) -> napi::Result<f64> {
    let (job_id, cancel) = new_job();
    let result = tokio::task::spawn_blocking(move || {
        let r = do_get_directory_size(Path::new(&path), Some(&cancel));
        remove_job(job_id);
        r
    })
    .await
    .map_err(|e| napi::Error::from_reason(e.to_string()))?;
    Ok(result as f64)
}

// ─── Directory size tree ──────────────────────────────────────────────────────

#[napi(object)]
pub struct DirTreeEntry {
    pub path: String,
    pub size: f64,
    pub is_directory: bool,
    pub children: Option<Vec<DirTreeEntry>>,
}

fn do_get_directory_size_tree(path: &Path, cancel: Option<&Arc<AtomicBool>>) -> DirTreeEntry {
    let mut total: u64 = 0;
    let mut children: Vec<DirTreeEntry> = Vec::new();

    let read_dir = match std::fs::read_dir(path) {
        Ok(rd) => rd,
        Err(_) => {
            return DirTreeEntry {
                path: path.to_string_lossy().into_owned(),
                size: 0.0,
                is_directory: true,
                children: Some(vec![]),
            }
        }
    };

    for entry in read_dir.flatten() {
        if let Some(c) = cancel {
            if c.load(Ordering::Relaxed) {
                break;
            }
        }

        let child_path = entry.path();
        let ft = match entry.file_type() {
            Ok(ft) => ft,
            Err(_) => continue,
        };

        if ft.is_dir() {
            let child = do_get_directory_size_tree(&child_path, cancel);
            total += child.size as u64;
            children.push(child);
        } else if ft.is_file() {
            let size = entry.metadata().map(|m| m.len()).unwrap_or(0);
            total += size;
            children.push(DirTreeEntry {
                path: child_path.to_string_lossy().into_owned(),
                size: size as f64,
                is_directory: false,
                children: None,
            });
        }
    }

    DirTreeEntry {
        path: path.to_string_lossy().into_owned(),
        size: total as f64,
        is_directory: true,
        children: Some(children),
    }
}

/// Get directory size tree (synchronous).
#[napi]
pub fn get_directory_size_tree_sync(path: String) -> DirTreeEntry {
    do_get_directory_size_tree(Path::new(&path), None)
}

/// Get directory size tree (asynchronous).
#[napi]
pub async fn get_directory_size_tree_async(path: String) -> napi::Result<DirTreeEntry> {
    let (job_id, cancel) = new_job();
    let result = tokio::task::spawn_blocking(move || {
        let r = do_get_directory_size_tree(Path::new(&path), Some(&cancel));
        remove_job(job_id);
        r
    })
    .await
    .map_err(|e| napi::Error::from_reason(e.to_string()))?;
    Ok(result)
}

// ─── Directory size by glob ───────────────────────────────────────────────────

fn build_glob_set(patterns: &[String]) -> napi::Result<GlobSet> {
    let mut builder = GlobSetBuilder::new();
    for p in patterns {
        let glob = Glob::new(p)
            .map_err(|e| napi::Error::from_reason(e.to_string()))?;
        builder.add(glob);
    }
    builder.build()
        .map_err(|e| napi::Error::from_reason(e.to_string()))
}

fn do_get_directory_size_by_glob(
    path: &Path,
    glob_set: &GlobSet,
    cancel: Option<&Arc<AtomicBool>>,
) -> u64 {
    let mut total: u64 = 0;
    let walker = walkdir::WalkDir::new(path)
        .follow_links(false)
        .into_iter()
        .filter_map(|e| e.ok());

    for entry in walker {
        if let Some(c) = cancel {
            if c.load(Ordering::Relaxed) {
                break;
            }
        }
        if !entry.file_type().is_file() {
            continue;
        }

        let file_path = entry.path();
        // Match against full path relative to base, or just filename
        let rel = file_path.strip_prefix(path).unwrap_or(file_path);
        let filename = file_path.file_name().and_then(|n| n.to_str()).unwrap_or("");

        if glob_set.is_match(rel) || glob_set.is_match(filename) {
            if let Ok(meta) = entry.metadata() {
                total += meta.len();
            }
        }
    }
    total
}

/// Get directory size counting only files matching glob patterns (synchronous).
#[napi]
pub fn get_directory_size_by_glob_sync(path: String, globs: Vec<String>) -> napi::Result<f64> {
    let glob_set = build_glob_set(&globs)
        .map_err(|e| napi::Error::from_reason(format!("Invalid glob pattern: {}", e)))?;
    Ok(do_get_directory_size_by_glob(Path::new(&path), &glob_set, None) as f64)
}

/// Get directory size counting only files matching glob patterns (asynchronous).
#[napi]
pub async fn get_directory_size_by_glob_async(
    path: String,
    globs: Vec<String>,
) -> napi::Result<f64> {
    let glob_set = build_glob_set(&globs)
        .map_err(|e| napi::Error::from_reason(format!("Invalid glob pattern: {}", e)))?;

    let (job_id, cancel) = new_job();
    let result = tokio::task::spawn_blocking(move || {
        let r = do_get_directory_size_by_glob(Path::new(&path), &glob_set, Some(&cancel));
        remove_job(job_id);
        r
    })
    .await
    .map_err(|e| napi::Error::from_reason(e.to_string()))?;
    Ok(result as f64)
}
