
#include <iostream>
#include <string>
#include <stdexcept>
#include <ctime>
using namespace std;

// ============================================================================
// ENUM: Trạng thái hoạt động của thiết bị
// ============================================================================
enum class DeviceStatus {
    Active,             // Đang hoạt động bình thường
    UnderMaintenance,   // Đang bảo trì
    Retire              // Đã ngưng sử dụng / thanh lý
};

// ============================================================================
// ABSTRACT BASE CLASS: Device (Lớp cơ sở trừu tượng quản lý thông tin chung)
// ============================================================================
class Device {
private:
    const string deviceId;      // Mã thiết bị (hằng số, không được thay đổi sau khi tạo)
    string deviceName;          // Tên thiết bị
    int yearPutIntoService;     // Năm đưa vào sử dụng
    double purchasePrice;       // Giá mua ban đầu
    DeviceStatus deviceStatus;  // Trạng thái thiết bị

public:
    // Constructor kiểm tra tính hợp lệ dữ liệu đầu vào (Validation)
    Device(string id, string name, double price, int year) 
        : deviceId(id), deviceName(name), purchasePrice(price), yearPutIntoService(year) {
        
        // Kiểm tra mã thiết bị
        if (deviceId.empty()) {
            throw invalid_argument("Ma thiet bi khong duoc de trong");
        }
        // Kiểm tra giá mua
        if (purchasePrice <= 0) {
            throw invalid_argument("Gia mua phai lon hon 0");
        }
        
        // Mặc định trạng thái ban đầu là Active
        deviceStatus = DeviceStatus::Active;
        
        // Kiểm tra năm đưa vào sử dụng không được lớn hơn năm hiện tại
        time_t now = time(nullptr);
        tm* currentTime = localtime(&now);
        int currentYear = currentTime->tm_year + 1900;
        if (yearPutIntoService > currentYear) {
            throw invalid_argument("Nam su dung khong duoc lon hon nam hien tai");
        }
    }

    // Phương thức thuần ảo (Pure virtual): Bắt buộc các lớp con phải tự triển khai cách tính
    virtual double calculateAnnualMaintenanceCost() const = 0;

    // Virtual Destructor: Đảm bảo hủy đối tượng an toàn khi sử dụng con trỏ đa hình
    virtual ~Device() = default;

    // --- Các phương thức Getter ---
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

    // Phương thức thuần ảo trả về chuỗi mô tả thông tin thiết bị
    virtual string toString() const = 0;
};

// ============================================================================
// INTERFACE: INetworkable (Quy định hành vi kết nối mạng)
// ============================================================================
class INetworkable {
public:
    virtual ~INetworkable() = default;

    virtual string getIpAddress() const = 0;    // Lấy địa chỉ IP hiện tại
    virtual void Connect(string ipAddress) = 0; // Kết nối tới mạng với IP được chỉ định
    virtual void Disconnect() = 0;              // Ngắt kết nối khỏi mạng
    virtual bool IsConnected() const = 0;       // Kiểm tra trạng thái đang kết nối hay không
};

// ============================================================================
// DERIVED CLASS: Computer (Kế thừa Device và thực thi Interface INetworkable)
// ============================================================================
class Computer : public Device, public INetworkable {
private:
    int ramGB;                  // Dung lượng RAM (GB)
    string processorType;       // Loại CPU/vi xử lý
    bool hasDedicatedGPU;       // Có card đồ họa rời hay không
    string ipAddress;           // Địa chỉ IP đang hoạt động
    bool connected;             // Cờ đánh dấu trạng thái kết nối mạng

public:
    // Constructor khởi tạo máy tính cùng trạng thái mạng ban đầu
    Computer(string id, string name, double price, int year,
             int ram, string processor, bool gpu,
             bool connect, string ip)
        : Device(id, name, price, year),
          ramGB(ram),
          processorType(processor),
          hasDedicatedGPU(gpu),
          ipAddress(ip),
          connected(connect) {

        // Nếu khai báo đang kết nối thì bắt buộc phải có địa chỉ IP
        if (connected && ipAddress.empty()) {
            throw invalid_argument(
                "Dia chi IP khong duoc de trong khi o trang thai da ket noi"
            );
        }
    }

