#include <iostream>

class Icalc {
public:
    virtual int add(int a, int b) = 0;
    virtual ~Icalc() = default;
};

class calcLogic {
public:
    int add(int a, int b) {
        std::cout << "\nTinh tong: ";
        return a + b;
    }
};

class proxy : public Icalc {
private: calcLogic& logic;
public:explicit proxy(calcLogic& serverLogic)
    :logic(serverLogic) {

}
      int add(int a, int b) override {
          std::cout << "\n[proxy] Yeu cau nhan: add(" << a << "," << b << ")";
          return logic.add(a, b);
      }
};

class client {
public: void run(Icalc& calculator) {
    int result = calculator.add(10, 20);
    std::cout << "\n[client]Tra ket qua: " << result << std::endl;
}
};
int main() {
    calcLogic serverlogic;
    proxy proxy(serverlogic);
    client client;
    client.run(proxy);

    return 0;
}