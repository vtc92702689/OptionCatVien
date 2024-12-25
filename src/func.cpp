#include "func.h"
#include <Arduino.h>
#include <LittleFS.h>

// Định nghĩa các biến toàn cục
int btnSetDebounceMill = 20; // Thời gian chống nhiễu cho nút bấm, tránh ghi nhận nhiều lần nhấn do nhiễu
int btnSetPressMill = 1000;  // Thời gian tối thiểu để nhận diện một lần nhấn giữ nút
int pIndex = 1;              // Chỉ số hiện tại của mục cài đặt, dùng để định vị mục trong danh sách
int menuIndex = 1;           // Chỉ số menu hiện tại, dùng để xác định menu đang được chọn
int totalChildren;           // Tổng số phần tử con trong menu hiện tại
int maxValue = 0;            // Giá trị lớn nhất có thể, dùng trong các mục cài đặt
int minValue = 0;            // Giá trị nhỏ nhất có thể, dùng trong các mục cài đặt
int maxLength = 0;           // Độ dài tối đa của chuỗi giá trị, dùng để giới hạn đầu vào
int columnIndex = 0;         // Chỉ số cột hiện tại để chỉnh sửa giá trị, thường dùng khi nhập liệu số
int currentValue;            // Giá trị hiện tại của mục cài đặt, lưu trữ giá trị đang được chỉnh sửa

byte trangThaiHoatDong = 0;  // Trạng thái hoạt động của chương trình, dùng để điều hướng giữa các trạng thái khác nhau
byte mainStep = 0;           // Bước chính trong quy trình hoạt động của chương trình
byte testModeStep = 0;       // Bước kiểm tra trong chế độ kiểm tra
byte maxTestModeStep = 0;    // Bước kiểm tra tối đa trong chế độ kiểm tra

byte testOutputStep = 0;     // Bước xuất kiểm tra
byte maxTestOutputStep = 0;  // Bước xuất kiểm tra tối đa

bool explanationMode = false;   // Chế độ giải thích, cho phép hiển thị thông tin chi tiết về mục cài đặt
bool editAllowed = false;       // Cho phép chỉnh sửa, xác định liệu người dùng có thể chỉnh sửa mục cài đặt hay không
bool hienThiTestOutput = false; // Hiển thị kết quả kiểm tra, dùng để điều hướng hiển thị
bool daoTinHieuOutput = false;  // Đảo tín hiệu đầu ra, dùng khi cần thay đổi trạng thái tín hiệu đầu ra
bool chayTestMode = false;      // Chạy chế độ kiểm tra, xác định liệu chế độ kiểm tra có đang hoạt động

const char* menu1;  // Chuỗi chứa nội dung menu 1, lấy từ tài liệu JSON
const char* menu2;  // Chuỗi chứa nội dung menu 2, lấy từ tài liệu JSON
const char* menu3;  // Chuỗi chứa nội dung menu 3, lấy từ tài liệu JSON
const char* configFile = "/config.json"; // Đường dẫn đến tệp cấu hình, nơi lưu trữ thông tin cấu hình của chương trình

String displayScreen = "index"; // Màn hình hiển thị hiện tại, xác định màn hình nào đang được hiển thị
String setupCodeStr;            // Chuỗi mã cài đặt, dùng để lưu trữ mã cài đặt của mục hiện tại
String valueStr;                // Chuỗi giá trị, lưu trữ giá trị của mục hiện tại dưới dạng chuỗi
String textExplanationMode;     // Chuỗi mô tả chế độ giải thích, chứa nội dung giải thích của mục cài đặt
String textStr;                 // Chuỗi mô tả, chứa thông tin văn bản hiển thị trên màn hình
String keyStr;                  // Chuỗi khóa, dùng để lưu trữ khóa của mục cài đặt trong tài liệu JSON
String ListExp[10];             // Mảng chứa các phần chức năng giải thích thông số, chứa các mục giải thích cho mục cài đặt

