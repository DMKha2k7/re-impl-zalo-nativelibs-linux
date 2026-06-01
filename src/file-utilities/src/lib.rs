#![deny(clippy::all)]

use std::collections::HashMap;
use std::path::Path;
use std::sync::atomic::{AtomicBool, Ordering};
use std::sync::{Arc, Mutex};

use globset::{Glob, GlobSet, GlobSetBuilder};
use napi_derive::napi;

// ─── Job Registry ────────────────────────────────────────────────────────────

lazy_static::lazy_static! {
    static ref JOB_REGISTRY: Mutex<HashMap<u64, Arc<AtomicBool>>> = Mutex::new(HashMap::new());
}

fn remove_job(id: u64) {
    JOB_REGISTRY.lock().unwrap().remove(&id);
}

// ─── cancelJob ───────────────────────────────────────────────────────────────

/// Cancel a running async job by its id.
#[napi(js_name = "cancelJob")]
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

// ─── Option structs ──────────────────────────────────────────────────────────

#[napi(object)]
#[derive(Default)]
pub struct GetDirectorySizeOptions {
    pub workers: Option<i32>,
}

#[napi(object)]
#[derive(Default)]
pub struct GetDirectorySizeTreeOptions {
    pub max_depth: Option<i32>,
    pub workers: Option<i32>,
    pub include_root: Option<bool>,
}

#[napi(object)]
#[derive(Default)]
pub struct GetDirectorySizeByGlobOptions {
    pub workers: Option<i32>,
}

// ─── Filesystem detection ─────────────────────────────────────────────────────

fn detect_fs_type_linux(path: &str) -> String {
    use std::ffi::CString;

    let cpath = match CString::new(path) {
        Ok(s) => s,
        Err(_) => return "unknown".to_string(),
    };

    let mut buf: libc::statfs64 = unsafe { std::mem::zeroed() };
    let ret = unsafe { libc::statfs64(cpath.as_ptr(), &mut buf) };
    if ret != 0 {
        return "unknown".to_string();
    }

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

#[napi(js_name = "detectFilesystemSync")]
pub fn detect_filesystem_sync(path: String) -> FilesystemInfo {
    do_detect_filesystem(path)
}

#[napi(js_name = "detectFilesystemAsync")]
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

    let no_hardlink = [
        0x4d44u64,       // MSDOS/FAT
        0x2011BAB0u64,   // exFAT
        0x5346544eu64,   // NTFS (limited)
        0x9660u64,       // ISO9660
        0x65735546u64,   // FUSE
        0x73717368u64,   // SquashFS
        0x28CD3D45u64,   // CramFS
    ];

    !no_hardlink.contains(&(buf.f_type as u64))
}

#[napi(js_name = "detectHardlinksSync")]
pub fn detect_hardlinks_sync(path: String) -> bool {
    do_detect_hardlinks(&path)
}

#[napi(js_name = "detectHardlinksAsync")]
pub async fn detect_hardlinks_async(path: String) -> napi::Result<bool> {
    let result = tokio::task::spawn_blocking(move || do_detect_hardlinks(&path))
        .await
        .map_err(|e| napi::Error::from_reason(e.to_string()))?;
    Ok(result)
}

// ─── Directory size ───────────────────────────────────────────────────────────

fn do_get_directory_size(path: &Path, cancel: Option<&Arc<AtomicBool>>) -> u64 {
    let mut total: u64 = 0;
    let walker = walkdir::WalkDir::new(path).follow_links(false);

    for entry in walker {
        if let Some(c) = cancel {
            if c.load(Ordering::Relaxed) {
                break;
            }
        }
        // FIX: Avoid using filter_map to prevent silently swallowing data on permission denied or access errors
        match entry {
            Ok(e) => {
                if e.file_type().is_file() {
                    if let Ok(meta) = e.metadata() {
                        total += meta.len();
                    }
                }
            }
            Err(err) => {
                // Log warning to stderr instead of crashing or stopping the entire process
                eprintln!("[file-utilities] Warning walking entry: {}", err);
            }
        }
    }
    total
}

#[napi(js_name = "getDirectorySizeSync")]
pub fn get_directory_size_sync(path: String, _options: Option<GetDirectorySizeOptions>) -> f64 {
    do_get_directory_size(Path::new(&path), None) as f64
}

