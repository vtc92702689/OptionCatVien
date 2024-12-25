#ifndef FUNC_H
#define FUNC_H

#include <U8g2lib.h> // Bao gồm thư viện U8g2 để điều khiển màn hình OLED
#include <ArduinoJson.h> // Bao gồm thư viện ArduinoJson để làm việc với JSON

// Khai báo các hàm trong func.cpp
bool isNumeric(const char* str); 
// Hàm kiểm tra một chuỗi có phải là số hay không

void splitString(const String& input, String* output, int maxParts);
// Hàm tách chuỗi theo dấu phẩy thành các phần nhỏ hơn

bool WaitMillis(unsigned long thoiDiemCuoi, unsigned long waitTime);
// Hàm chờ trong khoảng thời gian tính bằng milliseconds

bool WaitMicros(unsigned long thoiDiemCuoi, unsigned long waitTime);
// Hàm chờ trong khoảng thời gian tính bằng microseconds

void drawCenteredText(const char* text, int y);
// Hàm vẽ văn bản căn giữa theo chiều ngang trên màn hình OLED

void wrapText(const char* text, int16_t x, int16_t y, int16_t lineHeight, int16_t maxWidth);
// Hàm hiển thị văn bản xuống dòng nếu quá dài so với chiều rộng màn hình

void log(String mes);
// Hàm ghi log ra Serial

void writeFile(JsonDocument& doc, const char* path);
// Hàm ghi tệp JSON vào hệ thống tệp

void showList(int indexNum);
// Hàm hiển thị danh sách menu

void showText(const char* title, const char* messenger);
// Hàm hiển thị văn bản với tiêu đề và nội dung

void showProgress(int parameter1, int parameter2, int parameter3);
// Hàm hiển thị tiến trình với ba tham số

void showSetup(const char* setUpCode, const char* value, const char* text);
// Hàm hiển thị cài đặt với mã cài đặt, giá trị và văn bản mô tả

void showEdit(int columnIndex);
// Hàm hiển thị chế độ chỉnh sửa, cho phép người dùng chỉnh sửa giá trị tại cột hiện tại

void loadJsonSettings();
// Hàm tải cấu hình từ tài liệu JSON và cập nhật các biến liên quan

void editValue(const char* Calculations);
// Hàm chỉnh sửa giá trị hiện tại dựa trên phép tính (addition hoặc subtraction)

void readConfigFile();
// Hàm đọc tệp cấu hình JSON và nạp nội dung vào tài liệu JSON

void reSet();
// Hàm đặt lại giá trị cấu hình về mặc định và ghi lại tệp JSON với các giá trị này

// Khai báo các biến toàn cục
extern int btnSetDebounceMill; // Thời gian chống nhiễu cho nút bấm, tránh ghi nhận nhiều lần nhấn do nhiễu
extern int btnSetPressMill; // Thời gian tối thiểu để nhận diện một lần nhấn giữ nút
extern int pIndex; // Chỉ số hiện tại của mục cài đặt, dùng để định vị mục trong danh sách
extern int menuIndex; // Chỉ số menu hiện tại, dùng để xác định menu đang được chọn
extern int totalChildren; // Tổng số phần tử con trong menu hiện tại
extern int maxValue; // Giá trị lớn nhất có thể, dùng trong các mục cài đặt
extern int minValue; // Giá trị nhỏ nhất có thể, dùng trong các mục cài đặt
extern int maxLength; // Độ dài tối đa của chuỗi giá trị, dùng để giới hạn đầu vào
extern int columnIndex; // Chỉ số cột hiện tại để chỉnh sửa giá trị, thường dùng khi nhập liệu số
extern int currentValue; // Giá trị hiện tại của mục cài đặt, lưu trữ giá trị đang được chỉnh sửa

extern byte trangThaiHoatDong; // Trạng thái hoạt động của chương trình, dùng để điều hướng giữa các trạng thái khác nhau
extern byte mainStep; // Bước chính trong quy trình hoạt động của chương trình
extern byte testModeStep; // Bước kiểm tra trong chế độ kiểm tra
extern byte maxTestModeStep; // Bước kiểm tra tối đa trong chế độ kiểm tra

extern byte testOutputStep; // Bước xuất kiểm tra
extern byte maxTestOutputStep; // Bước xuất kiểm tra tối đa

extern bool explanationMode; // Chế độ giải thích, cho phép hiển thị thông tin chi tiết về mục cài đặt
extern bool editAllowed; // Cho phép chỉnh sửa, xác định liệu người dùng có thể chỉnh sửa mục cài đặt hay không
extern bool hienThiTestOutput; // Hiển thị kết quả kiểm tra, dùng để điều hướng hiển thị
extern bool daoTinHieuOutput; // Đảo tín hiệu đầu ra, dùng khi cần thay đổi trạng thái tín hiệu đầu ra
extern bool chayTestMode; // Chạy chế độ kiểm tra, xác định liệu chế độ kiểm tra có đang hoạt động

extern const char* menu1; // Chuỗi chứa nội dung menu 1, lấy từ tài liệu JSON
extern const char* menu2; // Chuỗi chứa nội dung menu 2, lấy từ tài liệu JSON
extern const char* menu3; // Chuỗi chứa nội dung menu 3, lấy từ tài liệu JSON
extern const char* configFile; // Đường dẫn đến tệp cấu hình, nơi lưu trữ thông tin cấu hình của chương trình

extern String displayScreen; // Màn hình hiển thị hiện tại, xác định màn hình nào đang được hiển thị
extern String setupCodeStr; // Chuỗi mã cài đặt, dùng để lưu trữ mã cài đặt của mục hiện tại
extern String valueStr; // Chuỗi giá trị, lưu trữ giá trị của mục hiện tại dưới dạng chuỗi
extern String textExplanationMode; // Chuỗi mô tả chế độ giải thích, chứa nội dung giải thích của mục cài đặt
extern String textStr; // Chuỗi mô tả, chứa thông tin văn bản hiển thị trên màn hình
extern String keyStr; // Chuỗi khóa, dùng để lưu trữ khóa của mục cài đặt trong tài liệu JSON
extern String ListExp[10]; // Mảng chứa các phần chức năng giải thích thông số, chứa các mục giải thích cho mục cài đặt

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2; // Khai báo extern cho u8g2, đối tượng điều khiển màn hình OLED
extern StaticJsonDocument<200> jsonDoc; // Khai báo extern cho jsonDoc, tài liệu JSON chứa cấu hình của chương trình

#endif
