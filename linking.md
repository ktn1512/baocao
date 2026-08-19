# Static Linking (Liên kết tĩnh)
## 1. Khái niệm

**Static linking** là quá trình linker kết hợp mã máy từ các file object và thư viện tĩnh vào cùng một file thực thi.
Sau khi chương trình được tạo, các phần mã cần thiết của thư viện đã nằm bên trong file thực thi. Vì vậy, chương trình thường không cần tìm các file thư viện tương ứng khi chạy.

Ví dụ, với chương trình gồm `main.o` và `math.o`:
```text
main.o + math.o + thư viện tĩnh (.a) -> program
```

Trong đó:
- `main.o`, `math.o`: mã máy đã được biên dịch nhưng chưa hoàn chỉnh.
- `.a` (Linux/macOS) hoặc `.lib` (Windows): thư viện tĩnh, thường là một tập hợp các file object.
- `program`: file thực thi sau khi liên kết.

## 2. Các bước hoạt động
Quá trình static linking thường gồm các bước chính:

1. **Biên dịch:** Mỗi file mã nguồn được dịch thành file object.
2. **Giải quyết symbol:** Linker tìm phần định nghĩa cho các hàm hoặc biến mà chương trình đang sử dụng.
3. **Chọn mã cần thiết:** Linker lấy các module cần dùng từ thư viện tĩnh.
4. **Gán địa chỉ:** Các đoạn mã và dữ liệu được sắp xếp, đồng thời các tham chiếu được sửa thành địa chỉ cụ thể.
5. **Tạo file thực thi:** Kết quả là một file có thể được nạp để chạy.
Ví dụ, nếu `main.o` gọi hàm `add()` được định nghĩa trong `math.o`, linker sẽ kết hợp hai file này và nối lời gọi `add()` với phần định nghĩa tương ứng.

## 3. Tạo và sử dụng thư viện tĩnh

### Tạo file object

```bash
gcc -c math.c -o math.o
```

### Tạo thư viện tĩnh

Lệnh `ar` đóng gói các file object thành thư viện `libmath.a`:

```bash
ar rcs libmath.a math.o
```

Theo quy ước, thư viện có tên dạng `lib<name>.a`. Vì vậy, `libmath.a` được tham chiếu bằng tùy chọn `-lmath`.
### Liên kết chương trình

```bash
gcc -c main.c -o main.o
gcc main.o -L. -lmath -o program
```

Ý nghĩa các tùy chọn:

- `-L.`: thêm thư mục hiện tại vào nơi tìm thư viện.
- `-lmath`: liên kết với `libmath.a`.
- `-o program`: đặt tên file thực thi là `program`.

Có thể yêu cầu liên kết tĩnh toàn bộ thư viện bằng:

```bash
gcc -static main.o -L. -lmath -o program
```

Tuy nhiên, tùy chọn `-static` cần các bản static library tương ứng của hệ thống. Nếu chúng chưa được cài đặt, linker sẽ báo lỗi. Trong thực tế, lệnh `gcc main.o -L. -lmath -o program` vẫn liên kết thư viện tự tạo `libmath.a` theo dạng tĩnh, còn các thư viện hệ thống khác có thể vẫn là thư viện động.
## 4. Ví dụ đầy đủ

`math.c`:

```c
int add(int first, int second) {
    return first + second;
}
```

`main.c`:

```c
#include <stdio.h>

int add(int first, int second);

int main(void) {
    printf("%d\n", add(2, 3));
    return 0;
}
```

Biên dịch và chạy:

```bash
gcc -c math.c -o math.o
ar rcs libmath.a math.o
gcc -c main.c -o main.o
gcc main.o -L. -lmath -o program
./program
```

Kết quả:

```text
5
```

## 5. Ưu điểm

- File thực thi ít phụ thuộc vào việc cài đặt thư viện bên ngoài khi chạy.
- Dễ triển khai trong môi trường không có sẵn đúng phiên bản thư viện.
- Có thể giúp chương trình ổn định hơn vì mã thư viện đã được đóng gói tại thời điểm build.
- Linker thường chỉ lấy các module cần thiết trong thư viện tĩnh.

## 6. Nhược điểm

- File thực thi thường lớn hơn vì chứa bản sao mã thư viện.
- Khi thư viện được cập nhật, chương trình thường phải liên kết và build lại.
- Nhiều chương trình cùng dùng một thư viện sẽ có thể chứa nhiều bản sao của cùng mã máy, làm tăng dung lượng bộ nhớ.
- Có thể gặp vấn đề tương thích giấy phép hoặc phụ thuộc hệ thống khi đóng gói thư viện tĩnh.

## 7. So sánh với dynamic linking

| Đặc điểm | Static linking | Dynamic linking |
| --- | --- | --- |
| Thời điểm liên kết | Khi build chương trình | Khi chương trình được nạp hoặc đang chạy |
| Thư viện | `.a` hoặc `.lib` | `.so`, `.dll`, `.dylib` |
| File thực thi | Thường lớn hơn | Thường nhỏ hơn |
| Phụ thuộc lúc chạy | Ít hơn | Cần có thư viện động phù hợp |
| Cập nhật thư viện | Thường phải build lại | Có thể cập nhật riêng thư viện |
| Dùng chung bộ nhớ | Khó tối ưu hơn giữa nhiều tiến trình | Có thể dùng chung thư viện trong bộ nhớ |

