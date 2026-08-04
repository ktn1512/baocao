# Các cơ chế IPC phổ biến (Inter-Process Communication)

## Mục lục

1. [Pipe](#1-pipe)
2. [FIFO (Named Pipe)](#2-fifo-named-pipe)
3. [Message Queue](#3-message-queue)
4. [Shared Memory](#4-shared-memory)
5. [Socket](#5-socket)
6. [Signal](#6-signal)
7. [RPC (Remote Procedure Call)](#7-rpc-remote-procedure-call)

---

# IPC là gì?

**IPC (Inter-Process Communication)** là các cơ chế giúp các process trong hệ điều hành có thể trao đổi dữ liệu và đồng bộ với nhau.

Các cơ chế IPC phổ biến:

- Pipe
- FIFO
- Message Queue
- Shared Memory
- Socket
- Signal
- RPC

---

# 1. Pipe

## Khái niệm

Pipe giống như một **đường ống byte**, dữ liệu chảy theo một hướng:

```
Process A ──────── Pipe ────────> Process B

Process A ghi "Hello"
Process B đọc "Hello"
```

Pipe thường có hai đầu:

```
fd[0] : đọc dữ liệu
fd[1] : ghi dữ liệu
```

---

## Ví dụ Pipe trong Linux

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

        // Process con chỉ đọc
        close(pipe_fd[1]);

        char buffer[100]{};

        read(pipe_fd[0], buffer, sizeof(buffer));

        std::cout<<"Process con nhận: "
                 << buffer << "\n";

        close(pipe_fd[0]);

    }
    else{

        // Process cha chỉ ghi
        close(pipe_fd[0]);

        const char* message =
            "Xin chao process con";


        write(
            pipe_fd[1],
            message,
            strlen(message)+1
        );


        close(pipe_fd[1]);

        wait(nullptr);
    }


    return 0;
}
```

---

## Luồng hoạt động

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

---

## Đặc điểm Pipe

Ưu điểm:

- Đơn giản
- Truyền dữ liệu dạng byte stream
- Tốc độ tốt

Nhược điểm:

- Thường chỉ truyền một chiều
- Không lưu dữ liệu lâu dài

Pipe phù hợp nhất với các process có quan hệ:

```
Parent Process
       |
      fork()
       |
       v
Child Process
```

Vì process con kế thừa file descriptor sau khi `fork()`.

---

# 2. FIFO (Named Pipe)

## Khái niệm

FIFO giống Pipe nhưng có **tên trong filesystem**.

Ví dụ:

```
/tmp/my_fifo
```

Mô hình:

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

---

## Tạo FIFO trên Linux

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

---

## Pipe và FIFO khác nhau

| Pipe | FIFO |
|---|---|
| Không có tên | Có tên trong filesystem |
| Thường dùng cha-con | Process độc lập có thể sử dụng |
| Tồn tại trong kernel | Có entry trong filesystem |

---

# 3. Message Queue

## Khái niệm

Message Queue là hàng đợi các message.

```
Process A

    |
    | Message 1
    | Message 2
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

---

Khác với Pipe:

- Pipe truyền một luồng byte liên tục
- Message Queue chia dữ liệu thành từng message riêng biệt

Ví dụ:

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

---

## Ưu điểm

- Giữ được ranh giới message
- Xử lý theo thứ tự
- Process gửi và nhận không cần chạy cùng lúc
- Có thể hỗ trợ độ ưu tiên

---

## Nhược điểm

- Kích thước message thường bị giới hạn
- Chậm hơn Shared Memory
- Cần đóng gói và sao chép dữ liệu

---

# 4. Shared Memory

## Khái niệm

Shared Memory cho phép nhiều process ánh xạ vào cùng một vùng RAM.

```
Process A Memory              Process B Memory

+-------------+                +-------------+
| Private A   |                | Private B   |
+-------------+                +-------------+
|             |                |             |
| Shared Mem  |----------------| Shared Mem  |
|             |                |             |
+-------------+                +-------------+
```

Hai process thực chất truy cập cùng một vùng nhớ vật lý.

Ví dụ:

```
Process A:

shared_memory = 100


Process B:

đọc shared_memory

=> 100
```

---

## Ưu điểm

Shared Memory thường là IPC nhanh nhất khi truyền dữ liệu lớn.

Ứng dụng:

- Frame camera
- Hình ảnh
- Buffer âm thanh
- Dữ liệu sensor
- Hệ thống realtime

---

## Vấn đề Race Condition

Ví dụ:

Hai process cùng thực hiện:

```
shared_memory++;
```

Thực chất gồm:

```
1. Đọc giá trị
2. Cộng 1
3. Ghi lại
```

Ban đầu:

```
shared_memory = 0
```

Process A:

```
Read 0
```

Process B:

```
Read 0
```

Process A:

```
Write 1
```

Process B:

```
Write 1
```

Kết quả:

```
shared_memory = 1
```

Trong khi mong muốn:

```
shared_memory = 2
```

---

## Giải pháp đồng bộ

Shared Memory thường đi cùng:

- Mutex
- Semaphore
- Atomic
- Condition Variable

Cần nhớ:

```
Shared Memory
        |
        |
        v

Truyền dữ liệu


Mutex/Semaphore
        |
        |
        v

Đồng bộ truy cập dữ liệu
```

---

# 5. Socket

## Khái niệm

Socket cho phép hai process giao tiếp với nhau.

Có hai loại chính:

---

## Unix Domain Socket

Dùng giữa các process trên cùng một máy.

```
Process A

    |
 Unix Socket
    |
Process B
```

---

## Network Socket

Dùng giao tiếp qua mạng.

Ví dụ:

- TCP
- UDP

---

## TCP

Đặc điểm:

- Có kết nối
- Đảm bảo dữ liệu đến
- Đúng thứ tự

Phù hợp:

- Web
- Truyền file
- Điều khiển từ xa

---

## UDP

Đặc điểm:

- Không đảm bảo gói tin đến
- Không đảm bảo thứ tự
- Nhanh hơn TCP

Phù hợp:

- Streaming
- Game realtime
- Dữ liệu gửi liên tục

---

## Điểm mạnh của Socket

Rất linh hoạt:

```
Cùng máy:

Process A
    |
Unix Domain Socket
    |
Process B


Khác máy:

Process A
    |
TCP/UDP Socket
    |
Process B
```

---

# 6. Signal

## Khái niệm

Signal là một thông báo nhỏ được gửi tới process để báo một sự kiện.

Signal không dùng để truyền lượng dữ liệu lớn.

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

## Một số signal phổ biến trong Linux

| Signal | Ý nghĩa |
|-|-|
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
- Không phù hợp để truyền message phức tạp

---

# 7. RPC (Remote Procedure Call)

## Khái niệm

RPC tạo cảm giác gọi hàm ở process khác hoặc máy khác giống như gọi hàm bình thường.

Ví dụ:

Client:

```cpp
int result = calc.add(10,20);
```

Nhưng bên dưới:

```
Client Proxy

      |
      | đóng gói request
      |
      v

IPC / Network

      |
      v

Server Stub

      |
      | giải mã
      |
      v

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
 | serialize request
 |
 v

IPC / Network

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

## RPC sử dụng nền tảng nào?

RPC không phải là cơ chế truyền dữ liệu cấp thấp độc lập.

Nó thường được xây dựng trên:

- Socket
- Pipe
- Message Queue
- Binder
- Network Protocol

---

## Binder

Binder là một hệ thống IPC có mô hình gần giống RPC.

Được sử dụng nhiều trong:

- Android Framework
- Các service trong Android

---

# So sánh nhanh các IPC

| Cơ chế | Tốc độ | Truyền dữ liệu lớn | Qua mạng | Độ phức tạp |
|-|-|-|-|-|
| Pipe | Cao | Không | Không | Thấp |
| FIFO | Cao | Không | Không | Thấp |
| Message Queue | Trung bình | Trung bình | Không | Trung bình |
| Shared Memory | Rất cao | Có | Không | Cao |
| Socket | Trung bình | Có | Có | Trung bình |
| Signal | Rất cao | Không | Không | Thấp |
| RPC | Phụ thuộc | Có | Có | Cao |

---

# Kết luận

Không có cơ chế IPC nào tốt nhất cho mọi trường hợp.

Lựa chọn phụ thuộc vào nhu cầu:

- Cần đơn giản giữa cha-con → **Pipe**
- Process độc lập trên cùng máy → **FIFO**
- Muốn gửi message có cấu trúc → **Message Queue**
- Truyền dữ liệu cực lớn → **Shared Memory**
- Giao tiếp giữa máy khác nhau → **Socket**
- Chỉ cần báo sự kiện → **Signal**
- Muốn gọi service như gọi hàm → **RPC**