#[napi(js_name = "getDirectorySizeAsync")]
pub async fn get_directory_size_async(
    path: String,
    _options: Option<GetDirectorySizeOptions>,
    job_id: Option<f64>,
) -> napi::Result<f64> {
    let cancel = if let Some(jid) = job_id {
        let flag = Arc::new(AtomicBool::new(false));
        JOB_REGISTRY.lock().unwrap().insert(jid as u64, flag.clone());
        Some(flag)
    } else {
        None
    };
    let cancel_clone = cancel.clone();
    let result = tokio::task::spawn_blocking(move || {
        let r = do_get_directory_size(Path::new(&path), cancel_clone.as_ref());
        if let Some(jid) = job_id {
            remove_job(jid as u64);
        }
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

// FIX: Add max_depth and current_depth to the recursive function to enforce the depth limit configured from JS
fn do_get_directory_size_tree(path: &Path, max_depth: i32, current_depth: i32, cancel: Option<&Arc<AtomicBool>>) -> DirTreeEntry {
    let mut total: u64 = 0;
    let mut children: Vec<DirTreeEntry> = Vec::new();

    if current_depth > max_depth {
        return DirTreeEntry {
            path: path.to_string_lossy().into_owned(),
            size: 0.0,
            is_directory: true,
            children: Some(vec![]),
        };
    }

    let read_dir = match std::fs::read_dir(path) {
        Ok(rd) => rd,
        Err(err) => {
            eprintln!("[file-utilities] Cannot read dir {:?}: {}", path, err);
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
            if current_depth < max_depth {
                let child = do_get_directory_size_tree(&child_path, max_depth, current_depth + 1, cancel);
                total += child.size as u64;
                children.push(child);
            }
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

#[napi(js_name = "getDirectorySizeTreeSync")]
pub fn get_directory_size_tree_sync(
    path: String,
    options: Option<GetDirectorySizeTreeOptions>,
) -> DirTreeEntry {
    let opts = options.unwrap_or_default();
    let max_depth = opts.max_depth.unwrap_or(3); // Default depth matching index.js sync mode
    do_get_directory_size_tree(Path::new(&path), max_depth, 0, None)
}

#[napi(js_name = "getDirectorySizeTreeAsync")]
pub async fn get_directory_size_tree_async(
    path: String,
    options: Option<GetDirectorySizeTreeOptions>,
    job_id: Option<f64>,
) -> napi::Result<DirTreeEntry> {
    let opts = options.unwrap_or_default();
    let max_depth = opts.max_depth.unwrap_or(10); // Default depth matching index.js async mode

    let cancel = if let Some(jid) = job_id {
        let flag = Arc::new(AtomicBool::new(false));
        JOB_REGISTRY.lock().unwrap().insert(jid as u64, flag.clone());
        Some(flag)
    } else {
        None
    };
    let cancel_clone = cancel.clone();
    let result = tokio::task::spawn_blocking(move || {
        let r = do_get_directory_size_tree(Path::new(&path), max_depth, 0, cancel_clone.as_ref());
        if let Some(jid) = job_id {
            remove_job(jid as u64);
        }
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
    let walker = walkdir::WalkDir::new(path).follow_links(false);

    for entry in walker {
        if let Some(c) = cancel {
            if c.load(Ordering::Relaxed) {
                break;
            }
        }
        
        // FIX: Handle errors explicitly to prevent silent data drop during glob matching
        match entry {
            Ok(e) => {
                if !e.file_type().is_file() {
                    continue;
                }
                let file_path = e.path();
                let rel = file_path.strip_prefix(path).unwrap_or(&file_path);
                let filename = file_path.file_name().and_then(|n| n.to_str()).unwrap_or("");

                if glob_set.is_match(rel) || glob_set.is_match(filename) {
                    if let Ok(meta) = e.metadata() {
                        total += meta.len();
                    }
                }
            }
            Err(err) => {
                eprintln!("[file-utilities] Warning walking glob entry: {}", err);
            }
        }
    }
    total
}

fn split_glob_pattern(pattern: &str) -> (String, String) {
    let wildcards = ['*', '?', '[', '{'];
    if let Some(idx) = pattern.chars().position(|c| wildcards.contains(&c)) {
        let prefix = &pattern[..idx];
        if let Some(sep_idx) = prefix.rfind('/') {
            let dir = &prefix[..sep_idx];
            let glob = &pattern[sep_idx + 1..];
            (
                if dir.is_empty() { "/".to_string() } else { dir.to_string() },
                glob.to_string(),
            )
        } else {
            (".".to_string(), pattern.to_string())
        }
    } else {
        let path = Path::new(pattern);
        if path.is_dir() {
            (pattern.to_string(), "**/*".to_string())
        } else if let Some(parent) = path.parent() {
            let dir = parent.to_string_lossy().into_owned();
            let file = path.file_name().unwrap_or_default().to_string_lossy().into_owned();
            (if dir.is_empty() { ".".to_string() } else { dir }, file)
        } else {
            (".".to_string(), pattern.to_string())
        }
    }
}

#[napi(js_name = "getDirectorySizeByGlobSync")]
pub fn get_directory_size_by_glob_sync(
    pattern: String,
    _options: Option<GetDirectorySizeByGlobOptions>,
) -> napi::Result<f64> {
    let (dir, glob) = split_glob_pattern(&pattern);
    let glob_set = build_glob_set(&[glob])?;
    Ok(do_get_directory_size_by_glob(Path::new(&dir), &glob_set, None) as f64)
}

#[napi(js_name = "getDirectorySizeByGlobAsync")]
pub async fn get_directory_size_by_glob_async(
    pattern: String,
    _options: Option<GetDirectorySizeByGlobOptions>,
    job_id: Option<f64>,
) -> napi::Result<f64> {
    let (dir, glob) = split_glob_pattern(&pattern);
    let glob_set = build_glob_set(&[glob])?;

    let cancel = if let Some(jid) = job_id {
        let flag = Arc::new(AtomicBool::new(false));
        JOB_REGISTRY.lock().unwrap().insert(jid as u64, flag.clone());
        Some(flag)
    } else {
        None
    };
    let cancel_clone = cancel.clone();
    let result = tokio::task::spawn_blocking(move || {
        let r = do_get_directory_size_by_glob(Path::new(&dir), &glob_set, cancel_clone.as_ref());
        if let Some(jid) = job_id {
            remove_job(jid as u64);
        }
        r
    })
    .await
    .map_err(|e| napi::Error::from_reason(e.to_string()))?;
    Ok(result as f64)
}