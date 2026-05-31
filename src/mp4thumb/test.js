const path = require('path');
const fs = require('fs');

// 1. Nạp file .node vừa compiled thành công
const nativePath = path.join(__dirname, 'build', 'Release', 'mp4thumb.node');
if (!fs.existsSync(nativePath)) {
    console.error(`[LỖI] Không tìm thấy file addon tại: ${nativePath}`);
    process.exit(1);
}
const addon = require(nativePath);

console.log("=== KIỂM TRA INTERFACE ADDON ===");
console.log("Các thành phần export:", addon);

// 2. Khởi tạo instance của Class MP4Thumb
const thumbGenerator = new addon.MP4Thumb();

// Đường dẫn file test
const videoInput = path.join(__dirname, 'sample.mp4');
const imgSyncOutput = path.join(__dirname, 'thumb_sync.jpg');
const imgAsyncOutput = path.join(__dirname, 'thumb_async.jpg');
const imgDefaultOutput = path.join(__dirname, 'thumb_default.jpg');

// Kiểm tra xem người dùng đã bỏ file video mẫu vào chưa
if (!fs.existsSync(videoInput)) {
    console.error(`\n[CẢNH BÁO] Chưa có file 'sample.mp4' ở thư mục này để chạy test thực tế.`);
    console.error(`Vui lòng copy 1 file video MP4 vào: ${videoInput} rồi chạy lại.\n`);
}

// -------------------------------------------------------------
// TEST CASE 1: Kiểm tra Error Handling (Truyền sai tham số)
// -------------------------------------------------------------
console.log("\n--- Test Case 1: Bắt lỗi khi không truyền tham số ---");
try {
    thumbGenerator.generateThumbnail(); // Không truyền gì
} catch (err) {
    console.log(`[OK] C++ đã bắt lỗi và ném exception về JS thành công: "${err.message}"`);
}

if (fs.existsSync(videoInput)) {
    // -------------------------------------------------------------
    // TEST CASE 2: Xử lý Đồng bộ (Synchronous) + Giới hạn kích thước
    // -------------------------------------------------------------
    console.log("\n--- Test Case 2: Tạo Thumbnail ĐỒNG BỘ (Sync) ---");
    console.log("Đang xử lý giải mã video...");
    try {
        const result = thumbGenerator.generateThumbnail(videoInput, imgSyncOutput, { maxWidth: 320, maxHeight: 240 });
        console.log(`[OK] Hàm đồng bộ trả về kết quả: ${result}`);
        if (fs.existsSync(imgSyncOutput)) {
            console.log(`[THÀNH CÔNG] File ảnh đồng bộ đã được tạo ra: ${imgSyncOutput}`);
        }
    } catch (err) {
        console.error("[LỖI] Xử lý đồng bộ thất bại:", err.message);
    }

    // -------------------------------------------------------------
    // TEST CASE 3: Test hàm setOutputPath mặc định
    // -------------------------------------------------------------
    console.log("\n--- Test Case 3: Test hàm setOutputPath ---");
    try {
        thumbGenerator.setOutputPath(imgDefaultOutput);
        // Không truyền tham số output path thứ 2, sử dụng cấu hình mặc định vừa set
        const result = thumbGenerator.generateThumbnail(videoInput, undefined, { maxWidth: 200, maxHeight: 200 });
        console.log(`[OK] Tạo ảnh qua đường dẫn default trả về: ${result}`);
        if (fs.existsSync(imgDefaultOutput)) {
            console.log(`[THÀNH CÔNG] File ảnh default đã được tạo ra tại: ${imgDefaultOutput}`);
        }
    } catch (err) {
        console.error("[LỖI] SetOutputPath thất bại:", err.message);
    }

    // -------------------------------------------------------------
    // TEST CASE 4: Xử lý BẤT ĐỒNG BỘ (Asynchronous) dùng Thread Pool
    // -------------------------------------------------------------
    console.log("\n--- Test Case 4: Tạo Thumbnail BẤT ĐỒNG BỘ (Async Worker) ---");
    console.log("Đang đẩy tác vụ vào Thread Pool ngầm của libuv...");

    thumbGenerator.generateThumbnailAsync(videoInput, imgAsyncOutput, { maxWidth: 640, maxHeight: 480 }, (err, success) => {
        console.log("\n[CALLBACK] Luồng ngầm xử lý xong và gọi về callback JavaScript!");
        if (err) {
            console.error("[LỖI Async]:", err);
        } else {
            console.log(`[OK] Trạng thái thành công: ${success}`);
            if (fs.existsSync(imgAsyncOutput)) {
                console.log(`[THÀNH CÔNG] File ảnh bất đồng bộ đã được tạo ra: ${imgAsyncOutput}`);
            }
        }
    });

    console.log(">> [LOG] Tiến trình JavaScript chính VẪN CHẠY TIẾP (Không bị block luồng)!");
    console.log("Đang đợi luồng ngầm hoàn thành...");
}