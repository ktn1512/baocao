# Các cơ chế IPC phổ biến

## Mục lục

- [1. Pipe](#1-pipe)
- [2. FIFO - Named Pipe](#2-fifo---named-pipe)
- [3. Message Queue](#3-message-queue)
- [4. Shared Memory](#4-shared-memory)
- [5. Socket](#5-socket)
- [6. Signal](#6-signal)
- [7. RPC - Remote Procedure Call](#7-rpc---remote-procedure-call)

---

# 1. Pipe

Pipe giống như một đường ống byte.

```
Process A ──────── Pipe ────────> Process B

              dữ liệu chảy một chiều
```

Process A ghi:

```
Hello
```

Process B đọc:

```
Hello
```

Pipe thường có 2 đầu:

```
fd[0]: đọc
fd[1]: ghi
```

## Ví dụ

```cpp
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

int main(){

    int pipe_fd[2];

    if(pipe(pipe_fd)==-1){
        std::cerr<<"Không tạo được pipe\n";
        return 1;
    }


    pid_t pid = fork();


    if(pid < 0){
        std::cerr<<"Không tạo được process\n";
        return 1;
    }


    if(pid == 0){

        close(pipe_fd[1]);

        char buffer[100]{};

        read(pipe_fd[0], buffer, sizeof(buffer));

        std::cout<<"Process con nhan "
                 << buffer << "\n";

        close(pipe_fd[0]);

    }
    else{

        close(pipe_fd[0]);

        const char* message =
            "Xin chao process con";

        write(
            pipe_fd[1],
            message,
            std::strlen(message)+1
        );

        close(pipe_fd[1]);

        wait(nullptr);
    }

    return 0;
}
```

## Luồng

```
Process cha

   |
   | write("Xin chao")
   |
   v

 Pipe

   |
   | read()
   |
   v

Process con
```

Pipe thông thường phù hợp nhất với các process có quan hệ cha-con, vì process con kế thừa file descriptor sau `fork()`.

## Đặc điểm Pipe

- Đơn giản
- Truyền theo luồng byte
- Thường một chiều
- Chủ yếu dùng giữa process có quan hệ cha-con
- Dữ liệu không tồn tại lâu dài

---

# 2. FIFO - Named Pipe

FIFO gần giống pipe nhưng có tên trong filesystem.

Ví dụ:

```
/tmp/my_fifo
```

Luồng:

```
Process A

   |
   | ghi vào /tmp/my_fifo
   |
   v

FIFO

   |
   | đọc từ /tmp/my_fifo
   |
   v

Process B
```

## Ví dụ tạo FIFO trên Linux

Tạo FIFO:

```bash
mkfifo /tmp/my_fifo
```

Terminal 1:

```bash
echo "hello" > /tmp/my_fifo
```

Terminal 2:

```bash
cat /tmp/my_fifo
```

## Pipe và FIFO khác nhau

### Pipe

- Không có tên
- Thường dùng cha-con

### FIFO

- Có tên trong filesystem
- Process độc lập vẫn mở được

---

# 3. Message Queue - Hàng đợi thông điệp

```
Process A

   |
   | gửi Message 1
   | gửi Message 2
   |
   v

+----------------+
| Message 1      |
| Message 2      |
| Message 3      |
+----------------+

   |
   v

Process B
```

Điểm khác Pipe:

- Pipe là một luồng liên tục
- Message chia thành từng message riêng

Ví dụ gửi:

```
Message 1:

{
 command = ADD,
 a = 10,
 b = 20
}


Message 2:

{
 command = SUB,
 a = 30,
 b = 5
}
```

## Ưu điểm

- Giữ ranh giới giữa các message
- Có thể xử lí theo thứ tự
- Người gửi và nhận không nhất thiết chạy cùng 1 lúc
- Có thể hỗ trợ mức độ ưu tiên

## Nhược điểm

- Kích thước message thường bị giới hạn
- Chậm hơn shared memory
- Phải đóng gói và sao chép dữ liệu

---

# 4. Shared Memory

Shared Memory cho phép nhiều process ánh xạ vào một vùng RAM.

```
Process A memory             Process B memory

┌───────────────┐            ┌───────────────┐
│ Vùng riêng A  │            │ Vùng riêng B  │
├───────────────┤            ├───────────────┤
│ Shared Memory │────────────│ Shared Memory │
└───────────────┘            └───────────────┘
```

Cả 2 process thực chất cùng truy cập vào cùng một vùng vật lí.

Ví dụ:

```
Process A ghi:

value = 100


        ↓


Process B đọc được:

value = 100
```

Shared Memory thường là cơ chế IPC nhanh nhất đối với lượng dữ liệu lớn vì không cần liên tục copy toàn bộ dữ liệu qua kernel.

## Ví dụ

- Truyền frame camera
- Truyền hình ảnh
- Buffer âm thanh
- Dữ liệu cảm biến lớn
- Dữ liệu thời gian thực

---

## Vấn đề của Shared Memory

Giả sử cả 2 process đều thực hiện:

```
shared_memory++;
```

Lệnh gồm:

1. Đọc shared memory
2. Cộng 1
3. Ghi lại

Nếu cả hai cùng đọc giá trị 0:

```
Process A đọc 0

Process B đọc 0
```

Process A ghi:

```
1
```

Process B ghi:

```
1
```

Nếu xảy ra tuần tự thì kết quả sẽ là:

```
2
```

Nhưng nếu xảy ra đồng thời thì kết quả là:

```
1
```

Vì vậy Shared Memory phải đi cùng với:

- Mutex
- Semaphore
- Atomic
- Condition Variable

Shared Memory dùng để truyền dữ liệu.

Mutex và Semaphore dùng để đồng bộ việc truy cập dữ liệu đó.

---

# 5. Socket

Socket cho phép 2 process giao tiếp với nhau.

Có 2 loại quan trọng:

## Unix Domain Socket

Dùng giữa các process trên cùng một máy.

## Network Socket

Dùng qua mạng.

---

## TCP

Đặc điểm:

- Có kết nối
- Đảm bảo dữ liệu đến
- Đúng thứ tự

Phù hợp:

- Gửi file
- Web
- Lệnh điều khiển cần tin cậy

---

## UDP

Đặc điểm:

- Không đảm bảo gói đến
- Không đảm bảo thứ tự
- Nhanh và đơn giản hơn

Phù hợp:

- Streaming
- Dữ liệu gửi liên tục

---

Socket rất linh hoạt:

```
Cùng máy:

Unix Domain Socket


Khác máy:

TCP/UDP Socket
```

---

# 6. Signal

Signal là một thông báo nhỏ gửi đến process để báo một sự kiện.

Signal không dùng để truyền dữ liệu lớn.

Ví dụ:

```
Process A

    |
    | SIGTERM
    |
    v

Process B
```

---

## Một số Signal phổ biến trên Linux

| Signal | Ý nghĩa |
|---|---|
| SIGINT | Ngắt process (Ctrl + C) |
| SIGTERM | Yêu cầu process kết thúc |
| SIGKILL | Kill process ngay lập tức |
| SIGSTOP | Tạm dừng process |
| SIGCONT | Tiếp tục process |

---

## Đặc điểm Signal

Ưu điểm:

- Rất nhẹ
- Gửi nhanh
- Phù hợp thông báo sự kiện

Nhược điểm:

- Không truyền được nhiều dữ liệu
- Không phù hợp truyền message phức tạp

---

# 7. RPC - Remote Procedure Call

RPC tạo cảm giác gọi hàm ở process khác hoặc máy khác như gọi hàm bình thường.

Client viết:

```cpp
int result = calc.add();
```

Nhưng bên dưới:

```
Client Proxy

    ↓ đóng gói dữ liệu

IPC hoặc Network

    ↓

Server Stub

    ↓ giải mã

CalculatorService::add()
```

---

## Luồng RPC

```
Client

  |
  | add(10,20)
  |
  v

Proxy

  |
  | đóng gói request
  |
  v

IPC

  |
  v

Stub

  |
  | gọi hàm thật
  |
  v

Service
```

---

RPC không nhất thiết là một cơ chế truyền dữ liệu cấp thấp độc lập.

Nó thường được xây dựng trên:

- Socket
- Pipe
- Message Queue
- Binder
- Một giao thức mạng khác

---

## Binder

Binder là một hệ thống IPC có mô hình gần với RPC.

Binder được sử dụng nhiều trong:

- Android Framework
- Android Service

```
```
