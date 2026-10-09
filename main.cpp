
#include <iostream>
#include <string>
#include <stdexcept>
#include <ctime>
using namespace std;

enum class DeviceStatus {
    Active,
    UnderMaintenance,
    Retire
};

class Device {
private:
    const string deviceId;
    string deviceName;
    int yearPutIntoService;
    double purchasePrice;   
    DeviceStatus deviceStatus;
public:
    Device(string id, string name, double price, int year) : deviceId(id), deviceName(name), purchasePrice(price), yearPutIntoService(year) {
        if (deviceId.empty()) {
            throw invalid_argument("Ma thiet bi khong duoc de trong");
        }
        if (purchasePrice <= 0) {
            throw invalid_argument("Gia mua phai lon hon 0");
        }
        deviceStatus = DeviceStatus::Active;
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
    string getDeviceName() const {
        return deviceName;
    }
    int getYearPutIntoService() const {
        return yearPutIntoService;
    }
    DeviceStatus getDeviceStatus() const {
        return deviceStatus;
    }
    virtual string toString() const = 0;
};

class Computer : public Device {
private:
    int ramGB;
    string processorType;
    bool hasDedicatedGPU;
public:
    Computer(string id, string name, double price, int year, int ram, string processor, bool gpu) : Device(id, name, price, year) {
        ramGB = ram;
        processorType = processor;
        hasDedicatedGPU = gpu;
    }
    
double calculateAnnualMaintenanceCost() const override {
    time_t now = time(nullptr);
    tm* currentTime = localtime(&now);
    int currentYear = currentTime->tm_year + 1900;
    int yearsUsed = currentYear - getYearPutIntoService();

    if (hasDedicatedGPU && yearsUsed > 5) {
        return getPurchasePrice() * 0.08;
    }
    else if (hasDedicatedGPU) {
        return getPurchasePrice() * 0.07;
    }
    else {
        return getPurchasePrice() * 0.05;
    }
}

int getRamGB() const {
        return ramGB;
}

string getProcessorType() const {
    return processorType;
}

bool getHasDedicatedGPU() const {
    return hasDedicatedGPU;
}

};