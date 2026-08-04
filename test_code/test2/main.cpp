#include <iostream>
#include <fcntl.h>      // open(), O_WRONLY
#include <sys/stat.h>   // mkfifo()
#include <unistd.h>     // write(), close()
#include <string>

int main() {
    const char* fifo_path = "/tmp/my_named_fifo";

    // 1. Tạo tệp FIFO (nếu chưa tồn tại) với quyền 0666 (đọc/ghi)
    mkfifo(fifo_path, 0666);

    std::cout << "[Writer Process]: Đang chờ Tiến trình Nhận kết nối...\n";

    // 2. Mở FIFO để GHI.
    // Lưu ý: Lệnh open() ở đây sẽ dừng (block) cho tới khi có một tiến trình khác mở FIFO để ĐỌC
    int fd = open(fifo_path, O_WRONLY);
    if (fd == -1) {
        std::cerr << "Lỗi: Không thể mở FIFO!\n";
        return 1;
    }

    std::string message = "Chủ nhật đi chơi không?";
    std::cout << "[Writer Process]: Đã kết nối! Đang gửi -> \"" << message << "\"\n";

    // 3. Gửi dữ liệu qua FIFO
    write(fd, message.c_str(), message.length() + 1);

    // 4. Đóng kết nối
    close(fd);
    std::cout << "[Writer Process]: Đã hoàn tất và thoát.\n";

    return 0;
}