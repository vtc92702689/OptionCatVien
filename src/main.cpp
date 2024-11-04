#include <Arduino.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <OneButton.h>
#include <LittleFS.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // Khởi tạo đối tượng màn hình OLED U8G2


StaticJsonDocument<200> jsonDoc;

const char* jsonString = R"({
  "main": {
    "main1": {
      "text": "CAI DAT",
      "key": "CD",
      "children": {
        "CD1": {
          "key": "CD1",
          "text": "Che do hoat dong",
          "defaultValue": 1,
          "configuredValue": 1,
          "minValue": 1,
          "maxValue": 3,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": true,
          "explanationDetails": "Vao&Ra,D.Vao,D.Ra"
        },
        "CD2": {
          "key": "CD2",
          "text": "Do tre dao - mili giay",
          "defaultValue": 150,
          "configuredValue": 150,
          "minValue": 1,
          "maxValue": 10000,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        },
        "CD3": {
          "key": "CD3",
          "text": "Do tre dau cat vao - mui kim",
          "defaultValue": 10,
          "configuredValue": 10,
          "minValue": 1,
          "maxValue": 10000,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        },
        "CD4": {
          "key": "CD4",
          "text": "Do tre cat dau ra - mui kim",
          "defaultValue": 10,
          "configuredValue": 10,
          "minValue": 1,
          "maxValue": 1000,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        },
        "CD5": {
          "key": "CD5",
          "text": "Che do thoi hoi",
          "defaultValue": 1,
          "configuredValue": 1,
          "minValue": 1,
          "maxValue": 4,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": true,
          "explanationDetails": "Vao&Ra,D.Vao,D.Ra,Full"
        },
        "CD6": {
          "key": "CD6",
          "text": "Do tre thoi hoi dau vao",
          "defaultValue": 1500,
          "configuredValue": 1500,
          "minValue": 100,
          "maxValue": 10000,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        },
        "CD7": {
          "key": "CD7",
          "text": "Do tre thoi hoi dau ra",
          "defaultValue": 1500,
          "configuredValue": 1500,
          "minValue": 100,
          "maxValue": 10000,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        },
        "CD8": {
          "key": "CD8",
          "text": "Do tre thoi hoi khi may",
          "defaultValue": 1500,
          "configuredValue": 1500,
          "minValue": 100,
          "maxValue": 10000,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        },
        "CD9": {
          "key": "CD9",
          "text": "So mui chong nhieu",
          "defaultValue": 2,
          "configuredValue": 2,
          "minValue": 1,
          "maxValue": 10,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        }
      },
      "totalChildren": 9
    },
    "main2": {
      "text": "CHUC NANG",
      "key": "CN",
      "children": {
        "CN1": {
          "key": "CN1",
          "text": "TEST MODE",
          "defaultValue": 1,
          "configuredValue": 1,
          "minValue": 1,
          "maxValue": 1,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        },
        "CN2": {
          "key": "CN2",
          "text": "TEST IO",
          "defaultValue": 1,
          "configuredValue": 1,
          "minValue": 1,
          "maxValue": 2,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        },
        "CN3": {
          "key": "CN3",
          "text": "RESET",
          "defaultValue": 0,
          "configuredValue": 0,
          "minValue": 0,
          "maxValue": 1,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        }
      },
      "totalChildren": 3
    },
    "main3": {
      "text": "GIOI THIEU",
      "key": "GT",
      "children": {
        "GT1": {
          "key": "GT1",
          "text": "DKien Design",
          "defaultValue": 1,
          "configuredValue": 1,
          "minValue": 1,
          "maxValue": 1,
          "accessAllowed": true,
          "editAllowed": true,
          "explanationMode": false,
          "explanationDetails": ""
        }
      },
      "totalChildren": 1
    }
  }
})";

// Khai báo các nút

const int sensorCount = 17;
const int sensorFabric = 16;
const int outRelayCut = 25;
const int outRelayAir = 26;

int btnSetDebounceMill = 20;  // thời gian chống nhiễu phím
int btnSetPressMill = 1000;  // thời gian nhấn giữ phím
int pIndex = 1;
int menuIndex = 1;
int totalChildren; //Tổng số tệp con trong Func loadJsonSettings
int maxValue = 0;
int minValue = 0;
int maxLength = 0; //Số kí tự hiển thị trên func showSetup
int columnIndex = 0; // Biến theo dõi hàng hiện tại (0 = đơn vị, 1 = chục, ...)
int currentValue;

OneButton btnMenu(34, false,false);
OneButton btnSet(35, false,false);
OneButton btnUp(36, false,false);
OneButton btnDown(39, false,false);
OneButton btnRun(32,false,false);
OneButton btnEstop(33,false,false);

