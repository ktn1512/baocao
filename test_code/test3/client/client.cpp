#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

// Cấu trúc dữ liệu phải khớp hoàn toàn với Server
struct UserInfo {
    char name[50];
    int age;
    char gender[10];
};

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    // 1. Tạo Socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "[CLIENT] Lỗi tạo Socket!\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Định vị Server là Localhost (127.0.0.1)
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "[CLIENT] Địa chỉ IP không hợp lệ!\n";
        return -1;
    }

    // 2. Kết nối tới Server
    std::cout << "[CLIENT] Đang kết nối tới Server...\n";
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "[CLIENT] Kết nối thất bại! Hãy chắc chắn Server đã khởi chạy.\n";
        return -1;
    }

    std::cout << "[CLIENT] Đã kết nối! Đang chờ Server gửi dữ liệu...\n";

    // 3. Nhận dữ liệu từ Server đổ vào biến info
    UserInfo info;
    int bytes_read = read(sock, &info, sizeof(info));

    if (bytes_read > 0) {
        // 4. Hiển thị thông tin ra màn hình
        std::cout << "\n====================================\n";
        std::cout << "   THÔNG TIN NHẬN TỪ SERVER (IPC)   \n";
        std::cout << "====================================\n";
        std::cout << "Họ và tên : " << info.name << "\n";
        std::cout << "Tuổi      : " << info.age << "\n";
        std::cout << "Giới tính : " << info.gender << "\n";
        std::cout << "====================================\n";
    } else {
        std::cout << "[CLIENT] Không nhận được dữ liệu từ Server.\n";
    }

    // 5. Đóng Socket
    close(sock);

    return 0;
}