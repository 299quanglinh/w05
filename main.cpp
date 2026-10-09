
#include <iostream>
#include <string>
#include <stdexcept>
using namespace std;

class Device {
private:
    const string deviceId;
    string deviceName;
    int yearPutIntoService;
    double purchasePrice;
    bool deviceStatus;

public:
    Device(string id, double price) : deviceId(id), purchasePrice(price) {
        if (deviceId.empty()) {
            throw invalid_argument("Ma thiet bi khong duoc de trong");
        }
        if (purchasePrice <= 0) {
            throw invalid_argument("Gia mua phai lon hon 0");
        }
        deviceStatus = true;
    }
    string getDeviceId() const {
        return deviceId;
    }
    double getPurchasePrice() const {
        return purchasePrice;
    }
};

int main() {
    return 0;
}