bool explanationMode; //logic chức năng diễn giải

const char* menu1;
const char* menu2;
const char* menu3;

String displayScreen = "MENU";
String setupCodeStr;
String valueStr;
String textExplanationMode;
String textStr;
String keyStr;
String ListExp[10]; // Mảng để chứa các phần chức năng Diễn giải thông số

void loadSetup();

bool isNumeric(const char* str) {
  // Chuỗi rỗng không được coi là số
  if (str == nullptr || str[0] == '\0') return false;

  // Nếu ký tự đầu là dấu trừ, bắt đầu kiểm tra từ ký tự thứ 2
  int startIndex = 0;
  if (str[0] == '-') {
    if (str[1] == '\0') return false; // "-"" không phải là số
    startIndex = 1; // Bỏ qua dấu trừ
  }

  // Kiểm tra từng ký tự có phải là số không
  for (int i = startIndex; str[i] != '\0'; i++) {
    if (!isDigit(str[i])) {
      return false; // Nếu có ký tự không phải là số
    }
  }

  return true; // Nếu tất cả ký tự là số
}

void splitString(const String& input, String* output, int maxParts) {
  int partCount = 0; // Đếm số phần đã tách
  int startIndex = 0; // Vị trí bắt đầu

  while (partCount < maxParts) {
    int commaIndex = input.indexOf(',', startIndex); // Tìm dấu phẩy

    // Nếu không tìm thấy dấu phẩy, lấy phần còn lại
    if (commaIndex == -1) {
      output[partCount++] = input.substring(startIndex);
      break; // Thoát khỏi vòng lặp
    }

    // Lấy phần từ startIndex đến dấu phẩy
    output[partCount++] = input.substring(startIndex, commaIndex);
    startIndex = commaIndex + 1; // Cập nhật vị trí bắt đầu
  }
}

bool WaitMillis(unsigned long thoiDiemCuoi, unsigned long waitTime) {
  return (millis() - thoiDiemCuoi > waitTime);
}

bool WaitMicros(unsigned long thoiDiemCuoi, unsigned long waitTime) {
  return (micros() - thoiDiemCuoi > waitTime);
}

void drawCenteredText(const char* text, int y) {
  int screenWidth = u8g2.getDisplayWidth();
  
  // Đo độ rộng của chuỗi
  int textWidth = u8g2.getStrWidth(text);
  
  // Tính toán vị trí x để căn giữa
  int x = (screenWidth - textWidth) / 2;

  u8g2.setFont(u8g2_font_ncenB14_tr); // Chọn font chữ
  u8g2.drawStr(x, y, text);           // Vẽ chuỗi căn giữa
}

void wrapText(const char* text, int16_t x, int16_t y, int16_t lineHeight, int16_t maxWidth) {   // Hàm wrapText để hiển thị văn bản xuống dòng nếu dài quá
  int16_t cursorX = x;  // Vị trí x bắt đầu in
  int16_t cursorY = y;  // Vị trí y bắt đầu in
  const char* wordStart = text;  // Vị trí bắt đầu của từ trong chuỗi
  const char* currentChar = text;  // Ký tự hiện tại đang xử lý

  while (*currentChar) {     // Vòng lặp qua từng ký tự trong chuỗi
    if (*currentChar == ' ' || *(currentChar + 1) == '\0') {    
      char word[64];   // Tạo chuỗi tạm để chứa từ hiện tại
      int len = currentChar - wordStart + 1;
      strncpy(word, wordStart, len);
      word[len] = '\0';

      int16_t textWidth = u8g2.getStrWidth(word);  // Kiểm tra nếu từ có vừa với chiều rộng màn hình
      if (cursorX + textWidth > maxWidth) {
        cursorX = x;  // Nếu từ quá dài, xuống dòng
        cursorY += lineHeight;  // Tăng vị trí y để xuống dòng
      }

      u8g2.drawStr(cursorX, cursorY, word);  // Vẽ từ lên màn hình
      cursorX += textWidth;  // Cập nhật vị trí x cho từ tiếp theo
      
      if (*currentChar == ' ') {
        cursorX += u8g2.getStrWidth(" ");  // Thêm khoảng trắng nếu ký tự là ' '
      }

      wordStart = currentChar + 1; // Di chuyển đến từ tiếp theo
    }
    currentChar++;  // Chuyển ký tự hiện tại sang ký tự tiếp theo
  }
}
void log(String mes){
  Serial.println(mes);
}

