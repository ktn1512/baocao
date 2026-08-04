# Tại sao cần có IPC (Inter-Process Communication)?

## 1. Bài toán cơ bản

Giả sử có hai chương trình đang chạy:

- **Process Client**
- **Process Server**

Trong **Server** có hàm:

```cpp
int add(int a, int b) {
    return a + b;
}
```

Ta có thể gọi trực tiếp:

```cpp
add(10, 20);
```

nếu hàm này nằm trong cùng một process.

Tuy nhiên, **Client không thể gọi trực tiếp hàm `add()` của Server**, vì:

- Code của Server nằm trong không gian địa chỉ của Server.
- Stack của Client và Server hoàn toàn độc lập.
- Client không thể truyền tham số thông qua stack của Server.
- Địa chỉ của hàm `add()` trong Server không có ý nghĩa đối với Client.

Nói cách khác, hai process không thể truy cập trực tiếp bộ nhớ của nhau.

Do đó, Client phải gửi **yêu cầu (request)** đến Server, Server xử lý rồi gửi **kết quả (response)** trở lại.

```text
Client
   │
   │ Request: add(10,20)
   ▼
Server
   │
   │ Thực hiện add()
   ▼
Result = 30
   │
   ▼
Client
```

---

# 2. IPC hoạt động như thế nào?

IPC **không gọi trực tiếp hàm** của process khác.

Ví dụ, thay vì:

```cpp
calc->add(10, 20);
```

thực tế sẽ diễn ra các bước sau:

1. Client đóng gói yêu cầu `ADD`.
2. Đóng gói tham số `10`.
3. Đóng gói tham số `20`.
4. Gửi dữ liệu sang Server.
5. Server nhận và giải mã yêu cầu.
6. Server gọi hàm `add()`.
7. Đóng gói kết quả `30`.
8. Gửi kết quả về Client.

Minh họa:

```text
Client
│
├── Command : ADD
├── Param 1 : 10
├── Param 2 : 20
│
└──────────────► Server
                 │
                 ▼
             add(10,20)
                 │
                 ▼
            Result = 30
                 │
◄────────────────┘
```

Vì vậy, IPC chỉ truyền:

- Byte
- Dữ liệu
- Thông điệp (Message)

**IPC không truyền lời gọi hàm hay con trỏ hàm giữa các process.**

---

# 3. Những trường hợp cần IPC

## Client - Server

Ví dụ:

- Trình duyệt (Client)
- Web Server

Client gửi yêu cầu:

```text
GET /index.html
```

Server xử lý và trả về nội dung trang web.

---

## Ứng dụng Android

Trong Android:

- Mỗi ứng dụng thường chạy trong một process riêng.
- Các ứng dụng giao tiếp với nhau thông qua IPC.

Ví dụ:

- Camera
- Danh bạ
- Tin nhắn
- Google Play Services

Đều sử dụng IPC để trao đổi dữ liệu.

---

## Chia nhỏ một ứng dụng

Một ứng dụng lớn có thể được chia thành nhiều process:

```text
Application
├── UI Process
├── Database Process
├── Network Process
└── Worker Process
```

Các process này cần IPC để trao đổi thông tin với nhau.

---

## Bảo mật

Việc tách thành nhiều process giúp:

- Cô lập lỗi.
- Hạn chế quyền truy cập.
- Nếu một process bị lỗi hoặc bị tấn công, các process khác vẫn hoạt động.

Ví dụ:

```text
Browser
├── UI Process
├── GPU Process
├── Network Process
└── Renderer Process
```

Mỗi tab trình duyệt có thể chạy trong process riêng để tăng tính an toàn.

---

# 4. IPC giải quyết những vấn đề gì?

IPC cung cấp cơ chế để các process có thể giao tiếp với nhau một cách an toàn và hiệu quả.

## Truyền dữ liệu

Cho phép gửi và nhận dữ liệu giữa các process.

Ví dụ:

- Chuỗi ký tự
- Số nguyên
- Cấu trúc dữ liệu
- File
- Dữ liệu nhị phân

---

## Xác định người gửi và người nhận

IPC xác định:

- Process nào gửi dữ liệu.
- Process nào nhận dữ liệu.

Điều này giúp dữ liệu đến đúng nơi cần xử lý.

---

## Đồng bộ (Synchronization)

Đảm bảo các process phối hợp đúng thứ tự.

Ví dụ:

- Client phải chờ Server xử lý xong trước khi nhận kết quả.
- Tránh hai process cùng ghi vào một tài nguyên tại cùng thời điểm.

---

## Bảo vệ và phân quyền

Không phải process nào cũng được phép giao tiếp với mọi process.

Hệ điều hành có thể kiểm soát:

- Quyền truy cập.
- Danh tính process.
- Mức độ cho phép chia sẻ dữ liệu.

Điều này giúp tăng tính bảo mật và ổn định của hệ thống.

---

# Tóm tắt

| Vấn đề | IPC giải quyết |
|---------|----------------|
| Hai process không thể gọi trực tiếp hàm của nhau | Gửi yêu cầu và nhận phản hồi thông qua dữ liệu hoặc thông điệp |
| Truyền dữ liệu | Gửi byte, message hoặc vùng nhớ dùng chung |
| Đồng bộ | Điều phối thứ tự thực thi giữa các process |
| Xác định bên gửi/nhận | Đảm bảo dữ liệu đến đúng process |
| Bảo mật | Kiểm soát quyền truy cập và chia sẻ dữ liệu giữa các process |

> **Kết luận:** Mỗi process có không gian địa chỉ riêng nên không thể truy cập trực tiếp bộ nhớ hay gọi hàm của process khác. IPC là tập hợp các cơ chế do hệ điều hành cung cấp để các process có thể trao đổi dữ liệu, đồng bộ hoạt động và phối hợp thực hiện công việc một cách an toàn.