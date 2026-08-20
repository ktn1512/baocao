### Tạo 1 class CanSignal

#### protected

- khai báo tag để ghi log `VFCAN::MGR`
- khai báo `SignalID`
- khai báo vector giữ dữ liệu thô của can signal `data`
- khai báo cờ đồng bộ  `isNeedSync`
- khai báo vector chứa dữ liệu can không hợp lệ `invalidVector`
- dùng mutex để bảo vệ dữ liệu can 
- tạo giá trị đếm rst mặc định `invalidCount_resetValue = 1`

#### public
- khởi tạo constructor dùng canid `CanSignal(const uint16_t& id);`
- khởi tạo contructor với các biến cụ thể `CanSignal(const uint16_t& id, const bool& isNeedSync, const std::vector<uint8_t>& initData, const std::vector<uint8_t>& inv);`
- khởi tạo destructor `~CanSignal()`
- khởi tạo 1 copy constructor 
- khởi tạo phương thức getter id, len, isNeedSync, invalidVector, signal(data)
- cập nhật giá trị của can signal `virtual bool setSignal(const vector<uint8_t> &_data, bool force = false);`
- hàm xử lý khi signal thay đổi, phát event thông báo `OnchangeHandle(const vector<uint8_t>& _data)`
- hàm đồng bộ lên connected service `SyncBackToConnSer()`
- hàm lấy giá trị invalid của can signal theo id `GetSignalInvalidValue(uint16_t id)`
- hàm chuyển giá trị thành chuỗi để debug/log `getValueString()`

### Tạo 1 class CanSignalDebounce kế thừa CanSignal

Cập nhật signal CAN có kèm cơ chế chống nhiễu (debounce), chỉ cho phép signal thay đổi khi dữ liệu hợp lệ và vượt ngưỡng thay đổi quy định.

#### public

- tạo obj `CanSignalDebounce(const uint16_t& id, const bool& isNeedSync, const std::vector<uint8_t>& initData, const std::vector<uint8_t>& inv, const uint32_t& _bounceThreshold);`
- hàm override nhận dữ liệu mới, kiểm tra debounce/validity, cập nhật signal nếu cần và phát sinh xử lý onchange `virtual bool setSignal(const vector<uint8_t> &_data, bool force = false) override;`

#### protected

- hàm kiểm tra xem CAN Signal có cần cập nhật (OnChange) hay không `virtual bool shouldOnchange(const vector<uint8_t> &commingData);`
- khai báo giá trị ngưỡng `bounceThreshold`

### Tạo 1 class CANMgr là friend của class CanSignal

#### public

- đồng bộ toàn bộ dữ liệu CAN Signal sang Connected Service `virtual void SyncBackToConnSer();`

- lấy dữ liệu của Signal theo ID (nếu chưa tồn tại thì tạo signal mới và trả về giá trị khởi tạo) `virtual vector<uint8_t> getSignal(uint16_t id);`

- cập nhật dữ liệu cho Signal theo ID `virtual bool setSignal(uint16_t id, const vector<uint8_t> &_data, bool force = false);`

- cập nhật Signal và yêu cầu gửi Signal đó ra CAN Bus `virtual bool updateSignal(uint16_t id, const vector<uint8_t> &_data);`

- parse CAN message nhận được, tách Signal và cập nhật vào hệ thống `virtual bool vipc_can_message_parser(const std::vector<uint8_t> &msg);`

- gửi một Signal ra CAN Bus thông qua vIPC `virtual void vipc_can_send(const CanSignal &signals);`

- gửi nhiều Signal ra CAN Bus thông qua vIPC `virtual void vipc_can_send_multiple(const vector<CanSignal> &signals);`

- gửi frame START để bắt đầu nhận dữ liệu CAN từ vIPC `virtual void vipc_send_start_msg();`

- gửi frame STOP để dừng nhận dữ liệu CAN từ vIPC `virtual void vipc_send_stop_msg();`

- ép toàn bộ Signal về giá trị invalid `virtual void invalidateAll();`

- lấy instance duy nhất của CANMgr (Singleton) `static CANMgr *GetInstance();`

- kiểm tra Signal ID có hợp lệ không `static bool isValidSignal(uint16_t sigID);`

