# Quá trình biên dịch code C/C++

GCC (GNU Compiler Collection) là công cụ phổ biến để biên dịch mã C/C++.

Quá trình biên dịch một file `.c` thành file thực thi thường gồm 4 giai đoạn: **Preprocessing, Compilation, Assembly, Linking**.

## 1. Preprocessing (Tiền xử lý)

- **Mô tả:** Xử lý các chỉ thị như `#include`, `#define`, `#ifdef`,...
- **Output:** File `.i`
- **Lệnh:**

```bash
gcc -E source.c -o source.i
head -20 source.i

# C++
g++ -E source.cpp -o source.ii
```

## 2. Compilation (Biên dịch)

- **Mô tả:** Dịch mã C đã tiền xử lý thành mã Assembly.
- **Output:** File `.s`
- **Lệnh:**

```bash
gcc -S source.i -o source.s
# Hoặc biên dịch trực tiếp từ .c
gcc -S source.c -o source.s

# C++
g++ -S source.cpp -o source.s
```

## 3. Assembly (Lắp ráp)

- **Mô tả:** Dịch mã Assembly thành mã máy dưới dạng object code.
- **Output:** File `.o` hoặc `.obj`
- **Lệnh:**

```bash
gcc -c source.s -o source.o
# Hoặc dịch trực tiếp từ .c
gcc -c source.c -o source.o

# C++
g++ -c source.cpp -o source.o
```

## 4. Linking (Liên kết)

- **Mô tả:** Kết hợp các file object và thư viện để tạo file thực thi.
- **Output:** File thực thi.
- **Lệnh:**

```bash
gcc source.o -o executable
gcc main.o utils.o -o program
gcc source.o -o calculator -lm

# C++
g++ source.o -o executable
g++ main.o utils.o -o program
```

Có thể thực hiện toàn bộ quá trình bằng một lệnh:

```bash
gcc source.c -o executable
g++ source.cpp -o executable
```
