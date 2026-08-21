### Class CanEventEater : android::IBinder::DeathRecipient

kế thừa android::IBinder::DeathRecipient để nhận callback từ Android Binder khi client chết

#### public:

- lưu lại listenerID `explicit CanEvtDeathEater(const int64_t &listenerID);`
- hàm tự động gọi khi client chết `virtual void binderDied(const android::wp<android::IBinder> &who) override;`

#### private:
- biến listenID

**CanEvtDeathEater là lớp giám sát vòng đời của các ICanListener thông qua cơ chế Android Binder. Khi client đăng ký listener bị chết hoặc mất kết nối, Binder sẽ gọi binderDied(), từ đó hệ thống có thể tự động xóa listener tương ứng bằng listenerID, đảm bảo danh sách listener luôn hợp lệ và tránh rò rỉ tài nguyên.**

### Class CanSignalEventMgr 

#### private:

- Constructor private, không cho tạo object từ bên ngoài `CanSignalEventMgr();`

- Cấm copy obj `CanSignalEventMgr(const CanSignalEventMgr &t) = delete;`

- Cấm gán obj `CanSignalEventMgr& operator=(const CanSignalEventMgr &t) = delete;`

---

- khai báo `enum IDX` để truy cập các phần tử trong tuple
- `LISTENER = 0` :Index của con trỏ ICanListener.
- `DEATH_EATER = 1`: Index của CanEvtDeathEater.
- `SIGNAL_LIST = 2`: Index của danh sách signal đã subscribe.
- map signalID -> listenerManager: `std::unordered_map<int32_t, std::set<int32_t>> listenerManager;`
- Quản lý quan hệ Listener ID → (Listener, DeathRecipient, danh sách Signal)`std::unordered_map<int32_t, std::tuple<...>> eventCache;`

**Chức năng class**

**CanSignalEventMgr quản lý cơ chế Subscribe/Unsubscribe CAN Signal, lưu danh sách listener của từng signal, tự động dọn listener khi client chết thông qua CanEvtDeathEater, và phát sự kiện tới các listener khi giá trị CAN Signal thay đổi.**

Signal thay đổi
    ↓
Phải notify listener
    ↓
Cần biết signal có listener nào
    ↓
listenerManager

Listener chết
    ↓
Phải remove tất cả subscribe
    ↓
Cần biết listener đang nghe signal nào
    ↓
eventCache

Listener chết
    ↓
Ai báo cho mình biết?
    ↓
DeathRecipient
    ↓
CanEvtDeathEater

Chỉ có 1 manager toàn hệ thống
    ↓
Singleton
    ↓
getInstance()
