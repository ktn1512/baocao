### Chức năng file client_if

*CanClientIf là một CAN Listener Client dùng để đăng ký với CanSignalEventMgr/CanMgr và nhận callback khi signal thay đổi.*

Luồng hoạt động:

CAN Signal thay đổi
        ↓
CanSignalEventMgr
        ↓
ICanListener::CanSignalOnchange()
        ↓
CanClientIf

Nói cách khác:

***CanClientIf = client nhận thông báo OnChange từ CAN Service***

- khai báo constructor/destructor mặc định
- callback gọi khi signal thay đổi, nhận `CanSignalID`, `SignalValue` `CanSignalOnchange(int32_t CanSignalID,const std::vector<int8_t>& SignalValue)`

Luồng

EventMgr notify
      ↓
CanSignalOnchange()
      ↓
Client xử lý dữ liệu mới

- hàm `getKey(int64_t* _aidl_return)` lấy địa chỉ obj làm listener ID

- 