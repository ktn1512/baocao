# 1. Tại sao cần Binder IPC?

Giả sử trên Android có hai process:

- **Process A:** Ứng dụng
- **Process B:** System Service

Process A muốn gọi một hàm nằm trong Process B. Tuy nhiên, mỗi process có một không gian địa chỉ riêng. Một con trỏ trong Process A không thể trực tiếp trỏ đến dữ liệu hoặc hàm trong Process B.

Vì vậy, Process A phải thực hiện các bước sau:

1. Đóng gói yêu cầu.
2. Gửi yêu cầu sang Process B.
3. Process B giải mã yêu cầu.
4. Gọi hàm thật.
5. Đóng gói kết quả.
6. Trả kết quả về Process A.

---

# 2. Binder làm gì?

Binder biến quá trình phức tạp trên thành một câu lệnh gọi trông giống như một lời gọi hàm bình thường.

Ví dụ:

```cpp
int result = calculator->add(10, 20);
```

Tuy nhiên, bên dưới lời gọi này, quá trình thực tế diễn ra như sau:

```text
Client Process
     |
     | add(10, 20)
     v
Binder Proxy
     |
     | Đóng gói dữ liệu vào Parcel
     v
Binder Driver
     |
     | Chuyển transaction
     v
Binder Stub trong Server
     |
     | Giải mã Parcel
     v
Hàm add() thật
```

Kết quả được truyền ngược lại:

```text
Hàm add() thật
     |
     | 30
     v
Stub
     |
     | Ghi kết quả vào Parcel reply
     v
Binder Driver
     |
     v
Proxy
     |
     v
Client nhận kết quả 30
```

---

# 3. Bốn thành phần quan trọng

## 3.1. Client

Client là process muốn sử dụng dịch vụ.

Ví dụ, một ứng dụng muốn sử dụng dịch vụ hệ thống trên Android.

## 3.2. Server

Server là process cung cấp dịch vụ.

Server chứa phần cài đặt thật của các hàm mà Client muốn gọi.

## 3.3. Binder Driver

Binder Driver là thành phần nằm trong kernel, chịu trách nhiệm:

- Chuyển yêu cầu giữa các process.
- Quản lý Binder object.
- Quản lý Binder handle.
- Đánh thức Binder thread.
- Truyền thông tin UID/PID của bên gọi.
- Quản lý buffer của transaction.

Có thể hình dung Binder Driver là thành phần trung gian vận chuyển dữ liệu:

```text
Client <---- Binder Driver ----> Server
```

## 3.4. Service Manager

Service Manager giống như một danh bạ dịch vụ.

- Server đăng ký dịch vụ với Service Manager.
- Client tìm kiếm dịch vụ thông qua tên.
- Service Manager trả về một Binder handle đại diện cho dịch vụ đó.

Ví dụ:

```text
Server đăng ký:
"calculator" -> CalculatorService

Client tìm kiếm:
getService("calculator")
```

---

# 4. IPC và RPC

## 4.1. IPC

**IPC** là viết tắt của **Inter-Process Communication**.

IPC nói chung về việc hai process trao đổi dữ liệu với nhau.

Một số cơ chế IPC phổ biến:

- Pipe
- Socket
- Shared Memory
- Message Queue
- Binder

## 4.2. RPC

**RPC** là viết tắt của **Remote Procedure Call**.

RPC là cách làm cho việc gọi hàm nằm trong một process khác trông giống như gọi một hàm cục bộ.

Ví dụ:

```cpp
int result = calculator->add(10, 20);
```

Trong ví dụ trên, hàm `add()` thực tế có thể nằm trong một process khác, nhưng đối với Client, nó trông giống như một lời gọi hàm thông thường.

---

# 5. Kết luận

Binder vừa là một cơ chế **IPC**, vừa hỗ trợ mô hình **RPC**.

- **Binder IPC:** Truyền dữ liệu giữa các process.
- **Binder RPC:** Gọi phương thức trên một object nằm trong process khác.

Mô hình tổng quát:

```text
Client
  |
  | Gọi hàm
  v
Proxy
  |
  | Ghi dữ liệu vào Parcel
  v
Binder Driver
  |
  | Chuyển transaction
  v
Stub
  |
  | Đọc dữ liệu từ Parcel
  v
Server Implementation
```
