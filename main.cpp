
#include <iostream>
#include <string>
#include <stdexcept>
#include <ctime>
#include <vector>
using namespace std;

// ============================================================================
// ENUM: Trạng thái hoạt động của thiết bị
// ============================================================================
enum class DeviceStatus {
    Active,             // Đang hoạt động bình thường
    UnderMaintenance,   // Đang bảo trì
    Retired              // Đã ngưng sử dụng / thanh lý
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
        int currentYear = currentTime->tm_year + 1900;
        int yearsUsed = currentYear - getYearPutIntoService();

        double rate = 0.05;
        if (yearsUsed > 5) {
            rate += 0.01;
        }
        if (hasDedicatedGPU) {
            rate += 0.02;
        }
        return getPurchasePrice() * rate;
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
    bool isColor;               // Có phải máy in màu hay không
    string ipAddress;           // Địa chỉ IP đang hoạt động
    bool connected;             // Cờ trạng thái kết nối mạng

public:
    Printer(string id, string name, double price, int year, PrinterType type, int pages, bool color)
        : Device(id, name, price, year), printerType(type), pagesPrinted(pages), isColor(color), ipAddress(""), connected(false) {
        if (pagesPrinted < 0) {
            throw invalid_argument("So trang da in khong hop le");
        }
    }

    // --- Triển khai các phương thức của Interface INetworkable cho Printer ---

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

// ============================================================================
// CLASS: LabRoom (Quản lý phòng thực hành và danh sách các thiết bị trong phòng)
// ============================================================================
class LabRoom {
private:
    string roomId;              // Mã phòng thực hành
    string roomName;            // Tên phòng thực hành
    int capacity;               // Sức chứa (số lượng người tối đa)
    vector<Device*> devices;    // Danh sách con trỏ quản lý các thiết bị trong phòng

public:
    // Constructor khởi tạo phòng thực hành có validation dữ liệu
    LabRoom(string id, string name, int cap) 
        : roomId(id), roomName(name), capacity(cap) {
        if (roomId.empty()) {
            throw invalid_argument("Ma phong khong duoc de trong");
        }
        if (capacity <= 0) {
            throw invalid_argument("Suc chua phai lon hon 0");
        }
    }

    // Tìm kiếm thiết bị theo mã ID (trả về con trỏ Device* hoặc nullptr nếu không tìm thấy)
    Device* FindDevice(string deviceId) const {
        for (Device* device : devices) {
            if (device->getDeviceId() == deviceId) {
                return device;
            }
        }
        return nullptr;
    }

    // Thêm thiết bị vào phòng (Kiểm tra con trỏ hợp lệ và không trùng mã thiết bị)
    void AddDevice(Device* device) {
        if (device == nullptr) {
            throw invalid_argument("Thiet bi khong hop le");
        }
        if (FindDevice(device->getDeviceId()) != nullptr) {
            throw invalid_argument("Ma thiet bi da ton tai trong phong");
        }
        devices.push_back(device);
    }

    // Xóa thiết bị khỏi phòng theo mã ID (Trả về true nếu xóa thành công, false nếu không tìm thấy)
    bool RemoveDevice(string deviceId) {
        for (auto it = devices.begin(); it != devices.end(); ++it) {
            if ((*it)->getDeviceId() == deviceId) {
                devices.erase(it);
                return true;
            }
        }
        return false;
    }

    // Tính tổng chi phí bảo trì hàng năm của tất cả thiết bị trong phòng (sử dụng đa hình)
    double CalculateAnnualMaintenanceCost() const {
        double totalCost = 0;

        for (Device* device : devices) {
            totalCost += device->calculateAnnualMaintenanceCost();
        }
        return totalCost;
    }

    // Lọc và lấy danh sách các thiết bị cần bảo trì:
    // - Thiết bị đang có trạng thái UnderMaintenance
    // - Hoặc thiết bị đã đưa vào sử dụng trên 5 năm
    vector<Device*> GetDevicesRequiringMaintenance() const {
        vector<Device*> result;

        time_t now = time(nullptr);
        tm* currentTime = localtime(&now);
        int currentYear = currentTime->tm_year + 1900;

        for (Device* device : devices) {
            int yearsUsed = currentYear - device->getYearPutIntoService();
            if (device->getDeviceStatus() == DeviceStatus::UnderMaintenance
                || yearsUsed > 5) {
                result.push_back(device);
            }
        }
        return result;
    }