Tóm lại, static linking đóng gói mã thư viện trực tiếp vào file thực thi, giúp triển khai độc lập hơn nhưng làm tăng kích thước và khiến việc cập nhật thư viện kém linh hoạt hơn.
# Dynamic linking (Liên kết động)

## 1. Khái niệm

**Dynamic linking** là quá trình liên kết chương trình với thư viện động khi chương trình được nạp hoặc bắt đầu chạy, thay vì đưa mã thư viện trực tiếp vào file thực thi lúc build.

Thư viện động được lưu ở file riêng, thường có dạng `.so` trên Linux, `.dll` trên Windows hoặc `.dylib` trên macOS.

**Dynamic linker** là thành phần có nhiệm vụ tìm thư viện động, nạp thư viện vào bộ nhớ và nối các lời gọi hàm với phần định nghĩa tương ứng.

```text
Executable + thư viện động (.so/.dll/.dylib)
                    ↓ dynamic linker khi chạy
                 Chương trình hoạt động
```

## 2. Các bước hoạt động

Quá trình dynamic linking thường gồm các bước chính:

1. **Biên dịch:** Mã nguồn được dịch thành file object.
2. **Liên kết ban đầu:** Linker ghi lại tên các thư viện và symbol mà chương trình cần, nhưng không chép toàn bộ mã thư viện vào executable.
3. **Nạp chương trình:** Khi chạy, hệ điều hành gọi dynamic linker.
4. **Tìm và nạp thư viện:** Dynamic linker tìm các file thư viện cần thiết và đưa chúng vào bộ nhớ.
5. **Giải quyết symbol:** Các lời gọi hàm trong chương trình được nối với địa chỉ của hàm trong thư viện.
6. **Thực thi:** Sau khi liên kết hoàn tất, chương trình bắt đầu chạy.

Nếu không tìm thấy thư viện hoặc thư viện không tương thích, chương trình có thể không khởi động được.

## 3. Tạo và sử dụng thư viện động

### Tạo thư viện động

Trên Linux, tùy chọn `-fPIC` tạo mã có thể được nạp ở nhiều địa chỉ bộ nhớ, còn `-shared` tạo file thư viện động:

```bash
gcc -fPIC -c math.c -o math.o
gcc -shared math.o -o libmath.so
```

Theo quy ước, thư viện có tên dạng `lib<name>.so`. Vì vậy, `libmath.so` được tham chiếu bằng tùy chọn `-lmath`.

### Liên kết chương trình

```bash
gcc -c main.c -o main.o
gcc main.o -L. -lmath -Wl,-rpath,'$ORIGIN' -o program
```

Ý nghĩa các tùy chọn:

- `-L.`: thêm thư mục hiện tại vào nơi tìm thư viện khi build.
- `-lmath`: liên kết với `libmath.so`.
- `-Wl,-rpath,'$ORIGIN'`: giúp chương trình tìm thư viện trong cùng thư mục với executable khi chạy.
- `-o program`: đặt tên file thực thi là `program`.

## 4. Ví dụ đầy đủ

`math.c`:

```c
int add(int first, int second) {
    return first + second;
}
```

`main.c`:

```c
#include <stdio.h>

int add(int first, int second);

int main(void) {
    printf("%d\n", add(2, 3));
    return 0;
}
```

Biên dịch và chạy:

```bash
gcc -fPIC -c math.c -o math.o
gcc -shared math.o -o libmath.so
gcc -c main.c -o main.o
gcc main.o -L. -lmath -Wl,-rpath,'$ORIGIN' -o program
./program
```

Kết quả:

```text
5
```

Trong ví dụ này, mã của `add()` vẫn nằm trong `libmath.so`. File `program` chỉ lưu thông tin để dynamic linker tìm và sử dụng hàm đó khi chạy.

## 5. Ưu điểm

- File thực thi thường nhỏ hơn vì không chứa toàn bộ mã thư viện.
- Nhiều chương trình có thể dùng chung một bản thư viện trong bộ nhớ.
- Có thể cập nhật thư viện mà không cần build lại mọi chương trình sử dụng nó.
- Tiết kiệm dung lượng khi nhiều chương trình cùng dùng một thư viện.

## 6. Nhược điểm

- Chương trình phụ thuộc vào việc thư viện có sẵn trên máy chạy.
- Có thể xảy ra lỗi thiếu thư viện hoặc không tương thích phiên bản.
- Việc khởi động cần thêm bước nạp thư viện và giải quyết symbol.
- Phân phối chương trình phức tạp hơn vì phải đi kèm hoặc cài đặt đúng các thư viện động.

## 7. So sánh với static linking

| Đặc điểm | Static linking | Dynamic linking |
| --- | --- | --- |
| Thời điểm liên kết | Khi build chương trình | Khi chương trình được nạp hoặc chạy |
| Thư viện | Mã nằm trong executable | File riêng `.so`, `.dll`, `.dylib` |
| File thực thi | Thường lớn hơn | Thường nhỏ hơn |
| Phụ thuộc lúc chạy | Ít hơn | Cần thư viện phù hợp |
| Cập nhật thư viện | Thường phải build lại | Có thể cập nhật riêng |
| Dùng chung bộ nhớ | Khó tối ưu hơn | Có thể dùng chung thư viện |

Tóm lại, static linking ưu tiên tính độc lập khi triển khai, còn dynamic linking ưu tiên khả năng dùng chung và cập nhật thư viện linh hoạt.