- ánh xạ Signal ID nội bộ sang TBox Signal ID `static bool GetTboxCanSignalID(uint16_t sigID, uint16_t *tbox_sigID);`

- parse raw CAN Signal ID để lấy Signal ID thực tế và độ dài dữ liệu `static void ParseSignalID(uint16_t raw, uint16_t *sigID, uint8_t *len);`

- xóa dữ liệu trước khi TBox chuyển sang trạng thái sleep `void clearDataForSleep();`

- (VF6/VF7/VF8/VF9) kiểm tra Signal có thuộc nhóm DeltaV hay không `bool isDeltaVSignals(uint16_t id);`

#### protected

- tag dùng để ghi log `static constexpr const char *TAG = "VFCAN::MGR";`

- kho lưu trữ toàn bộ CAN Signal trong hệ thống `map<uint16_t, std::unique_ptr<CanSignal>> CanDataPool;`

- cờ đánh dấu trạng thái đã invalidate toàn bộ signal hay chưa `atomic<bool> isAlreadyInvalid;`

#### debug only (VFX_VIPC_DEBUG)

- lấy trực tiếp đối tượng CanSignal theo ID để debug `CanSignal* getSignalById(uint16_t id);`

***

# CAN Manager Architecture

## Tổng quan

`vf_can_mgr.h` định nghĩa tầng quản lý CAN Signal của hệ thống.

Mục tiêu chính:

* Quản lý dữ liệu của các CAN Signal.
* Lọc nhiễu và kiểm soát thay đổi giá trị Signal.
* Parse CAN Message thành CAN Signal.
* Gửi CAN Signal ra CAN Bus.
* Đồng bộ dữ liệu CAN với các module ứng dụng và Connected Service.

---

## Kiến trúc

```text
CAN Bus
   ↕
 CANMgr
   ↕
CanSignal / CanSignalDebounce
   ↕
Application / Connected Service
```

---

## Class CanSignal

### Mô tả

Đại diện cho một CAN Signal trong hệ thống.

### Trách nhiệm

* Lưu trữ giá trị hiện tại của Signal.
* Quản lý giá trị Invalid.
* Phát hiện thay đổi dữ liệu.
* Trigger sự kiện khi Signal thay đổi.
* Hỗ trợ đồng bộ dữ liệu lên Connected Service.

### Vai trò

Là đối tượng dữ liệu cơ bản của một CAN Signal.

---

## Class CanSignalDebounce

### Mô tả

Kế thừa từ `CanSignal` và bổ sung cơ chế Debounce.

### Trách nhiệm

* Lọc nhiễu dữ liệu Signal.
* Kiểm tra dữ liệu mới có thực sự cần cập nhật hay không.
* Ngăn việc phát sinh sự kiện liên tục do dao động giá trị hoặc Invalid Value tạm thời.

### Vai trò

Là lớp `CanSignal` có thêm cơ chế chống nhiễu.

---

## Class CANMgr

### Mô tả

Là lớp quản lý trung tâm của toàn bộ CAN Signal trong hệ thống.

### Trách nhiệm

* Quản lý danh sách các CAN Signal.
* Cung cấp API Get/Set/Update Signal.
* Parse CAN Message thành CAN Signal.
* Gửi Signal ra CAN Bus.
* Đồng bộ dữ liệu với Connected Service.
* Quản lý trạng thái Invalid của toàn hệ thống.
* Triển khai theo Singleton Pattern.

### Vai trò

Là trung tâm điều phối toàn bộ dữ liệu CAN.

---

## Kết luận

`vf_can_mgr.h` xây dựng tầng **CAN Signal Management Layer**, đóng vai trò trung gian giữa CAN Bus và các module ứng dụng.

Ba thành phần chính:

| Class               | Vai trò                                             |
| ------------------- | --------------------------------------------------- |
| `CanSignal`         | Đại diện và quản lý dữ liệu của một CAN Signal      |
| `CanSignalDebounce` | Bổ sung cơ chế lọc nhiễu cho CAN Signal             |
| `CANMgr`            | Quản lý toàn bộ CAN Signal và giao tiếp với CAN Bus |