void writeFile(JsonDocument& doc, const char* path) {
    File file = LittleFS.open(path, "w");
    if (!file) {
        Serial.println("Failed to open file for writing");
        return;
    }

    // Chuyển đổi JSON thành chuỗi và ghi vào tệp
    serializeJson(doc, file);
    file.close();
    Serial.println("File written successfully!");
}

void showList(int indexNum){
  menu1 = jsonDoc["main"]["main1"]["text"];
  menu2 = jsonDoc["main"]["main2"]["text"];
  menu3 = jsonDoc["main"]["main3"]["text"];

  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)

  
  u8g2.drawStr(12, 16, menu1);  // Hiển thị danh mục 1
  u8g2.drawStr(12, 32, menu2);  // Hiển thị danh mục 2
  u8g2.drawStr(12, 48, menu3);  // Hiển thị danh mục 3

  u8g2.drawStr(0, indexNum * 16, ">");  // Hiển thị mã cài đặt (tại vị trí x=0, y=18)
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

void showText(const char* title, const char* messenger){
  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3hb_tf);  // Thiết lập font chữ đậm

  drawCenteredText(title,18);
  
  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)
  wrapText(messenger, 0, 42, 18, 128);  // Bắt đầu tại tọa độ x=0, y=46, mỗi dòng cách nhau 18 điểm, tối đa chiều rộng 128 điểm
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

void showProgress(int parameter1, int parameter2, int parameter3) {
  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)

  // Hiển thị thông số Total Stick
  u8g2.drawStr(0, 18, "Total Stick: ");
  u8g2.drawStr(100, 18, String(parameter1).c_str());  // Chuyển parameter1 thành chuỗi và hiển thị

  // Hiển thị thông số Re.Stick
  u8g2.drawStr(0, 36, "Re.Stick: ");
  u8g2.drawStr(100, 36, String(parameter2).c_str());  // Chuyển parameter2 thành chuỗi và hiển thị

  // Hiển thị thông số Count output
  u8g2.drawStr(0, 54, "Count output: ");
  u8g2.drawStr(100, 54, String(parameter3).c_str());  // Chuyển parameter3 thành chuỗi và hiển thị

  u8g2.sendBuffer();  // Gửi dữ liệu từ bộ đệm lên màn hình
}


void showSetup(const char* setUpCode, const char* value, const char* text) {   // Thêm maxValue vào tham số
  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3hb_tf);  // Thiết lập font chữ đậm

  char tempSetUpCode[64];    // Tạo một chuỗi tạm chứa mã cài đặt và dấu ";"
  snprintf(tempSetUpCode, sizeof(tempSetUpCode), "%s:", setUpCode);  // Nối mã cài đặt với dấu ":"
  
  u8g2.drawStr(0, 18, tempSetUpCode);  // Hiển thị mã cài đặt (tại vị trí x=0, y=18)
  
  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)
  
  // Chuyển đổi maxValue sang chuỗi
  char maxValueStr[16]; // Chuỗi chứa giá trị maxValue sau khi chuyển đổi
  snprintf(maxValueStr, sizeof(maxValueStr), "%d", maxValue);  // Chuyển maxValue thành chuỗi
  
  // Tính toán độ dài của value và maxValueStr
  int valueLength = strlen(value);
  if (!isNumeric(value)){
    maxLength = 5;
  } else {
    maxLength = strlen(maxValueStr);
    // Giới hạn độ dài value không vượt quá chiều dài maxValue
    if (valueLength > maxLength) {
        valueLength = maxLength;
    }
  }

  // Vị trí bắt đầu cho ký tự đầu tiên căn lề từ phải
  int startX = 128 - 10; // Bắt đầu từ vị trí rìa phải (x = 118)

  // Vẽ từng ký tự từ value lên màn hình, bắt đầu từ ký tự cuối cùng
  for (int i = 0; i < valueLength; i++) {
      char temp[2] = {value[valueLength - 1 - i], '\0'}; // Lấy ký tự theo thứ tự ngược lại
      u8g2.drawStr(startX - (i * 10), 18, temp); // Vẽ ký tự lùi về bên trái
  }

  u8g2.drawLine(0, 23, 128, 23);  // Vẽ một đường ngang trên màn hình (tọa độ từ x=0 đến x=128)

  wrapText(text, 0, 42, 18, 128);  // Bắt đầu tại tọa độ x=0, y=46, mỗi dòng cách nhau 18 điểm, tối đa chiều rộng 128 điểm
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