// Hàm kiểm tra một chuỗi có phải là số hay không
bool isNumeric(const char* str) {
  if (str == nullptr || str[0] == '\0') return false; // Chuỗi rỗng không được coi là số
  int startIndex = 0; // Bắt đầu từ ký tự đầu tiên
  if (str[0] == '-') { // Nếu ký tự đầu tiên là dấu trừ
    if (str[1] == '\0') return false; // Chỉ có dấu trừ không phải là số
    startIndex = 1; // Bỏ qua dấu trừ
  }
  for (int i = startIndex; str[i] != '\0'; i++) { // Duyệt qua từng ký tự trong chuỗi
    if (!isDigit(str[i])) { // Nếu ký tự không phải là số
      return false;
    }
  }
  return true; // Tất cả các ký tự là số
}

// Hàm tách chuỗi theo dấu phẩy
void splitString(const String& input, String* output, int maxParts) {
  int partCount = 0; // Đếm số phần đã tách
  int startIndex = 0; // Vị trí bắt đầu
  while (partCount < maxParts) {
    int commaIndex = input.indexOf(',', startIndex); // Tìm vị trí dấu phẩy
    if (commaIndex == -1) { // Nếu không tìm thấy dấu phẩy
      output[partCount++] = input.substring(startIndex); // Lấy phần còn lại của chuỗi
      break;
    }
    output[partCount++] = input.substring(startIndex, commaIndex); // Lấy phần từ vị trí bắt đầu đến dấu phẩy
    startIndex = commaIndex + 1; // Cập nhật vị trí bắt đầu
  }
}

// Hàm chờ trong khoảng thời gian bằng milliseconds
bool WaitMillis(unsigned long thoiDiemCuoi, unsigned long waitTime) {
  return (millis() - thoiDiemCuoi > waitTime); // Kiểm tra nếu thời gian chờ đã qua
}

// Hàm chờ trong khoảng thời gian bằng microseconds
bool WaitMicros(unsigned long thoiDiemCuoi, unsigned long waitTime) {
  return (micros() - thoiDiemCuoi > waitTime); // Kiểm tra nếu thời gian chờ đã qua
}

// Hàm vẽ văn bản căn giữa theo chiều ngang
void drawCenteredText(const char* text, int y) {
  int screenWidth = u8g2.getDisplayWidth(); // Lấy độ rộng màn hình
  int textWidth = u8g2.getStrWidth(text); // Lấy độ rộng của chuỗi
  int x = (screenWidth - textWidth) / 2; // Tính vị trí x để căn giữa
  u8g2.drawStr(x, y, text); // Vẽ chuỗi căn giữa
}

// Hàm hiển thị văn bản xuống dòng nếu dài quá
void wrapText(const char* text, int16_t x, int16_t y, int16_t lineHeight, int16_t maxWidth) {
  int16_t cursorX = x; // Vị trí x bắt đầu in
  int16_t cursorY = y; // Vị trí y bắt đầu in
  const char* wordStart = text; // Vị trí bắt đầu của từ trong chuỗi
  const char* currentChar = text; // Ký tự hiện tại đang xử lý
  while (*currentChar) { // Vòng lặp qua từng ký tự trong chuỗi
    if (*currentChar == ' ' || *(currentChar + 1) == '\0') { // Nếu gặp khoảng trắng hoặc kết thúc chuỗi
      char word[64]; // Tạo chuỗi tạm để chứa từ hiện tại
      int len = currentChar - wordStart + 1; // Độ dài từ
      strncpy(word, wordStart, len); // Sao chép từ vào chuỗi tạm
      word[len] = '\0'; // Kết thúc chuỗi
      int16_t textWidth = u8g2.getStrWidth(word); // Độ rộng của từ
      if (cursorX + textWidth > maxWidth) { // Nếu từ không vừa chiều rộng màn hình
        cursorX = x; // Xuống dòng
        cursorY += lineHeight; // Tăng vị trí y
      }
      u8g2.drawStr(cursorX, cursorY, word); // Vẽ từ lên màn hình
      cursorX += textWidth; // Cập nhật vị trí x
      if (*currentChar == ' ') {
        cursorX += u8g2.getStrWidth(" "); // Thêm khoảng trắng
      }
      wordStart = currentChar + 1; // Di chuyển đến từ tiếp theo
    }
    currentChar++; // Chuyển ký tự hiện tại sang ký tự tiếp theo
  }
}