    // --- Triển khai các phương thức của Interface INetworkable ---

    // Kết nối mạng: Kiểm tra IP không rỗng và chưa kết nối trước đó
    void Connect(string ipAddress) override {
        if (ipAddress.empty()) {
            throw invalid_argument(
                "Dia chi IP khong duoc de trong"
            );
        }

        if (IsConnected()) {
            throw invalid_argument(
                "Thiet bi da ket noi mang truoc do"
            );
        }

        this->ipAddress = ipAddress;
        connected = true;

        cout << getDeviceName()
             << " da ket noi toi mang voi IP: "
             << this->ipAddress << endl;
    }

    // Ngắt kết nối mạng: Xóa địa chỉ IP và cập nhật trạng thái
    void Disconnect() override {
        if (!IsConnected()) {
            throw invalid_argument(
                "Thiet bi chua ket noi mang"
            );
        }

        connected = false;
        ipAddress = ""; // Xóa địa chỉ IP đang hoạt động

        cout << getDeviceName()
             << " da ngat ket noi mang.\n";
    }

    // Lấy địa chỉ IP
    string getIpAddress() const override {
        return ipAddress;
    }

    // Kiểm tra trạng thái kết nối
    bool IsConnected() const override {
        return connected;
    }

    // --- Triển khai tính toán chi phí bảo trì hàng năm của Device ---
    // - Có GPU rời và sử dụng trên 5 năm: 8% giá mua
    // - Có GPU rời: 7% giá mua
    // - Còn lại: 5% giá mua
    double calculateAnnualMaintenanceCost() const override {
        time_t now = time(nullptr);
        tm* currentTime = localtime(&now);

        if (currentTime == nullptr) {
            throw runtime_error(
                "Khong lay duoc nam hien tai"
            );
        }

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

    // --- Getters của Computer ---
    int getRamGB() const {
        return ramGB;
    }

    string getProcessorType() const {
        return processorType;
    }

    bool getHasDedicatedGPU() const {
        return hasDedicatedGPU;
    }

    // Mô tả thông tin máy tính
    string toString() const override {
        return "Ma thiet bi: " + getDeviceId()
             + ", RAM: " + to_string(ramGB) + " GB"
             + ", CPU: " + processorType;
    }
};

// ============================================================================
// ENUM: Phân loại máy in
// ============================================================================
enum class PrinterType {
    Laser,  // Máy in Laser
    Inkjet  // Máy in phun
};

// ============================================================================
// DERIVED CLASS: Printer (Kế thừa Device và thực thi Interface INetworkable)
// ============================================================================
class Printer : public Device, public INetworkable {
private:
    PrinterType printerType;    // Loại máy in
    int pagesPrinted;           // Tổng số trang đã in
    bool supportsNetwork;       // Có hỗ trợ kết nối mạng hay không
    bool isColor;               // Có phải máy in màu hay không
    string ipAddress;           // Địa chỉ IP đang hoạt động
    bool connected;             // Cờ trạng thái kết nối mạng

public:
    Printer(string id, string name, double price, int year, PrinterType type, int pages, bool network, bool color)
        : Device(id, name, price, year), printerType(type), pagesPrinted(pages), 
          supportsNetwork(network), isColor(color), ipAddress(""), connected(false) {
        if (pagesPrinted < 0) {
            throw invalid_argument("So trang da in khong hop le");
        }
    }

    // --- Triển khai các phương thức của Interface INetworkable cho Printer ---

    // Kết nối mạng: Kiểm tra hỗ trợ mạng, địa chỉ IP không rỗng và chưa kết nối trước đó
    void Connect(string ipAddress) override {
        // Kiểm tra xem máy in có hỗ trợ kết nối mạng hay không
        if (!supportsNetwork) {
            throw invalid_argument("May in khong ho tro ket noi mang");
        }

        if (ipAddress.empty()) {
            throw invalid_argument(
                "Dia chi IP khong duoc de trong"
            );
        }

        if (IsConnected()) {
            throw invalid_argument(
                "Thiet bi da ket noi mang truoc do"
            );
        }

        this->ipAddress = ipAddress;
        connected = true;

        cout << getDeviceName()
             << " da ket noi toi mang voi IP: "
             << this->ipAddress << endl;
    }