void showEdit(int columnIndex) {
  u8g2.clearBuffer();  // Xóa bộ nhớ đệm của màn hình để vẽ mới
  u8g2.setFont(u8g2_font_crox3hb_tf);  // Thiết lập font chữ đậm
  char tempSetUpCode[64];    // Tạo một chuỗi tạm chứa mã cài đặt và dấu ";"
  snprintf(tempSetUpCode, sizeof(tempSetUpCode), "%s:", setupCodeStr.c_str());  // Nối mã cài đặt với dấu ":"
  //log(setupCodeStr);
  u8g2.drawStr(0, 18, tempSetUpCode);  // Hiển thị mã cài đặt (tại vị trí x=0, y=18)

  u8g2.setFont(u8g2_font_crox3h_tf);  // Thiết lập font chữ thường (không đậm)

  char maxValueStr[16]; // Chuỗi chứa giá trị maxValue sau khi chuyển đổi
  snprintf(maxValueStr, sizeof(maxValueStr), "%d", maxValue);  // Chuyển maxValue thành chuỗi

  const char* valueChr = valueStr.c_str();
  int valueLength = strlen(valueChr);
  maxLength = strlen(maxValueStr);

  // Giới hạn độ dài value không vượt quá chiều dài maxValue
  if (valueLength > maxLength) {
      valueLength = maxLength;
  }
  log("Value[" +String(columnIndex)+"]");
  int startX = 128 - 10; // 10 là độ rộng trung bình của một ký tự (nếu sử dụng font có kích thước tiêu chuẩn)

    for (int i = 0; i < maxLength; i++) {
      char temp[2] = {valueChr[valueLength - 1 - i], '\0'}; // Lấy ký tự theo thứ tự ngược lại
      if (i == columnIndex){
        u8g2.setDrawColor(1);  // Màu nền
        u8g2.drawBox(startX - (i * 10) - 1, 5, 10, 18);
        u8g2.setDrawColor(0);  // Màu chữ
        u8g2.drawStr(startX - (i * 10), 18, temp); // Vẽ ký tự lùi về bên trái
        u8g2.setDrawColor(1);
      } else {
        u8g2.drawStr(startX - (i * 10), 18, temp); // Vẽ ký tự lùi về bên trái
      }
    }

  u8g2.drawLine(0, 23, 128, 23);  // Vẽ một đường ngang trên màn hình (tọa độ từ x=0 đến x=128)
  wrapText(textStr.c_str(), 0, 42, 18, 128);  // Bắt đầu tại tọa độ x=0, y=46, mỗi dòng cách nhau 18 điểm, tối đa chiều rộng 128 điểm
  u8g2.sendBuffer(); // Gửi nội dung đệm ra màn hình
}

void loadJsonSettings() {
    try {
        const char* code = jsonDoc["main"]["main" + String(menuIndex)]["key"]; // Truy xuất key của mục menu hiện tại từ JSON
        totalChildren = jsonDoc["main"]["main" + String(menuIndex)]["totalChildren"]; // Truy xuất tổng số lượng phần tử con
        setupCodeStr = String(code) + String(pIndex); // Tạo setupCode dựa trên key và pIndex (số thứ tự)

        // Kiểm tra xem configuredValue có phải là số hay không
        if (jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"].is<int>()) {
            // Nếu là số, chuyển đổi sang String
            int valueInt = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"];
            valueStr = String(valueInt); // Chuyển đổi value từ int thành String
            currentValue = valueStr.toInt();
        } else if (jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"].is<const char*>()) {
            // Nếu là chuỗi, lấy giá trị trực tiếp
            valueStr = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"].as<const char*>();
            currentValue = -1;
        } else {
            valueStr = ""; // Gán giá trị mặc định nếu không phải số hoặc chuỗi
        }

        maxValue = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["maxValue"];
        minValue = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["minValue"];
        explanationMode = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["explanationMode"];
        if (explanationMode){
          String listStr = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["explanationDetails"];
          
          splitString(listStr, ListExp, 10);    // Hàm chuyển đổi String thành list
          textExplanationMode = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["text"].as<const char*>();
          textStr = textExplanationMode +": " + ListExp[currentValue-1];  // Hiển thị text từ list
        } else {
          textStr = jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["text"].as<const char*>(); // Truy xuất text từ JSON
        }
        //log(textStr);

        keyStr = String(code);
        log("Truy cập thẻ " + keyStr + "/" + setupCodeStr);
        showSetup(setupCodeStr.c_str(), valueStr.c_str() , textStr.c_str()); // Hiển thị thông tin cấu hình bằng cách gọi hàm showSetup
    } catch (const std::exception& e) { // Bắt lỗi nếu có ngoại lệ
        Serial.println("Error reading JSON data: "); // In thông báo lỗi
        Serial.println(e.what()); // In thông tin chi tiết từ e.what()
    } catch (...) { // Bắt mọi lỗi khác không thuộc std::exception
        Serial.println("An unknown error occurred while reading JSON data."); // In thông báo lỗi chung
    }
}

