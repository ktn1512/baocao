# Quá trình chạy code C/C++

Sau khi biên dịch và linking, chương trình có một file thực thi. Khi người dùng chạy file này, hệ điều hành sẽ nạp chương trình vào bộ nhớ và điều phối việc thực thi.

```text
[File thực thi]
      ↓
[OS Loader đọc file]
      ↓
[Tạo process và cấp PID]
      ↓
[Nạp chương trình vào bộ nhớ]
      ↓
[Nạp thư viện động (.dll/.so)]
      ↓
[Khởi tạo runtime C/C++]
      ↓
[Gọi main()]
      ↓
[CPU thực thi từng lệnh]
      ↓
[Xuất kết quả, ví dụ: hello]
      ↓
[Dọn dẹp và kết thúc process]
```

## 1. Tạo process và nạp chương trình

- Hệ điều hành tạo một process mới và cấp cho process một PID.
- OS Loader đọc cấu trúc file thực thi: PE trên Windows hoặc ELF trên Linux.
- Chương trình được ánh xạ vào không gian địa chỉ ảo của process.
- Các vùng nhớ chính gồm:
  - **Text:** chứa mã máy của chương trình.
  - **Data/BSS:** chứa biến toàn cục và biến `static`.
  - **Heap:** dùng cho bộ nhớ cấp phát bằng `new` hoặc `malloc()`.
  - **Stack:** chứa biến cục bộ, tham số hàm và thông tin lời gọi hàm.

## 2. Nạp thư viện động

Nếu chương trình dùng thư viện động, dynamic linker sẽ nạp các file `.dll` trên Windows hoặc `.so` trên Linux vào bộ nhớ và liên kết các hàm cần dùng.

## 3. Khởi tạo runtime

Trước khi gọi `main()`, runtime C/C++ sẽ:

- Chuẩn bị các đối số `argc`, `argv`.
- Khởi tạo biến toàn cục và biến `static`.
- Gọi constructor của các đối tượng toàn cục trong C++.
- Chuẩn bị các luồng vào/ra như `std::cin`, `std::cout` và `std::cerr`.

## 4. Thực thi `main()`

Sau khi runtime hoàn tất, quyền điều khiển được chuyển cho `main()`. CPU thực thi các lệnh trong chương trình.

Ví dụ:

```cpp
#include <iostream>

int main() {
    std::cout << "hello";
    return 0;
}
```

Chuỗi `hello` được xuất ra khi CPU thực thi câu lệnh `std::cout` trong `main()`. Vì vậy, linking chỉ tạo file thực thi; phải chạy file đó thì chương trình mới in ra kết quả.

## 5. Kết thúc chương trình

Khi `main()` trả về hoặc chương trình gọi `exit()`:

- Các destructor cần thiết được gọi.
- Bộ đệm I/O được xả và các file được đóng.
- Mã thoát được trả về cho hệ điều hành; thông thường `0` nghĩa là thành công.
- Hệ điều hành thu hồi bộ nhớ và kết thúc process.