    // Ngắt kết nối mạng: Xóa địa chỉ IP và cập nhật trạng thái
    void Disconnect() override {
        if (!IsConnected()) {
            throw invalid_argument(
                "Thiet bi chua ket noi mang"
            );
        }

        connected = false;
        ipAddress = ""; // Xóa địa chỉ IP khi ngắt kết nối

        cout << getDeviceName()
             << " da ngat ket noi mang.\n";
    }

    // Lấy địa chỉ IP hiện tại của máy in
    string getIpAddress() const override {
        return ipAddress;
    }

    // Kiểm tra trạng thái kết nối mạng của máy in
    bool IsConnected() const override {
        return connected;
    }
    // Chi phí bảo trì máy in:
    // - Cơ bản: 4% giá mua
    // - In > 100,000 trang: cộng thêm 500,000 đ
    // - In màu: cộng thêm 300,000 đ
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

    // --- Getters của Printer ---
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

    // Mô tả thông tin máy in
    string toString() const override {
        string typeName = (printerType == PrinterType::Laser) ? "Laser" : "Phun";
        return "Ma thiet bi: " + getDeviceId() + ", Loai may in: " + typeName + ", So trang da in: " + to_string(pagesPrinted) + ", Ho tro mang: " 
        + (supportsNetwork ? "Co" : "Khong") + ", May in mau: " + (isColor ? "Co" : "Khong");
    }
};

// ============================================================================
// DERIVED CLASS: Projector (Kế thừa Device)
// ============================================================================
class Projector : public Device {
private:
    int brightnessLumen;    // Độ sáng (Lumen)
    double lampHoursUsed;   // Số giờ hoạt động của bóng đèn

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

    // Chi phí bảo trì máy chiếu:
    // - Cơ bản: 3% giá mua
    // - Giờ đèn > 3000 giờ: cộng thêm 1,500,000 đ phí thay bóng đèn
    double calculateAnnualMaintenanceCost() const override {
        double cost = getPurchasePrice() * 0.03;
        if (lampHoursUsed > 3000) {
            cost += 1500000;
        }
        return cost;
    }

    // --- Getters của Projector ---
    int getBrightnessLumen() const {
        return brightnessLumen;
    }
    double getLampHoursUsed() const {
        return lampHoursUsed;
    }

