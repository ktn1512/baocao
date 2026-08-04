# IPC (Inter-Process Communication)

## Mục tiêu học tập

Hiểu cách các process giao tiếp với nhau trong hệ điều hành Linux/Android, nắm vững các cơ chế IPC, kỹ thuật đồng bộ dữ liệu và kiến trúc Binder IPC trên Android.

---

# 1. Nền tảng cơ bản

## Process là gì?

- Khái niệm Process
- Vòng đời của Process
- PID (Process ID)
- PPID (Parent Process ID)

## Thread vs Process

- Sự khác nhau giữa Thread và Process
- Ưu điểm, nhược điểm của từng mô hình
- Khi nào sử dụng Thread hoặc Process

## Vùng nhớ của Process

```text
+------------------+
| Text Segment     |
+------------------+
| Data Segment     |
+------------------+
| BSS              |
+------------------+
| Heap             |
|        ↑         |
|        ↓         |
| Stack            |
+------------------+
```

### Các thành phần

- Text Segment
- Data Segment
- BSS
- Heap
- Stack

---

# 2. Tại sao cần IPC?

## Mục đích

- Chia sẻ dữ liệu giữa các Process
- Đồng bộ hoạt động giữa các Process
- Giao tiếp Client-Server
- Tăng tính module hóa của hệ thống
- Hỗ trợ hệ điều hành đa nhiệm

---

# 3. Các cơ chế IPC phổ biến

## Pipe

### Đặc điểm

- Anonymous Pipe
- Thường dùng giữa Parent Process và Child Process
- Giao tiếp một chiều (One-way Communication)

```text
Process A ---> Pipe ---> Process B
```

---

## FIFO (Named Pipe)

### Đặc điểm

- Có tên trong File System
- Không yêu cầu quan hệ Parent-Child
- Có thể sử dụng giữa nhiều Process

```text
Process A ---> FIFO ---> Process B
```

---

## Shared Memory

### Đặc điểm

- Hiệu năng cao nhất trong các cơ chế IPC
- Các Process cùng truy cập một vùng nhớ chung
- Cần cơ chế đồng bộ để tránh Race Condition

```text
Process A
     \
      \
   Shared Memory
      /
     /
Process B
```

---

## Message Queue

### Đặc điểm

- Gửi/Nhận theo Message
- Hỗ trợ độ ưu tiên (Priority)
- Không cần chia sẻ bộ nhớ

```text
Process A ---> Message Queue ---> Process B
```

---

## Socket

### Các loại Socket

- Unix Domain Socket
- TCP Socket
- UDP Socket

### Ứng dụng

- Local Communication
- Network Communication

```text
Client <------> Server
```

---

## Signal

### Đặc điểm

- Cơ chế thông báo sự kiện
- Không dùng để truyền dữ liệu lớn

### Signal phổ biến

- SIGINT
- SIGKILL
- SIGTERM

```text
Process A ----Signal----> Process B
```

---

## RPC (Remote Procedure Call)

### Đặc điểm

- Gọi hàm từ xa
- Mô hình Client-Server
- Che giấu chi tiết truyền thông

### Ví dụ

- gRPC
- Binder IPC (Android)

```text
Client ---> RPC ---> Server
```

---

# 4. Đồng bộ khi IPC

Khi nhiều Process hoặc Thread cùng truy cập tài nguyên chung, cần cơ chế đồng bộ để tránh:

- Race Condition
- Deadlock
- Data Corruption

---

## Mutex

### Chức năng

- Lock
- Unlock

```text
Thread A --> Lock
             |
         Shared Data
             |
Thread B --> Wait
```

---

## Semaphore

### Binary Semaphore

- Giá trị 0 hoặc 1
- Tương tự Mutex

### Counting Semaphore

- Quản lý nhiều tài nguyên cùng lúc

```text
Semaphore = N

Acquire()
Release()
```

---

## Condition Variable

### Chức năng

- Wait()
- Signal()
- Broadcast()

```text
Producer ----Signal----> Consumer
```

---

# 5. Binder IPC (Android)

Binder là cơ chế IPC chính được Android sử dụng để giao tiếp giữa các Process.

---

## Kiến trúc tổng quan

```text
+------------+       +---------------+       +------------+
|   Client   | <-->  | Binder Driver | <-->  |  Service   |
+------------+       +---------------+       +------------+
```

---

## Interface

### Vai trò

- Định nghĩa API cho Client và Service
- Thường được mô tả bằng AIDL

---

## Service

### Vai trò

- Cung cấp chức năng cho Client
- Xử lý request

---

## Client

### Vai trò

- Gửi request đến Service
- Nhận kết quả trả về

---

## Proxy

### Vai trò

- Đại diện Service ở phía Client
- Đóng gói request

---

## Stub

### Vai trò

- Đại diện Client ở phía Service
- Giải mã request và gọi hàm thực tế

---

## Parcel

### Vai trò

- Serialization dữ liệu
- Truyền dữ liệu qua Binder

---

## Binder Driver

### Vai trò

- Thành phần trong Kernel
- Trung gian giao tiếp giữa Client và Service

---

## Luồng hoạt động của Binder

```text
Client
   |
   v
Proxy
   |
   v
Parcel
   |
   v
Binder Driver
   |
   v
Stub
   |
   v
Service
```

---

# Thứ tự học khuyến nghị

```text
Process & Memory
        ↓
Tại sao cần IPC
        ↓
Pipe
        ↓
FIFO
        ↓
Message Queue
        ↓
Shared Memory
        ↓
Signal
        ↓
Socket
        ↓
Mutex / Semaphore / Condition Variable
        ↓
RPC
        ↓
Binder IPC
        ↓
AIDL
        ↓
Android Framework
```

# Tài liệu tham khảo

## Linux IPC

- Linux System Programming
- Advanced Programming in the UNIX Environment (APUE)
- Linux Man Pages

## Android Binder

- Android Open Source Project (AOSP)
- Android Binder Driver Source Code
- Android IPC and Binder Framework Documentation