void reSet(){
   const char* filePath = "/config.json";

  // Kiểm tra tệp có thể đọc được không
  File file = LittleFS.open(filePath, "r");
  if (!file) {
      // Nếu không đọc được tệp, ghi jsonString vào tệp
      Serial.println("File not found, creating a new one...");

      // Mở tệp để ghi
      file = LittleFS.open(filePath, "w");
      if (!file) {
          showSetup("Error", "E001", "LittleFS Error");
          Serial.println("Failed to create file");
          return;
      }
      // Ghi nội dung vào tệp
      file.print(jsonString);
      file.close();
      Serial.println("File written successfully!");
  };
}

void editValue(const char* Calculations) {
    int newValue;
    int factor = pow(10, columnIndex); // Tính hàng (đơn vị, chục, trăm, v.v.)

    // Tăng hoặc giảm giá trị tại hàng đang chọn
    if (strcmp(Calculations, "addition") == 0) {
        newValue = currentValue + factor;
    } else if (strcmp(Calculations, "subtraction") == 0) {
        newValue = currentValue - factor;
    }

    // Kiểm tra nếu newValue nằm trong khoảng minValue và maxValue
    if (newValue >= minValue && newValue <= maxValue) {
        currentValue = newValue; // Cập nhật currentValue nếu newValue hợp lệ
    }

    // Chuyển giá trị thành chuỗi để hiển thị
    valueStr = String(currentValue);

    // Kiểm tra chức năng diễn giải có hoạt động hay không, nếu hoạt động thì hiển thị diễn giải
    if (explanationMode){
      textStr = textExplanationMode + ": " +ListExp[currentValue-1];
    }

    // Gọi lại hàm showEdit để cập nhật màn hình
    showEdit(columnIndex);
}

