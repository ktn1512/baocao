# Quá trình chạy code C/C++

Khi file thực thi C/C++ được khởi chạy (`.exe` trên Windows hoặc ELF binary trên Linux), hệ điều hành sẽ tạo tiến trình, nạp chương trình vào bộ nhớ và điều phối việc thực thi.

```text
[OS Loader] → [Tạo Process] → [Nạp vào Memory]
            → [Dynamic Linking] → [Khởi tạo Runtime]
            → [main()] → [Dọn dẹp và Exit]
```

## 1. Nạp chương trình vào bộ nhớ

- Hệ điều hành tạo một tiến trình mới và cấp cho tiến trình một PID.
- Loader đọc header của file thực thi (PE trên Windows hoặc ELF trên Linux).
- Hệ điều hành thiết lập không gian địa chỉ ảo và ánh xạ các vùng nhớ cần thiết.

## 2. Cấu trúc bộ nhớ của tiến trình

```text
Địa chỉ cao
┌────────────────────────┐
│ STACK                  │ ← Biến cục bộ, lời gọi hàm
│ Memory Mapping         │ ← Thư viện .dll / .so
│ HEAP                   │ ← Bộ nhớ cấp phát bằng new/malloc
│ BSS                    │ ← Biến global/static chưa khởi tạo
│ DATA                   │ ← Biến global/static đã khởi tạo
│ TEXT                   │ ← Mã máy của chương trình
└────────────────────────┘
Địa chỉ thấp
```

- **Text:** Chứa mã máy, thường được đặt ở chế độ chỉ đọc.
- **Data/BSS:** Chứa biến toàn cục và biến `static`.
- **Heap:** Dùng cho bộ nhớ cấp phát động bằng `new`, `delete`, `malloc()` và `free()`.
- **Stack:** Chứa biến cục bộ, tham số hàm và địa chỉ quay về.

## 3. Nạp thư viện động

Nếu chương trình sử dụng thư viện động, dynamic linker sẽ nạp các file `.dll` hoặc `.so` vào bộ nhớ và liên kết địa chỉ của các hàm cần sử dụng. Trên Linux, dynamic linker thường là `ld-linux.so`.

## 4. Khởi tạo Runtime

Chương trình không bắt đầu trực tiếp từ `main()`. Trước đó, C Runtime (CRT) thực hiện một số công việc:

- Chuẩn bị môi trường thực thi và các đối số `argc`, `argv`.
- Khởi tạo biến toàn cục và biến `static`.
- Gọi constructor của các đối tượng toàn cục trong C++.
- Khởi tạo các luồng vào/ra như `std::cin`, `std::cout`, `std::cerr`.

Sau khi hoàn tất, CRT chuyển quyền điều khiển cho hàm `main()`.

## 5. Thực thi hàm `main()`

- Mỗi lần gọi hàm, một **stack frame** được tạo trên Stack.
- Biến cục bộ và địa chỉ quay về được lưu trong stack frame.
- `new` hoặc `malloc()` cấp phát bộ nhớ trên Heap.
- Khi hàm kết thúc, stack frame của hàm được giải phóng.

## 6. Kết thúc chương trình

Khi `main()` trả về hoặc chương trình gọi `exit()`:

- Các destructor của đối tượng toàn cục/static được gọi theo thứ tự ngược lại.
- Các bộ đệm I/O được xả và các file được đóng.
- Mã thoát (exit code) được trả về cho hệ điều hành; thông thường `0` nghĩa là thành công.
- Hệ điều hành thu hồi bộ nhớ, đóng tài nguyên và kết thúc tiến trình.
