
#include <iostream>
#include <string>
#include <stdexcept>
#include <ctime>
using namespace std;

class Device {
private:
    const string deviceId;
    string deviceName;
    int yearPutIntoService;
    double purchasePrice;   
        bool deviceStatus;
public:
    Device(string id, string name, double price, int year) : deviceId(id), deviceName(name), purchasePrice(price), yearPutIntoService(year) {
        if (deviceId.empty()) {
            throw invalid_argument("Ma thiet bi khong duoc de trong");
        }
        if (purchasePrice <= 0) {
            throw invalid_argument("Gia mua phai lon hon 0");
        }
        deviceStatus = true;
        time_t now = time(nullptr);
        tm* currentTime = localtime(&now);
        int currentYear = currentTime->tm_year + 1900;
        if (yearPutIntoService > currentYear) {
            throw invalid_argument("Nam su dung khong duoc lon hon nam hien tai");
        }
    }
    virtual double calculateAnnualMaintenanceCost() const = 0;
    virtual ~Device() = default;
    string getDeviceId() const {
        return deviceId;
    }       
    double getPurchasePrice() const {
        return purchasePrice;
    }
};