void btnMenuClick() {
  //Serial.println("Button Clicked (nhấn nhả)");
  if (displayScreen == "ScreenCD") {
    if (keyStr == "CD") {
      writeFile(jsonDoc,"/config.json");
    }
    showList(menuIndex);  // Hiển thị danh sách menu hiện tại
    displayScreen = "MENU";
  } else if (displayScreen == "ScreenEdit") {
    loadJsonSettings();
    displayScreen = "ScreenCD";
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnMenuLongPressStart() {
  //Serial.println("Button Long Press Started (BtnMenu)");
}

// Hàm callback khi nút đang được giữ
void btnMenuDuringLongPress() {
  //Serial.println("Button is being Long Pressed (BtnMenu)");
}

void btnSetClick() {
  if (displayScreen == "MENU") {
    pIndex = 1;
    loadJsonSettings(); // Hiển thị giá trị thiết lập
    displayScreen = "ScreenCD"; // Chuyển màn hình sau khi xử lý dữ liệu thành công
  } else if (displayScreen == "ScreenCD"){
    columnIndex = maxLength-1;
    showEdit(columnIndex);
    displayScreen = "ScreenEdit";
  } else if (displayScreen == "ScreenEdit")  {
    if (columnIndex - 1 < 0){
      columnIndex = maxLength-1;
    } else {
      columnIndex --;
    }
    showEdit(columnIndex);
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnSetLongPressStart() {
  if (displayScreen = "ScreenEdit"){
    if (keyStr == "CD"){
      jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"] = currentValue;
      log("Đã lưu giá trị:" + String(currentValue) + " vào thẻ " + keyStr + "/" + setupCodeStr);
      loadJsonSettings();
      loadSetup();   
      displayScreen = "ScreenCD";
    } else if (keyStr == "CN"){
      if (setupCodeStr == "CN3" && currentValue == 1){
        reSet();
      }
    }
  }
}

// Hàm callback khi nút đang được giữ
void btnSetDuringLongPress() {
  //showSetup("Setup", "OFF", "Dang giu nut");
}

void btnUpClick() {
  if (displayScreen == "MENU") {
    if (menuIndex + 1 > 3) {
      menuIndex = 1;  // Khi chỉ số vượt quá giới hạn, quay lại đầu danh sách
    } else {
      menuIndex++;    // Tăng menuIndex lên 1
    }
    showList(menuIndex);  // Hiển thị danh sách với chỉ số mới
  } else if (displayScreen == "ScreenCD") {
    if (pIndex + 1 > totalChildren) {
      pIndex = 1;
    } else {
      pIndex ++;
    }
    loadJsonSettings(); // Hiển thị giá trị thiết lập
  } else if (displayScreen == "ScreenEdit") {
    editValue("addition");
    log("Value:" + valueStr);
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnUpLongPressStart() { 
  //Serial.println("Button Long Press Started (btnUp)");
}

// Hàm callback khi nút đang được giữ
void btnUpDuringLongPress() {
  //Serial.println("Button is being Long Pressed (btnUp)");
}

void btnDownClick() {
  if (displayScreen == "MENU") {
    if (menuIndex - 1 < 1) {
      menuIndex = 3;  // Khi chỉ số nhỏ hơn giới hạn, quay lại cuối danh sách
    } else {
      menuIndex--;    // Giảm menuIndex đi 1
    }
    showList(menuIndex);  // Hiển thị danh sách với chỉ số mới
  } else if (displayScreen == "ScreenCD"){
    if (pIndex - 1 < 1) {
      pIndex = totalChildren;
    } else {
      pIndex --;
    }
    loadJsonSettings(); // Hiển thị giá trị thiết lập
  } else if (displayScreen == "ScreenEdit"){
    editValue("subtraction");
    log("Value:" + valueStr);
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnDownLongPressStart() {
  //Serial.println("Button Long Press Started (btnDown)");
}

// Hàm callback khi nút đang được giữ
void btnDownDuringLongPress() {
  //Serial.println("Button is being Long Pressed (btnDown)");
}


// TỔNG HỢP THAM SỐ CÀI ĐẶT

byte cheDoHoatDong = 1;
int thoiGianNhaDao = 200;
int soDuMuiDauVao = 10;
int soDuMuiDauRa = 20;
byte cheDoThoiHoi = 1;
int thoiGianThoiHoiDauVao = 1000;
int thoiGianThoiHoiDauRa = 3000;
int thoiGianThoiHoiKhiChay = 5000;
int soMuiChongNhieu = 2;

// TỔNG HỢP THAM SỐ Ghi Nhớ
int mainStep = 0;
int soMuiChiTrongChuKi = 0;
int muiChiCuoiCungThayDoiTrangThai = 0;
int muiChiKetThucChuki = 0;

bool catDauVaoChuKi = false;
bool catDauRaChuKi = false;
bool thoiDauVaoChuKi = false;
bool thoiDauRaChuKi = false;
bool thoiHoiFull = false;
bool lastStatusFabricSensor = false;
bool lastStatusCountSensor = false;
bool trangThaiNhanVai = false;
bool ketThucChuKi = false;
unsigned long thoiDiemCuoiDaoCat;
unsigned long thoiDiemCuoiThoiHoi;
int doTreThoiHoi;

int trangThaiHoatDong = 0;


void loadSetup(){
   log("Tải thông số cài đặt");
   cheDoHoatDong = jsonDoc["main"]["main1"]["children"]["CD1"]["configuredValue"];
   thoiGianNhaDao = jsonDoc["main"]["main1"]["children"]["CD2"]["configuredValue"];
   soDuMuiDauVao = jsonDoc["main"]["main1"]["children"]["CD3"]["configuredValue"];
   soDuMuiDauRa = jsonDoc["main"]["main1"]["children"]["CD4"]["configuredValue"];
   cheDoThoiHoi = jsonDoc["main"]["main1"]["children"]["CD5"]["configuredValue"];
   thoiGianThoiHoiDauVao = jsonDoc["main"]["main1"]["children"]["CD6"]["configuredValue"];
   thoiGianThoiHoiDauRa = jsonDoc["main"]["main1"]["children"]["CD7"]["configuredValue"];
   thoiGianThoiHoiKhiChay = jsonDoc["main"]["main1"]["children"]["CD8"]["configuredValue"];
   soMuiChongNhieu = jsonDoc["main"]["main1"]["children"]["CD9"]["configuredValue"];
   log("cheDoHoatDong :" + String(cheDoHoatDong));
   log("thoiGianNhaDao :" + String(thoiGianNhaDao));
   log("soDuMuiDauVao :" + String(soDuMuiDauVao));
   log("cheDoThoiHoi :" + String(cheDoThoiHoi));
   log("thoiGianThoiHoiDauVao :" + String(thoiGianThoiHoiDauVao));
   log("thoiGianThoiHoiDauRa :" + String(thoiGianThoiHoiDauRa));
   log("thoiGianThoiHoiKhiChay :" + String(thoiGianThoiHoiKhiChay));
   log("soMuiChongNhieu :" + String(soMuiChongNhieu));
   trangThaiHoatDong = 1;
   log("Hoàn thành");
}

void funcKichHoatDaoCat(){
  thoiDiemCuoiDaoCat = millis();
  digitalWrite(outRelayCut,HIGH);
}
void funcCut() {
  if (WaitMillis(thoiDiemCuoiDaoCat,thoiGianNhaDao)){
    digitalWrite(outRelayCut,LOW);
  }
}
void funcKichHoatThoiHoi(int timer){
  doTreThoiHoi = timer;
  thoiDiemCuoiThoiHoi = millis();
  digitalWrite(outRelayAir,HIGH);
}
void funcBlowAir() {
  if (WaitMillis(thoiDiemCuoiThoiHoi,doTreThoiHoi)){
    digitalWrite(outRelayAir,LOW);
    thoiHoiFull = false;
  }
}

void funcFabricSensor(){
  bool statusFabricSensor = digitalRead(sensorFabric);
  if (statusFabricSensor != lastStatusFabricSensor){
    if (statusFabricSensor){
      trangThaiNhanVai = true;
      log("Nhận vải");
      
    } else {
      trangThaiNhanVai = false;
      log("Không có vải");
    }
    muiChiCuoiCungThayDoiTrangThai = soMuiChiTrongChuKi;
    log("Mũi cuối thay đổi trạng thái là: " + String(muiChiCuoiCungThayDoiTrangThai));
    lastStatusFabricSensor = statusFabricSensor;
  }
}

void funcCountSensor(){
  bool statusCountSensor = digitalRead(sensorCount);
  if (statusCountSensor != lastStatusCountSensor){
    if (statusCountSensor){
      soMuiChiTrongChuKi ++;
      log("số mũi chỉ trong chu kì là: " + String(soMuiChiTrongChuKi));
      log("bước là: " + String(mainStep));
      showProgress(soMuiChiTrongChuKi,muiChiCuoiCungThayDoiTrangThai,muiChiKetThucChuki);
      if (ketThucChuKi){
        muiChiKetThucChuki ++;
        log("đếm số mũi đầu ra là :" + String(muiChiKetThucChuki));
      }
      if (cheDoThoiHoi == 4 && !thoiHoiFull){
        thoiHoiFull = true;
        funcKichHoatThoiHoi(thoiGianThoiHoiKhiChay);
        log("thổi hơi full step");
      }
    } else {
      lastStatusCountSensor = statusCountSensor; // thêm trường hợp ngược này để tạo xong giả. khi nào test mạch thì xóa
    }
  }
}

void mainRun(){
  switch (mainStep) {
    case 0:
      if (soMuiChiTrongChuKi - muiChiCuoiCungThayDoiTrangThai > soMuiChongNhieu){
        mainStep ++;
        soMuiChiTrongChuKi = 0;
        log("Khởi tạo chu kì");
        log("bước là: " + String(mainStep)); 
      }
      break;
    case 1: 
      if (cheDoHoatDong == 1 || cheDoHoatDong == 2 ) {
        if (!catDauVaoChuKi){
          if (soMuiChiTrongChuKi == soDuMuiDauVao){
            if (!ketThucChuKi && trangThaiNhanVai){
              funcKichHoatDaoCat();
              log("Cắt đầu vào");
              catDauVaoChuKi = true;
            } else {
              catDauVaoChuKi = true;
            }
          }
        }
      } else {
        catDauVaoChuKi = true;
      }
      if (cheDoThoiHoi == 1 || cheDoThoiHoi == 2) {
        if (!thoiDauVaoChuKi){
          funcKichHoatThoiHoi(thoiGianThoiHoiDauVao);
          log("Thổi đầu vào");
          thoiDauVaoChuKi = true;
        }
      } else {
        thoiDauVaoChuKi = true;
      }
      if (catDauVaoChuKi && thoiDauVaoChuKi){
        mainStep ++;
      }
      break;
    case 2:
      if (soMuiChiTrongChuKi - muiChiCuoiCungThayDoiTrangThai > soMuiChongNhieu && !trangThaiNhanVai){
        if (cheDoThoiHoi == 1 || cheDoThoiHoi == 3){
        if (!thoiDauRaChuKi){
          funcKichHoatThoiHoi(thoiGianThoiHoiDauRa);
          log("Thổi đầu ra");
          thoiDauRaChuKi = true;
        }
      } else {
        thoiDauRaChuKi = true;
      } 
        catDauVaoChuKi = false;
        catDauRaChuKi = false;
        thoiDauVaoChuKi = false;
        thoiDauRaChuKi = false;
        mainStep = 0;
        ketThucChuKi = true;
        muiChiKetThucChuki = 0;
      }
      break;
  }
  if (ketThucChuKi) {
    if (muiChiKetThucChuki == soDuMuiDauRa){
        if (cheDoHoatDong == 1 || cheDoHoatDong == 3 ){
          if (!catDauRaChuKi){
              funcKichHoatDaoCat();
              log("Cắt đầu ra");
              catDauRaChuKi = true;
          }
        } else {
          catDauRaChuKi = true;
        }
      ketThucChuKi = false;
    }
  }
  if (thoiHoiFull){
    /* code */
  }
  

  funcBlowAir();
  funcCut();
}

void setup() {

  Serial.begin(115200);     // Khởi tạo Serial và màn hình

  log("Đang khởi động");

  u8g2.begin();  // Khởi tạo màn hình OLED

  btnMenu.attachClick(btnMenuClick);
  btnMenu.attachLongPressStart(btnMenuLongPressStart);
  btnMenu.attachDuringLongPress(btnMenuDuringLongPress);

  btnSet.attachClick(btnSetClick);
  btnSet.attachLongPressStart(btnSetLongPressStart);
  btnSet.attachDuringLongPress(btnSetDuringLongPress);

  btnUp.attachClick(btnUpClick);
  btnUp.attachLongPressStart(btnUpLongPressStart);
  btnUp.attachDuringLongPress(btnUpDuringLongPress);

  btnDown.attachClick(btnDownClick);  
  btnDown.attachLongPressStart(btnDownLongPressStart);
  btnDown.attachDuringLongPress(btnDownDuringLongPress);

  btnMenu.setDebounceMs(btnSetDebounceMill);
  btnSet.setDebounceMs(btnSetDebounceMill);
  btnUp.setDebounceMs(btnSetDebounceMill);
  btnDown.setDebounceMs(btnSetDebounceMill);

  btnMenu.setPressMs(btnSetPressMill);
  btnSet.setPressMs(btnSetPressMill);
  btnUp.setPressMs(btnSetPressMill);
  btnDown.setPressMs(btnSetPressMill);

  pinMode(sensorFabric, INPUT);
  pinMode(sensorCount, INPUT);
  pinMode(outRelayCut, OUTPUT);
  pinMode(outRelayAir, OUTPUT);

  digitalWrite(outRelayCut,LOW);
  digitalWrite(outRelayAir,LOW);


  if (!LittleFS.begin()) {
    showSetup("Error", "E003", "LittleFS Mount Failed");
    Serial.println("LittleFS Mount Failed");
    DeserializationError error = deserializeJson(jsonDoc, jsonString); // Phân tích chuỗi JSON
    if (error) {
        showSetup("Error", "E002", "Json Error");
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    Serial.println("Read Data From JsonString");
    loadSetup();
    return;
  }

  const char* filePath = "/config.json";

  // Kiểm tra tệp có thể đọc được không
  File file = LittleFS.open(filePath, "r");
  if (!file) {
      // Nếu không đọc được tệp, ghi jsonString vào tệp
      Serial.println("File not found, creating a new one...");

      // Mở tệp để ghi
      file = LittleFS.open(filePath, "w");
      if (!file) {
          showSetup("Error", "E001", "LittleFS Error");
          Serial.println("Failed to create file");
          return;
      }
      
      // Ghi nội dung vào tệp
      file.print(jsonString);
      file.close();
      Serial.println("File written successfully!");

      // Mở lại file sau khi ghi thành công
      file = LittleFS.open(filePath, "r");
      if (!file) {
          showSetup("Error", "E004", "Failed to Re-open File");
          Serial.println("Failed to re-open file");
          return;
      }
  }

  const size_t capacity = 4000;  // Kích thước buffer, có thể thay đổi theo dung lượng file
  char content[capacity];        // Mảng chứa nội dung file

  if (file) {
    size_t len = file.readBytes(content, capacity); // Đọc nội dung file vào buffer
    content[len] = '\0';  // Đảm bảo buffer kết thúc bằng null terminator
  }
  file.close(); // Đóng file

  // Sử dụng ArduinoJson để phân tích cú pháp JSON
  DeserializationError error = deserializeJson(jsonDoc, content); // Phân tích chuỗi JSON
  if (error) {
      showSetup("Error", "E002", "Json Error");
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
  }
  
  // Nếu mọi thứ thành công, tiếp tục với quá trình cài đặt của bạn
}

void loop() {
  switch (trangThaiHoatDong){
  case 0:
    
    break;
  case 1:
    btnMenu.tick();
    btnSet.tick();
    btnUp.tick();
    btnDown.tick();
    funcFabricSensor();
    funcCountSensor();
    mainRun();
    break;
  case 2:

    break;
  default:
    break;
  }
  
}