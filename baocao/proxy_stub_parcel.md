# Proxy, Stub và Parcel trong Binder IPC

## Tổng quan

Trong Binder IPC:

```text
Proxy  → đại diện cho server ở phía client
Stub   → nhận và giải mã yêu cầu ở phía server
Parcel → gói dữ liệu được truyền qua Binder
```

Luồng cơ bản:

```text
Client → Proxy → Binder Driver → Stub → Service
```

---

## 1. Bài toán ban đầu

Server cung cấp hàm:

```cpp
int add(int a, int b);
```

Client muốn gọi:

```cpp
int result = calculator->add(10, 20);
```

Nếu `calculator` nằm trong cùng process với client, hàm có thể được gọi trực tiếp.

Nếu `calculator` nằm trong process khác, client không thể nhảy trực tiếp đến địa chỉ hàm trong server vì mỗi process có không gian bộ nhớ riêng.

Binder tạo một object trung gian ở phía client gọi là **Proxy**.

---

## 2. Proxy

Proxy là object nằm trong process client nhưng đại diện cho object thật ở process server.

```text
Process Client                  Process Server

CalculatorProxy  ────────────>  CalculatorService
      giả                              thật
```

Ví dụ:

```cpp
int CalculatorProxy::add(int a, int b)
{
    Parcel data;
    Parcel reply;

    data.writeInt32(a);
    data.writeInt32(b);

    remoteBinder->transact(
        TRANSACTION_ADD,
        data,
        &reply
    );

    return reply.readInt32();
}
```

Proxy không tự thực hiện phép cộng. Nó chỉ:

1. Tạo `Parcel`.
2. Ghi tham số vào `Parcel`.
3. Gửi transaction qua Binder.
4. Chờ kết quả.
5. Đọc kết quả từ `reply`.
6. Trả kết quả về client.

Vì vậy:

```cpp
calculator->add(10, 20);
```

có thể hiểu gần giống:

```cpp
Parcel data;
Parcel reply;

data.writeInt32(10);
data.writeInt32(20);

binder.transact(
    TRANSACTION_ADD,
    data,
    &reply
);
```

---

## 3. Parcel

`Parcel` là vùng dữ liệu dùng để đóng gói thông tin truyền qua Binder.

Ví dụ nội dung một Parcel:

```text
Parcel data
+-------------------------+
| Interface token         |
+-------------------------+
| int a = 10              |
+-------------------------+
| int b = 20              |
+-------------------------+
```

Proxy ghi dữ liệu theo thứ tự:

```cpp
data.writeInt32(10);
data.writeInt32(20);
```

Stub phải đọc đúng kiểu và đúng thứ tự:

```cpp
int a = data.readInt32();
int b = data.readInt32();
```

Nếu Proxy ghi theo thứ tự:

```text
int → string → bool
```

thì Stub cũng phải đọc theo đúng thứ tự đó.

---

## 4. Transaction Code

Server có thể cung cấp nhiều hàm:

```cpp
int add(int a, int b);
int subtract(int a, int b);
int multiply(int a, int b);
```

Binder cần biết client muốn gọi hàm nào.

Mỗi hàm được gán một mã transaction:

```cpp
enum {
    TRANSACTION_ADD = 1,
    TRANSACTION_SUBTRACT = 2,
    TRANSACTION_MULTIPLY = 3
};
```

Khi gọi `add()`:

```cpp
remoteBinder->transact(
    TRANSACTION_ADD,
    data,
    &reply
);
```

Khi gọi `subtract()`:

```cpp
remoteBinder->transact(
    TRANSACTION_SUBTRACT,
    data,
    &reply
);
```

Binder driver không cần hiểu hàm `add()` hay `subtract()` làm gì.

Nó chỉ chuyển dữ liệu như:

```text
Transaction code = 1
Parcel data      = [10, 20]
```

sang process server.

---

## 5. Stub

Stub nằm trong process server.

Nó nhận transaction từ Binder driver, kiểm tra transaction code, đọc dữ liệu và gọi hàm tương ứng.

```cpp
status_t CalculatorStub::onTransact(
    uint32_t code,
    const Parcel& data,
    Parcel* reply
)
{
    switch (code) {
        case TRANSACTION_ADD: {
            int a = data.readInt32();
            int b = data.readInt32();

            int result = add(a, b);

            reply->writeInt32(result);
            return OK;
        }

        case TRANSACTION_SUBTRACT: {
            int a = data.readInt32();
            int b = data.readInt32();

            int result = subtract(a, b);

            reply->writeInt32(result);
            return OK;
        }
    }

    return UNKNOWN_TRANSACTION;
}
```

Stub thực hiện các công việc:

1. Nhận transaction.
2. Kiểm tra transaction code.
3. Đọc tham số từ `Parcel`.
4. Gọi hàm thật trong server.
5. Ghi kết quả vào `reply Parcel`.

---

## 6. Luồng gọi hoàn chỉnh

Client gọi:

```cpp
int result = calculator->add(10, 20);
```

Toàn bộ quá trình:

```text
Client
  |
  | calculator->add(10, 20)
  v
Proxy::add()
  |
  | ghi 10 và 20 vào Parcel
  | code = TRANSACTION_ADD
  v
Binder Driver
  |
  | tìm Binder object tương ứng
  | chuyển transaction sang server
  v
Stub::onTransact()
  |
  | kiểm tra TRANSACTION_ADD
  | đọc a = 10
  | đọc b = 20
  v
CalculatorService::add(10, 20)
  |
  | result = 30
  v
Stub
  |
  | ghi 30 vào reply Parcel
  v
Binder Driver
  |
  | chuyển reply về client
  v
Proxy
  |
  | đọc 30 từ reply
  v
Client nhận result = 30
```

---

## Ghi nhớ

```text
Proxy:
Lời gọi hàm → Transaction
```

```text
Parcel:
Chứa tham số và kết quả
```

```text
Stub:
Transaction → Lời gọi hàm thật
```

```text
Binder Driver:
Chuyển transaction giữa các process
```

Mô hình tổng quát:

```text
Client
  ↓
Proxy
  ↓
Binder Driver
  ↓
Stub
  ↓
Service
```
