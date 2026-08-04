#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>

#define SOCKET_PATH "/tmp/user_info.sock"

struct UserInfo {
    char name[50];
    int age;
    char gender[10];
};

int main() {
    // 1. Tạo UNIX Domain Socket
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // 2. Connect trực tiếp đến FILE PATH (Không cần IP/Port)
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Không thể kết nối! Server đã bật chưa?\n";
        return -1;
    }

    UserInfo info;
    read(sock, &info, sizeof(info));
    
    std::cout << "Nhận từ Server: " << info.name << " - " << info.age << " tuổi\n";

    close(sock);
    return 0;
}