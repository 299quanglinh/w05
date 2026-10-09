
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
    Computer(string id, string name, double price, int year, int ram, string processor, bool gpu)
        : Device(id, name, price, year), ramGB(ram), processorType(processor), hasDedicatedGPU(gpu) {}
    
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
string toString() const override {
    return "Ma thiet bi: " + getDeviceId() + ", RAM: " + to_string(ramGB) + " GB" + ", CPU: " + processorType;
}
};

enum class PrinterType {
    Laser,
    Inkjet
};

class Printer : public Device {
private:
    PrinterType printerType;
    int pagesPrinted;
    bool supportsNetwork;
    bool isColor;

public:
    Printer(string id, string name, double price, int year, PrinterType type, int pages, bool network, bool color)
        : Device(id, name, price, year), printerType(type), pagesPrinted(pages), supportsNetwork(network), isColor(color) {
        if (pagesPrinted < 0) {
            throw invalid_argument("So trang da in khong hop le");
        }
    }
    double calculateAnnualMaintenanceCost() const override {
        double cost = getPurchasePrice() * 0.04;
        if (pagesPrinted > 100000) {
            cost += 500000;
        }
        if (isColor) {
            cost += 300000;
        }
        return cost;
    }
    PrinterType getPrinterType() const {
        return printerType;
    }
    int getPagesPrinted() const {
        return pagesPrinted;
    }
    bool getSupportsNetwork() const {
        return supportsNetwork;
    }
    bool getIsColor() const {
        return isColor;
    }
    string toString() const override {
        string typeName;
        if (printerType == PrinterType::Laser) {
            typeName = "Laser";
        } else {
            typeName = "Phun";
        }
        return "Ma thiet bi: " + getDeviceId() + ", Loai may in: " + typeName + ", So trang da in: " + to_string(pagesPrinted) + ", Ho tro mang: " 
        + (supportsNetwork ? "Co" : "Khong") + ", May in mau: " + (isColor ? "Co" : "Khong");
    }
};


class Projector : public Device {
private:
    int brightnessLumen;
    double lampHoursUsed;
public:
    Projector(string id, string name, double price, int year, int brightness, double hours)
        : Device(id, name, price, year), brightnessLumen(brightness), lampHoursUsed(hours) {
        if (brightnessLumen <= 0) {
            throw invalid_argument("Do sang phai lon hon 0");
        }
        if (lampHoursUsed < 0) {
            throw invalid_argument("So gio su dung khong hop le");
        }
    }
    double calculateAnnualMaintenanceCost() const override {
        double cost = getPurchasePrice() * 0.03;
        if (lampHoursUsed > 3000) {
            cost += 1500000;
        }
        return cost;
    }
    int getBrightnessLumen() const {
        return brightnessLumen;
    }
    double getLampHoursUsed() const {
        return lampHoursUsed;
    }
    string toString() const override {
        return "Ma thiet bi: " + getDeviceId() + ", Do sang: " + to_string(brightnessLumen) + " lumen" + ", Gio su dung bong den: " + to_string(lampHoursUsed);
    }
};


int main() {
    try {
        // 1. Computer
        Computer c1(
            "PC001", "May tinh phong lab",
            15000000, 2020,
            16, "Intel Core i7", true
        );

        // 2. Printer
        Printer p1(
            "PR001", "May in laser mau",
            10000000, 2023,
            PrinterType::Laser, 120000, true, true
        );

        // 3. Projector
        Projector pj1(
            "PJ001", "May chieu phong hoc",
            20000000, 2022,
            4000, 3500
        );

        // In thong tin va chi phi bao tri
        cout << "===== COMPUTER =====" << endl;
        cout << c1.toString() << endl;
        cout << "Chi phi bao tri: "
             << c1.calculateAnnualMaintenanceCost()
             << " dong" << endl;

        cout << "\n===== PRINTER =====" << endl;
        cout << p1.toString() << endl;
        cout << "Chi phi bao tri: "
             << p1.calculateAnnualMaintenanceCost()
             << " dong" << endl;

        cout << "\n===== PROJECTOR =====" << endl;
        cout << pj1.toString() << endl;
        cout << "Chi phi bao tri: "
             << pj1.calculateAnnualMaintenanceCost()
             << " dong" << endl;
    }
    catch (const exception& e) {
        cout << "Loi: " << e.what() << endl;
    }

    return 0;
}
