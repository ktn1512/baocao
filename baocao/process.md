
# Process và Vùng Nhớ của Process

## 1. Program và Process

### Program

**Program** là file chương trình được lưu trên ổ đĩa, ví dụ:

```text
calc.exe
```

Nó chỉ là tập hợp các mã lệnh, dữ liệu và tài nguyên của chương trình, **chưa được thực thi**.

### Process

Khi hệ điều hành nạp một program vào RAM và CPU bắt đầu thực thi, hệ điều hành sẽ tạo ra một **process**.

> **Process = Program đang chạy**

Một chương trình có thể tạo ra nhiều process.

Ví dụ:

- Mở nhiều cửa sổ Notepad.
- Mở nhiều tiến trình của Chrome.

Mỗi process có:

- PID (Process ID)
- Không gian địa chỉ (Address Space) riêng
- Biến riêng
- Heap riêng
- Stack riêng
- Các tài nguyên riêng như:
  - File
  - Socket
  - Pipe
- Có ít nhất **1 thread**

---

# 2. Vùng nhớ của một Process

Một process thường có cấu trúc bộ nhớ như sau:

```text
Địa chỉ cao
┌───────────────────────────┐
│ Stack                     │
│ Biến cục bộ, lời gọi hàm  │
│            ↓              │
├───────────────────────────┤
│                           │
│ Vùng trống                │
│                           │
├───────────────────────────┤
│            ↑              │
│ Heap                      │
│ malloc, new               │
├───────────────────────────┤
│ BSS                       │
│ Biến toàn cục chưa gán    │
├───────────────────────────┤
│ Data                      │
│ Biến toàn cục đã gán      │
├───────────────────────────┤
│ Read-only Data            │
│ Chuỗi, hằng số            │
├───────────────────────────┤
│ Text / Code               │
│ Mã lệnh chương trình      │
└───────────────────────────┘
Địa chỉ thấp
```

## Text / Code

Chứa mã máy (machine code) của chương trình.

Ví dụ:

```cpp
int main() {
    printf("Hello");
}
```

Phần mã lệnh sau khi biên dịch sẽ nằm trong vùng **Text**.

---

## Read-only Data

Chứa:

- Chuỗi hằng
- Hằng số
- Dữ liệu chỉ đọc

Ví dụ:

```cpp
char *s = "Hello";
```

Chuỗi `"Hello"` sẽ nằm trong vùng **Read-only Data**.

---

## Data

Chứa:

- Biến toàn cục đã được khởi tạo
- Biến `static` đã được khởi tạo

Ví dụ:

```cpp
int a = 10;
static int b = 20;
```

---

## BSS

Chứa:

- Biến toàn cục chưa được khởi tạo
- Biến `static` chưa được khởi tạo rõ ràng

Ví dụ:

```cpp
int a;
static int b;
```

Thực tế hệ điều hành sẽ tự gán giá trị ban đầu là **0**.

---

## Heap

Dùng cho cấp phát bộ nhớ động.

Ví dụ:

```cpp
int *p = malloc(sizeof(int));
```

hoặc

```cpp
int *p = new int;
```

Đặc điểm:

- Cấp phát trong lúc chạy
- Lập trình viên phải tự giải phóng (`free`, `delete`)
- Heap phát triển từ địa chỉ thấp lên cao

---

## Stack

Chứa:

- Biến cục bộ
- Tham số hàm
- Địa chỉ trả về
- Thông tin của mỗi lần gọi hàm (Stack Frame)

Ví dụ:

```cpp
void foo() {
    int x = 10;
}
```

Biến `x` nằm trên **Stack**.

Stack:

- Tự cấp phát
- Tự thu hồi khi hàm kết thúc
- Phát triển từ địa chỉ cao xuống thấp

---

# 3. Mỗi Process có một không gian địa chỉ riêng

Giả sử có hai process:

```cpp
// Process A
int value = 10;

// Process B
int value = 20;
```

Mặc dù tên biến đều là `value`, nhưng:

- Hoàn toàn độc lập
- Không ảnh hưởng lẫn nhau
- Mỗi process có vùng nhớ riêng

---

# 4. Địa chỉ ảo (Virtual Address)

Địa chỉ mà chương trình nhìn thấy là **địa chỉ ảo (Virtual Address)**, không phải địa chỉ vật lý trong RAM.

Ví dụ:

```text
Process A
value ở 0x1000

Process B
value cũng ở 0x1000
```

Mặc dù cùng địa chỉ ảo:

```text
0x1000
```

nhưng hệ điều hành sẽ ánh xạ tới hai địa chỉ vật lý khác nhau trong RAM.

Ví dụ:

```text
Virtual Address 0x1000
        │
        ▼
Process A  ─────► Physical Address 0x3A000

Virtual Address 0x1000
        │
        ▼
Process B  ─────► Physical Address 0x7F000
```

Điều này giúp:

- Cô lập các process
- Tăng tính bảo mật
- Tránh process ghi đè bộ nhớ của nhau

---

# 5. Process và Thread

Một process có thể chứa nhiều thread.

```text
Process
├── Thread 1
├── Thread 2
└── Thread 3
```

## Các thread trong cùng một process dùng chung

- Code (Text)
- Biến toàn cục
- Heap
- File descriptor
- Socket
- Các đối tượng trong process

### Minh họa

```text
Process
┌─────────────────────────────┐
│ Code dùng chung             │
│ Heap dùng chung             │
│ Biến toàn cục dùng chung    │
│                             │
│ Stack Thread 1              │
│ Stack Thread 2              │
│ Stack Thread 3              │
└─────────────────────────────┘
```

---

## Mỗi Thread có Stack riêng

Mặc dù dùng chung Heap và biến toàn cục, nhưng mỗi thread đều có:

- Stack riêng
- Biến cục bộ riêng
- Lời gọi hàm riêng

Điều này giúp các thread thực thi độc lập.

---

# 6. Hai Process không dùng chung vùng nhớ

Mặc định, hai process không chia sẻ bộ nhớ với nhau.

```text
Process A                     Process B
┌──────────────────┐          ┌──────────────────┐
│ Code A           │          │ Code B           │
│ Heap A           │          │ Heap B           │
│ Stack A          │          │ Stack B          │
│ Global A         │          │ Global B         │
└──────────────────┘          └──────────────────┘
```

Nếu muốn chia sẻ dữ liệu giữa các process, cần sử dụng cơ chế **Inter-Process Communication (IPC)** như:

- Shared Memory
- Pipe
- Message Queue
- Socket
- Semaphore

---

# Tóm tắt

| Thành phần | Chức năng |
|------------|-----------|
| **Program** | File chương trình trên ổ đĩa |
| **Process** | Chương trình đang thực thi |
| **Text** | Mã lệnh của chương trình |
| **Read-only Data** | Chuỗi và hằng số |
| **Data** | Biến toàn cục/static đã khởi tạo |
| **BSS** | Biến toàn cục/static chưa khởi tạo |
| **Heap** | Bộ nhớ cấp phát động (`malloc`, `new`) |
| **Stack** | Biến cục bộ, tham số hàm, stack frame |
| **Thread** | Luồng thực thi trong process |
| **Virtual Address** | Địa chỉ ảo được ánh xạ sang địa chỉ vật lý |
| **IPC** | Cơ chế giao tiếp giữa các process |
````