    // Mô tả thông tin máy chiếu
    string toString() const override {
        return "Ma thiet bi: " + getDeviceId() + ", Do sang: " + to_string(brightnessLumen) + " lumen" + ", Gio su dung bong den: " + to_string(lampHoursUsed);
    }
};

#include <vector>

int main() {
    cout << "==========================================================" << endl;
    cout << "           CHUONG TRINH KIEM THU QUAN LY THIET BI         " << endl;
    cout << "==========================================================\n" << endl;

    // ========================================================================
    // PHAN 1: KIEM THU DA HINH QUA LOP CO SO Device*
    // ========================================================================
    cout << ">>> [PHAN 1] Quan ly danh sach thiet bi da hinh qua Device* <<<\n" << endl;

    Computer pc1("PC001", "PC Do hoa Lab 1", 25000000, 2020, 32, "Intel Core i7", true, false, "");
    Printer pr1("PR001", "May in Van phong (Co mang)", 8000000, 2023, PrinterType::Laser, 120000, true, true);
    Printer pr2("PR002", "May in Ca nhan (Khong mang)", 3000000, 2024, PrinterType::Inkjet, 20000, false, false);
    Projector pj1("PJ001", "May chieu Hoi truong", 18000000, 2022, 4500, 3200);

    vector<Device*> devices = { &pc1, &pr1, &pr2, &pj1 };

    double totalMaintenanceCost = 0;
    for (const auto& dev : devices) {
        cout << dev->toString() << endl;
        double cost = dev->calculateAnnualMaintenanceCost();
        cout << "   -> Chi phi bao tri hang nam: " << to_string((long long)cost) << " VND\n" << endl;
        totalMaintenanceCost += cost;
    }
    cout << "==> TONG CHI PHI BAO TRI: " << to_string((long long)totalMaintenanceCost) << " VND\n" << endl;

    // ========================================================================
    // PHAN 2: KIEM THU DA HINH QUA INTERFACE INetworkable*
    // ========================================================================
    cout << ">>> [PHAN 2] Quan ly cac thiet bi mang qua INetworkable* <<<\n" << endl;

    vector<INetworkable*> networkDevices = { &pc1, &pr1 };

    // 1. Ket noi mang dong loat
    cout << "--- 1. Thuc hien ket noi mang ---" << endl;
    pc1.Connect("192.168.1.101");
    pr1.Connect("192.168.1.102");

    // 2. Kiem tra thong tin qua con tro Interface
    cout << "\n--- 2. Kiem tra trang thai qua con tro INetworkable* ---" << endl;
    for (size_t i = 0; i < networkDevices.size(); ++i) {
        cout << "Thiet bi [" << i + 1 << "]:"
             << " IP = " << networkDevices[i]->getIpAddress()
             << " | Da ket noi? " << (networkDevices[i]->IsConnected() ? "CO" : "KHONG") 
             << endl;
    }

    // 3. Ngat ket noi va kiem tra IP da bi xoa hay chua
    cout << "\n--- 3. Ngat ket noi mang ---" << endl;
    for (auto* netDev : networkDevices) {
        netDev->Disconnect();
        cout << "Sau khi ngat: IP = \"" << netDev->getIpAddress() 
             << "\" | Da ket noi? " << (netDev->IsConnected() ? "CO" : "KHONG") << endl;
    }

    // ========================================================================
    // PHAN 3: KIEM THU CAC NGOAI LE (EXCEPTION HANDLING)
    // ========================================================================
    cout << "\n>>> [PHAN 3] Kiem thu cac ngoai le logic mang <<<\n" << endl;

    // Ngoai le 1: Ket noi mang tren may in KHONG ho tro mang (pr2)
    cout << "--- Test Ngoai le 1: May in khong ho tro mang (Mong doi loi) ---" << endl;
    try {
        pr2.Connect("192.168.1.200");
        cout << ">> THAT BAI: Khong nem ngoai le!" << endl;
    } catch (const exception& e) {
        cout << ">> THANH CONG bat loi: \"" << e.what() << "\"" << endl;
    }

    // Ngoai le 2: Ket noi voi IP rong
    cout << "\n--- Test Ngoai le 2: Ket noi voi IP rong (Mong doi loi) ---" << endl;
    try {
        pc1.Connect("");
        cout << ">> THAT BAI: Khong nem ngoai le!" << endl;
    } catch (const exception& e) {
        cout << ">> THANH CONG bat loi: \"" << e.what() << "\"" << endl;
    }

    // Ngoai le 3: Ket noi lai khi da dang ket noi
    cout << "\n--- Test Ngoai le 3: Ket noi lai khi chua ngat (Mong doi loi) ---" << endl;
    try {
        pc1.Connect("192.168.1.101");
        pc1.Connect("192.168.1.105"); // Thu ket noi de
        cout << ">> THAT BAI: Khong nem ngoai le!" << endl;
    } catch (const exception& e) {
        cout << ">> THANH CONG bat loi: \"" << e.what() << "\"" << endl;
    }

    // Ngoai le 4: Ngat ket noi khi chua ket noi
    cout << "\n--- Test Ngoai le 4: Ngat ket noi khi chua ket noi (Mong doi loi) ---" << endl;
    try {
        pr1.Disconnect(); // pr1 da ngat o Phan 2, thu ngat tiep
        cout << ">> THAT BAI: Khong nem ngoai le!" << endl;
    } catch (const exception& e) {
        cout << ">> THANH CONG bat loi: \"" << e.what() << "\"" << endl;
    }

    cout << "\n==========================================================" << endl;
    cout << "                TAT CA TEST CASES HOAN TAT!               " << endl;
    cout << "==========================================================" << endl;

    return 0;
}
