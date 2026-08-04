#include <sys/socket.h>
#include <sys/un.h> // Dùng thư viện này thay vì inet.h
#include <unistd.h>
#include <iostream>

#define SOCKET_PATH "/tmp/user_info.sock"

struct UserInfo {
    char name[50];
    int age;
    char gender[10];
};

int main() {
    // Xóa file socket cũ nếu có tồn tại
    unlink(SOCKET_PATH);

    // 1. Tạo UNIX Domain Socket (AF_UNIX) - Không cần Port
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // 2. Bind vào FILE PATH trên đĩa thay vì Bind IP/Port
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    std::cout << "[SERVER] Đang chờ Client kết nối qua file: " << SOCKET_PATH << "...\n";

    int new_socket = accept(server_fd, NULL, NULL);

    // ... Nhập dữ liệu và send() tương tự như trước ...
    UserInfo info = {"Nguyen Van A", 20, "Nam"};
    send(new_socket, &info, sizeof(info), 0);

    close(new_socket);
    close(server_fd);
    unlink(SOCKET_PATH); // Dọn dẹp file socket sau khi dùng xong
    return 0;
}