    // --- Các phương thức Getter ---
    string getRoomId() const {
        return roomId;
    }
    string getRoomName() const {
        return roomName;
    }
    int getCapacity() const {
        return capacity;
    }
    int getDeviceCount() const {
        return static_cast<int>(devices.size());
    }
};

// ============================================================================
// HÀM MAIN: CHƯƠNG TRÌNH KIỂM THỬ TOÀN DIỆN (TEST SUITE)
// ============================================================================
int main() {
    try {
        // --------------------------------------------------------------------
        // Bước 1: Khởi tạo dữ liệu mẫu cho các thiết bị
        // --------------------------------------------------------------------
        // 1. Tạo 2 máy tính (pc1: card onboard thông thường; pc2: card đồ họa rời GPU)
        Computer pc1(
            "PC01", "May tinh van phong",
            15000000, 2022,
            16, "Intel Core i5", false,
            false, ""
        );

        Computer pc2(
            "PC02", "May tinh do hoa",
            30000000, 2020,
            32, "Intel Core i7", true,
            false, ""
        );

        // 2. Tạo 2 máy in (printer1: máy in thường; printer2: máy in màu, in > 100.000 trang)
        Printer printer1(
            "PR01", "May in van phong",
            5000000, 2023,
            PrinterType::Laser, 50000, true, false
        );

        Printer printer2(
            "PR02", "May in kho",
            8000000, 2020,
            PrinterType::Laser, 150000, true, true
        );

        // 3. Tạo máy chiếu (projector1: giờ hoạt động của bóng đèn > 3.000 giờ)
        Projector projector1(
            "PJ01", "May chieu phong hoc",
            20000000, 2021,
            4000, 3500
        );

        // 4. Tạo 2 phòng thực hành quản lý thiết bị
        LabRoom room1("ROOM01", "Phong thuc hanh 1", 3);
        LabRoom room2("ROOM02", "Phong thuc hanh 2", 3);

        // --------------------------------------------------------------------
        // Bước 2: Thêm thiết bị vào từng phòng thực hành (Kiểm thử AddDevice)
        // --------------------------------------------------------------------
        cout << "\n===== 1. THEM THIET BI =====\n";

        room1.AddDevice(&pc1);
        room1.AddDevice(&printer1);
        room1.AddDevice(&projector1);

        room2.AddDevice(&pc2);
        room2.AddDevice(&printer2);

        cout << "Da them thiet bi vao hai phong thanh cong.\n";

        // --------------------------------------------------------------------
        // Bước 3: Kiểm thử bắt lỗi khi thêm thiết bị trùng mã ID (Validation)
        // --------------------------------------------------------------------
        cout << "\n===== 2. KIEM TRA TRUNG MA =====\n";

        Computer pcDuplicate(
            "PC01", "May tinh trung ma",
            12000000, 2024,
            8, "Intel Core i3", false,
            false, ""
        );

        try {
            room1.AddDevice(&pcDuplicate); // PC01 đã tồn tại trong room1, mong đợi ném ngoại lệ
        }
        catch (const invalid_argument& e) {
            cout << "Bat loi thanh cong: " << e.what() << '\n';
        }

        // --------------------------------------------------------------------
        // Bước 4: Xuất thông tin chi tiết các thiết bị trong từng phòng (toString)
        // --------------------------------------------------------------------
        cout << "\n===== 3. DANH SACH THIET BI =====\n";

        cout << "\n--- " << room1.getRoomName() << " ---\n";
        cout << pc1.toString() << '\n';
        cout << printer1.toString() << '\n';
        cout << projector1.toString() << '\n';

        cout << "\n--- " << room2.getRoomName() << " ---\n";
        cout << pc2.toString() << '\n';
        cout << printer2.toString() << '\n';

        // --------------------------------------------------------------------
        // Bước 5: Tính tổng chi phí bảo trì hàng năm mỗi phòng (Đa hình Device*)
        // --------------------------------------------------------------------
        cout << "\n===== 4. CHI PHI BAO TRI =====\n";

        cout << room1.getRoomName() << ": "
             << to_string((long long)room1.CalculateAnnualMaintenanceCost())
             << " VND/nam\n";

        cout << room2.getRoomName() << ": "
             << to_string((long long)room2.CalculateAnnualMaintenanceCost())
             << " VND/nam\n";

        // --------------------------------------------------------------------
        // Bước 6: Lọc và liệt kê danh sách thiết bị cần bảo dưỡng định kỳ
        // --------------------------------------------------------------------
        cout << "\n===== 5. THIET BI CAN BAO TRI =====\n";

        cout << "\n--- " << room1.getRoomName() << " ---\n";
        for (Device* device : room1.GetDevicesRequiringMaintenance()) {
            cout << device->toString() << '\n';
        }

        cout << "\n--- " << room2.getRoomName() << " ---\n";
        for (Device* device : room2.GetDevicesRequiringMaintenance()) {
            cout << device->toString() << '\n';
        }

        // --------------------------------------------------------------------
        // Bước 7: Kiểm thử kết nối mạng thông qua Interface INetworkable*
        // --------------------------------------------------------------------
        cout << "\n===== 6. KET NOI MANG =====\n";

        vector<INetworkable*> networkDevices = {
            &pc1, &pc2, &printer1, &printer2
        };

        vector<string> ipAddresses = {
            "192.168.1.10",
            "192.168.1.11",
            "192.168.1.20",
            "192.168.1.21"
        };

        // Thực hiện kết nối mạng đồng loạt cho các thiết bị
        for (size_t i = 0; i < networkDevices.size(); ++i) {
            try {
                networkDevices[i]->Connect(ipAddresses[i]);
            }
            catch (const exception& e) {
                cout << "Loi ket noi: " << e.what() << '\n';
            }
        }

        // --------------------------------------------------------------------
        // Bước 8: Duyệt danh sách đa hình chỉ thông qua con trỏ INetworkable*
        // --------------------------------------------------------------------
        cout << "\n===== 7. DUYET THIET BI MANG =====\n";

        for (INetworkable* device : networkDevices) {
            cout << "IP: " << device->getIpAddress()
                 << " | Trang thai: "
                 << (device->IsConnected()
                     ? "Dang ket noi"
                     : "Chua ket noi")
                 << '\n';
        }

        // --------------------------------------------------------------------
        // Bước 9: Ngắt kết nối mạng an toàn và dọn dẹp
        // --------------------------------------------------------------------
        cout << "\n===== 8. NGAT KET NOI MANG =====\n";

        for (INetworkable* device : networkDevices) {
            if (device->IsConnected()) {
                device->Disconnect();
            }
        }
    }
    catch (const exception& e) {
        cerr << "Loi chuong trinh: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
