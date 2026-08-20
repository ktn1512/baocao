khai báo kiểu `SignalID` cho ID

khai báo struct dữ liệu truyền trên can:
- `name`: tên signal
- `byteLen`: kích thước signal
- `tboxWriteable`: có được ghi không
- `id`: id của signal
- `bitLen`: signal dài bao nhiêu bit
- `startBit`: bit bắt đầu trong can frame
- `factor`: hệ số scale
- `offset`: độ lệch
- `min`: giá trị vật lí nhỏ nhất hợp lệ
- `max`: giá trị vật lí lớn nhất lợp lệ
- `initVal`: giá trị khởi tạo 