// Hàm ghi log ra Serial
void log(String mes) {
  Serial.println(mes); // In thông điệp ra Serial
}

// Hàm ghi tệp JSON vào hệ thống tệp
void writeFile(JsonDocument& doc, const char* path) {
  File file = LittleFS.open(path, "w"); // Mở tệp ở chế độ ghi
   if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  serializeJson(doc, file); // Ghi dữ liệu JSON vào tệp
  file.close(); // Đóng tệp sau khi ghi
  Serial.println("File written successfully!");
}

// Hàm hiển thị danh sách menu
void showList(int indexNum) {
  menu1 = jsonDoc["main"]["main1"]["text"]; // Lấy nội dung của menu 1 từ JSON
  menu2 = jsonDoc["main"]["main2"]["text"]; // Lấy nội dung của menu 2 từ JSON
  menu3 = jsonDoc["main"]["main3"]["text"]; // Lấy nội dung của menu 3 từ JSON

  u8g2.clearBuffer(); // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3h_tf); // Thiết lập font chữ thường (không đậm)

  u8g2.drawStr(12, 16, menu1); // Hiển thị danh mục 1
  u8g2.drawStr(12, 32, menu2); // Hiển thị danh mục 2
  u8g2.drawStr(12, 48, menu3); // Hiển thị danh mục 3

  u8g2.drawStr(0, indexNum * 16, ">"); // Hiển thị dấu nhắc tại vị trí mục đã chọn
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

// Hàm hiển thị văn bản với tiêu đề và nội dung
void showText(const char* title, const char* messenger) {
  u8g2.clearBuffer(); // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3hb_tf); // Thiết lập font chữ đậm

  drawCenteredText(title, 18); // Vẽ tiêu đề căn giữa

  u8g2.setFont(u8g2_font_crox3h_tf); // Thiết lập font chữ thường (không đậm)
  wrapText(messenger, 0, 42, 18, 128); // Hiển thị nội dung văn bản xuống dòng nếu dài quá
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

// Hàm hiển thị tiến trình
void showProgress(int parameter1, int parameter2, int parameter3) {
  u8g2.clearBuffer(); // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3h_tf); // Thiết lập font chữ thường (không đậm)

  u8g2.drawStr(0, 18, "Total Stick: "); // Hiển thị tiêu đề tổng số
  u8g2.drawStr(100, 18, String(parameter1).c_str()); // Hiển thị giá trị tổng số

  u8g2.drawStr(0, 36, "Re.Stick: "); // Hiển thị tiêu đề Re.Stick
  u8g2.drawStr(100, 36, String(parameter2).c_str()); // Hiển thị giá trị Re.Stick

  u8g2.drawStr(0, 54, "Count output: "); // Hiển thị tiêu đề Count output
  u8g2.drawStr(100, 54, String(parameter3).c_str()); // Hiển thị giá trị Count output

  u8g2.sendBuffer(); // Gửi dữ liệu từ bộ đệm lên màn hình
}

// Hàm hiển thị cài đặt
void showSetup(const char* setUpCode, const char* value, const char* text) {
  u8g2.clearBuffer(); // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3hb_tf); // Thiết lập font chữ đậm

  char tempSetUpCode[64]; // Tạo chuỗi tạm chứa mã cài đặt và dấu ";"
  snprintf(tempSetUpCode, sizeof(tempSetUpCode), "%s:", setUpCode); // Nối mã cài đặt với dấu ":"
  u8g2.drawStr(0, 18, tempSetUpCode); // Hiển thị mã cài đặt

  u8g2.setFont(u8g2_font_crox3h_tf); // Thiết lập font chữ thường (không đậm)

  char maxValueStr[16]; // Chuỗi chứa giá trị maxValue sau khi chuyển đổi
  snprintf(maxValueStr, sizeof(maxValueStr), "%d", maxValue); // Chuyển maxValue thành chuỗi

  int valueLength = strlen(value); // Độ dài của chuỗi giá trị
  if (!isNumeric(value)) { // Nếu giá trị không phải là số
    maxLength = 5;
  } else {
    maxLength = strlen(maxValueStr); // Độ dài tối đa của giá trị
    if (valueLength > maxLength) {
      valueLength = maxLength;
    }
  }

  int startX = 128 - 10; // Bắt đầu từ vị trí rìa phải

  for (int i = 0; i < valueLength; i++) {
    char temp[2] = {value[valueLength - 1 - i], '\0'}; // Lấy ký tự theo thứ tự ngược lại
    u8g2.drawStr(startX - (i * 10), 18, temp); // Vẽ ký tự lùi về bên trái
  }

  u8g2.drawLine(0, 23, 128, 23); // Vẽ một đường ngang trên màn hình

  wrapText(text, 0, 42, 18, 128); // Hiển thị nội dung văn bản xuống dòng nếu dài quá
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

// Hàm hiển thị chế độ chỉnh sửa
void showEdit(int columnIndex) {
  u8g2.clearBuffer(); // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3hb_tf); // Thiết lập font chữ đậm

  char tempSetUpCode[64]; // Tạo chuỗi tạm chứa mã cài đặt và dấu ";"
  snprintf(tempSetUpCode, sizeof(tempSetUpCode), "%s:", setupCodeStr.c_str()); // Nối mã cài đặt với dấu ":"
  u8g2.drawStr(0, 18, tempSetUpCode); // Hiển thị mã cài đặt

  u8g2.setFont(u8g2_font_crox3h_tf); // Thiết lập font chữ thường (không đậm)

  char maxValueStr[16]; // Chuỗi chứa giá trị maxValue sau khi chuyển đổi
  snprintf(maxValueStr, sizeof(maxValueStr), "%d", maxValue); // Chuyển maxValue thành chuỗi

  const char* valueChr = valueStr.c_str(); // Lấy giá trị hiện tại
  int valueLength = strlen(valueChr); // Độ dài của giá trị hiện tại
  maxLength = strlen(maxValueStr); // Độ dài tối đa của giá trị

  if (valueLength > maxLength) {
    valueLength = maxLength;
  }

  int startX = 128 - 10; // Bắt đầu từ vị trí rìa phải

  for (int i = 0; i < maxLength; i++) {
    char temp[2] = {valueChr[valueLength - 1 - i], '\0'}; // Lấy ký tự theo thứ tự ngược lại
    if (i == columnIndex) {
      u8g2.setDrawColor(1); // Đặt màu nền
      u8g2.drawBox(startX - (i * 10) - 1, 5, 10, 18); // Vẽ ô vuông làm nền
      u8g2.setDrawColor(0); // Đặt màu chữ
      u8g2.drawStr(startX - (i * 10), 18, temp); // Vẽ ký tự
      u8g2.setDrawColor(1); // Khôi phục màu nền
    } else {
      u8g2.drawStr(startX - (i * 10), 18, temp); // Vẽ ký tự
    }
  }

  u8g2.drawLine(0, 23, 128, 23); // Vẽ một đường ngang trên màn hình

  wrapText(textStr.c_str(), 0, 42, 18, 128); // Hiển thị nội dung văn bản xuống dòng nếu dài quá
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

// Hàm tải cấu hình từ JSON
// Hàm tải cấu hình từ JSON (tiếp tục)
void loadJsonSettings() {
  try {
    const char* code = jsonDoc["main"]["main" + String(menuIndex)]["key"]; // Lấy key của menu hiện tại từ JSON
    totalChildren = jsonDoc["main"]["main" + String(menuIndex)]["totalChildren"]; // Lấy tổng số phần tử con
    setupCodeStr = String(code) + String(pIndex); // Tạo setupCode dựa trên key và pIndex

    // Kiểm tra xem configuredValue có phải là số không
    if (jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"].is<int>()) {
      int valueInt = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"];
      valueStr = String(valueInt); // Chuyển đổi value từ int thành String
      currentValue = valueStr.toInt(); // Cập nhật currentValue
    } else if (jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"].is<const char*>()) {
      valueStr = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"].as<const char*>();
      currentValue = -1; // Nếu không phải là số thì đặt currentValue là -1
    } else {
      valueStr = ""; // Gán giá trị mặc định nếu không phải số hoặc chuỗi
    }

    maxValue = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["maxValue"];
    minValue = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["minValue"];
    explanationMode = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["explanationMode"];
    editAllowed = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["editAllowed"];
    
    // Kiểm tra chế độ giải thích
    if (explanationMode) {
      String listStr = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["explanationDetails"];
      splitString(listStr, ListExp, 10);
      textExplanationMode = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["text"].as<const char*>();
      textStr = textExplanationMode + ": " + ListExp[currentValue - 1]; // Hiển thị text từ list
    } else {
      textStr = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["text"].as<const char*>();
    }

    keyStr = String(code);
    log("Truy cập thẻ " + keyStr + "/" + setupCodeStr);
    showSetup(setupCodeStr.c_str(), valueStr.c_str(), textStr.c_str());
  } catch (const std::exception& e) {
    Serial.println("Error reading JSON data: "); // In thông báo lỗi
    Serial.println(e.what()); // In thông tin chi tiết từ e.what()
  } catch (...) {
    Serial.println("An unknown error occurred while reading JSON data."); // In thông báo lỗi chung
  }
}

// Hàm chỉnh sửa giá trị
void editValue(const char* Calculations) {
  int newValue;
  int factor = pow(10, columnIndex); // Tính hàng (đơn vị, chục, trăm, v.v.)

  if (strcmp(Calculations, "addition") == 0) {
    newValue = currentValue + factor; // Tăng giá trị tại hàng đang chọn
  } else if (strcmp(Calculations, "subtraction") == 0) {
    newValue = currentValue - factor; // Giảm giá trị tại hàng đang chọn
  }

  if (newValue >= minValue && newValue <= maxValue) {
    currentValue = newValue; // Cập nhật currentValue nếu newValue hợp lệ
  }

  valueStr = String(currentValue); // Chuyển giá trị thành chuỗi để hiển thị

  if (explanationMode) {
    textStr = textExplanationMode + ": " + ListExp[currentValue - 1];
  }

  showEdit(columnIndex); // Cập nhật màn hình
}

// Hàm đọc tệp cấu hình
void readConfigFile() {
  File config = LittleFS.open(configFile, "r"); // Mở tệp ở chế độ đọc
  if (!config) {
    Serial.println("Failed to open config file"); // In thông báo lỗi
    return;
  }

  DeserializationError error = deserializeJson(jsonDoc, config);
  if (error) {
    Serial.println("Failed to read config file"); // In thông báo lỗi
    return;
  }
  config.close(); // Đóng tệp sau khi đọc
}

// Hàm đặt lại giá trị cấu hình mặc định
void reSet() {
  int totalPrmReSet = jsonDoc["main"]["main1"]["totalChildren"];
  for (size_t i = 0; i < totalPrmReSet; i++) {
    jsonDoc["main"]["main1"]["children"]["CD" + String(i + 1)]["configuredValue"] = jsonDoc["main"]["main1"]["children"]["CD" + String(i + 1)]["defaultValue"];
  }
  writeFile(jsonDoc, "/config.json"); // Ghi lại tệp cấu hình với các giá trị mặc định
}
