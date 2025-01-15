#include <Arduino.h>
#include <OneButton.h>
#include "ota.h"
#include "func.h"  // Bao gồm file header func.h để sử dụng các hàm từ func.cpp


//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // Khởi tạo đối tượng màn hình OLED U8G2
//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // Khởi tạo đối tượng màn hình OLED U8G2

// Thông tin mạng WiFi và OTA


// Khai báo task handle
TaskHandle_t TaskCountSensorHandle = NULL;

StaticJsonDocument<200> jsonDoc;

const char* jsonString = R"()";
void tinhToanCaiDat();
void loadSetup();
void funcCountSensor();

OneButton btnMenu(34, false,false);
OneButton btnSet(35, false,false);
OneButton btnUp(36, false,false);
OneButton btnDown(39, false,false);
OneButton btnRun(32,false,false);
OneButton btnEstop(33,false,false);


void btnMenuClick() {
  //Serial.println("Button Clicked (nhấn nhả)");
  if (displayScreen == "ScreenCD") {
    if (keyStr == "CD") {
      writeFile(jsonDoc,"/config.json");
    }
    showList(menuIndex);  // Hiển thị danh sách menu hiện tại
    trangThaiHoatDong = 0;
    displayScreen = "MENU";
  } else if (displayScreen == "ScreenEdit") {
    loadJsonSettings();
    displayScreen = "ScreenCD";
  } else if (displayScreen == "index" && mainStep == 0) {
    trangThaiHoatDong = 0;
    showList(menuIndex);  // Hiển thị danh sách menu hiện tại
    displayScreen = "MENU";
  } else if (displayScreen == "MENU" && mainStep == 0){
    displayScreen= "index";
    trangThaiHoatDong = 1;
    showText("HELLO", "ESP32-OPTION");
  } else if (displayScreen == "testIO"){
    loadJsonSettings();
    displayScreen = "ScreenCD";
    trangThaiHoatDong = 0;
  } else if (displayScreen == "testOutput"){
    loadJsonSettings();
    displayScreen = "ScreenCD";
    trangThaiHoatDong = 0;
  } else if (displayScreen == "screenTestMode" && testModeStep == 0){
    loadJsonSettings();
    displayScreen = "ScreenCD";
    trangThaiHoatDong = 0;
  } else if (displayScreen == "OTA"){

  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnMenuLongPressStart() {
  if (displayScreen == "OTA") {
  }
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
  } else if (displayScreen == "ScreenCD" && editAllowed){
    if (keyStr == "CD"){
      columnIndex = maxLength-1;
      showEdit(columnIndex);
      displayScreen = "ScreenEdit";
    } else if (keyStr == "CN") {
      if (setupCodeStr == "CN1"){
        trangThaiHoatDong = 201;   //Trạng thái hoạt động 201 là trạng thái TestMode
        testModeStep = 0;
        chayTestMode = true;
        showText("TEST MODE", String("Step " + String(testModeStep)).c_str());
        displayScreen = "screenTestMode";
      } else if (setupCodeStr == "CN2"){
        trangThaiHoatDong = 202;   //Trạng thái hoạt động 202 là trạng thái TEST IO INPUT
        showText("TEST I/O", "TEST I/O INPUT");
        displayScreen = "testIO";
      } else if ((setupCodeStr == "CN3")){
        trangThaiHoatDong = 203;  //Trạng thái hoạt động 203 là trạng thái TEST IO OUTPUT
        testOutputStep = 0;
        displayScreen = "testOutput";
        hienThiTestOutput = true;
      } else if ((setupCodeStr == "CN5")){
        setupOTA();
        displayScreen = "OTA";
        trangThaiHoatDong = 204;  //Trạng thái hoạt động 204 là trạng thái OTA UPDATE+0
      } else {
        columnIndex = maxLength-1;
        showEdit(columnIndex);
        displayScreen = "ScreenEdit";
      }
    }
  } else if (displayScreen == "ScreenEdit")  {
    if (keyStr == "CD"){
      if (columnIndex - 1 < 0){
        columnIndex = maxLength-1;
      } else {
        columnIndex --;
      }
      showEdit(columnIndex);
    }
  } else if (displayScreen == "testOutput"){
    daoTinHieuOutput = true;
  }
}

// Hàm callback khi bắt đầu nhấn giữ nút
void btnSetLongPressStart() {
  if (displayScreen == "ScreenEdit"){
    if (keyStr == "CD"){
      jsonDoc["main"]["main" + String(menuIndex)]["children"][setupCodeStr]["configuredValue"] = currentValue;
      //log("Đã lưu giá trị:" + String(currentValue) + " vào thẻ " + keyStr + "/" + setupCodeStr);
      loadJsonSettings();
      loadSetup();
      tinhToanCaiDat();
      displayScreen = "ScreenCD";
    } else if (keyStr == "CN"){
      if (setupCodeStr == "CN4" && currentValue == 1){
        reSet();
        showText("RESET","Tat May Khoi Dong Lai!");
        trangThaiHoatDong = 200;  //Trạng thái hoạt động 200 là reset, không cho phép thao tác nào
        displayScreen = "RESET";
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
    if (keyStr == "CD"){
      editValue("addition");
      log("Value:" + valueStr);
    } else if (keyStr == "CN") {
      editValue("addition");
      log("Value:" + valueStr);
    }
  } else if (displayScreen == "testOutput"){
    if (testOutputStep == maxTestOutputStep){
      testOutputStep = 0;
      hienThiTestOutput = true;
    } else {
      testOutputStep ++;
      hienThiTestOutput = true;
    }
  } else if (displayScreen == "screenTestMode"){
    if (testModeStep < maxTestModeStep){
      testModeStep ++;
    } else {
      testModeStep = 0;
    }
    chayTestMode = true;
    showText("TEST MODE", String("Step " + String(testModeStep)).c_str());
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
    if (keyStr == "CD"){
      editValue("subtraction");
      log("Value:" + valueStr);
    } else if (keyStr == "CN"){
      editValue("subtraction");
      log("Value:" + valueStr);
    }
  } else if (displayScreen == "testOutput"){
    if (testOutputStep == 0){
      testOutputStep = maxTestOutputStep;
      hienThiTestOutput = true;
    } else {
      testOutputStep --;
      hienThiTestOutput = true;
    }
  } else if (displayScreen == "screenTestMode"){
    if (testModeStep > 0){
      /*testModeStep --;
      chayTestMode = true;
      showText("TEST MODE", String("Step " + String(testModeStep)).c_str());*/
    }
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

//KHAI BÁO CHÂN IO Ở ĐÂY

const int sensorCount = 17;
const int sensorFabric = 16;
const int outRelayCut = 25;
const int outRelayAir = 26;

//KHAI BÁO THÔNG SỐ TRƯƠNG TRÌNH

byte cheDoHoatDong = 1;
int thoiGianNhaDao = 200;
int soDuMuiDauVao = 10;
int soDuMuiDauRa = 20;
byte cheDoThoiHoi = 1;
int thoiGianThoiHoiDauVao = 1000;
int thoiGianThoiHoiDauRa = 3000;
int thoiGianThoiHoiKhiChay = 5000;
int soMuiChongNhieu = 2;

int muiChiChuKiTruoc =0;

// TỔNG HỢP THAM SỐ Ghi Nhớ

unsigned long soMuiChiTrongChuKi = 0;
unsigned long muiChiCuoiCungThayDoiTrangThai = 0;
unsigned long muiChiBatDauChuKi = 0;
unsigned long muiChiKetThucChuKi = 0;

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

//TRƯƠNG TRÌNH NGƯỜI DÙNG LẬP TRÌNH


void testMode(){
  switch (testModeStep){
  case 0:
    if(chayTestMode){
      maxTestModeStep = 2;
      chayTestMode = false;
    }
    break;

  case 1:
    if (chayTestMode){
      /* code */
    }
    break;
  case 2:
    /* code */
    break;
  default:
    /* code */
    break;
  }
}

void testInput(){
  static bool trangthaiCuoiIO1;
  if (digitalRead(17)!= trangthaiCuoiIO1){
    trangthaiCuoiIO1 = digitalRead(17);
    showText("IO 17" , String(trangthaiCuoiIO1).c_str());
  }
  static bool trangthaiCuoiIO2;
  if (digitalRead(16)!= trangthaiCuoiIO2){
    trangthaiCuoiIO2 = digitalRead(16);
    showText("IO 16" , String(trangthaiCuoiIO2).c_str());
  }
  static bool trangthaiCuoiIO3;
  if (digitalRead(4)!= trangthaiCuoiIO3){
    trangthaiCuoiIO3 = digitalRead(4);
    showText("IO 04" , String(trangthaiCuoiIO3).c_str());
  }
  static bool trangthaiCuoiIO4;
  if (digitalRead(0)!= trangthaiCuoiIO4){
    trangthaiCuoiIO4 = digitalRead(0);
    showText("IO 00" , String(trangthaiCuoiIO4).c_str());
  }
  static bool trangthaiCuoiIO5;
  if (digitalRead(2)!= trangthaiCuoiIO5){
    trangthaiCuoiIO5 = digitalRead(2);
    showText("IO 02" , String(trangthaiCuoiIO5).c_str());
  }
  static bool trangthaiCuoiIO6;
  if (digitalRead(15)!= trangthaiCuoiIO6){
    trangthaiCuoiIO6 = digitalRead(15);
    showText("IO 15" , String(trangthaiCuoiIO6).c_str());
  }
}
void testOutput(){
  switch (testOutputStep){
    case 0:
      if (hienThiTestOutput){
        maxTestOutputStep = 3;
        bool tinHieuHienTai = digitalRead(25);
        showText("IO 25", String(tinHieuHienTai).c_str());
        hienThiTestOutput = false;
      } else if (daoTinHieuOutput){
        bool tinHieuHienTai = digitalRead(25);
        digitalWrite(25,!tinHieuHienTai);
        hienThiTestOutput = true;
        daoTinHieuOutput = false;
      }
      break;
    case 1:
      if (hienThiTestOutput){
        bool tinHieuHienTai = digitalRead(26);
        showText("IO 26", String(tinHieuHienTai).c_str());
        hienThiTestOutput = false;
      } else if (daoTinHieuOutput){
        bool tinHieuHienTai = digitalRead(26);
        digitalWrite(26,!tinHieuHienTai);
        hienThiTestOutput = true;
        daoTinHieuOutput = false;
      }
      break;
    case 2:
      if (hienThiTestOutput){
        bool tinHieuHienTai = digitalRead(27);
        showText("IO 27", String(tinHieuHienTai).c_str());
        hienThiTestOutput = false;
      } else if (daoTinHieuOutput){
        bool tinHieuHienTai = digitalRead(27);
        digitalWrite(27,!tinHieuHienTai);
        hienThiTestOutput = true;
        daoTinHieuOutput = false;
      }
      break;
    case 3:
      if (hienThiTestOutput){
        bool tinHieuHienTai = digitalRead(14);
        showText("IO 14", String(tinHieuHienTai).c_str());
        hienThiTestOutput = false;
      } else if (daoTinHieuOutput){
        bool tinHieuHienTai = digitalRead(14);
        digitalWrite(14,!tinHieuHienTai);
        hienThiTestOutput = true;
        daoTinHieuOutput = false;
      }
      break;
    default:
      break;
  }
}

void TaskCountSensor(void *pvParameters) {
  while (1) {
    funcCountSensor();
    vTaskDelay(1); // Điều chỉnh độ trễ nếu cần
  }
}

void tinhToanCaiDat(){

}

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
}

void khoiDong(){
  lastStatusCountSensor = digitalRead(sensorCount);
  displayScreen = "index";
  showText("HELLO","ESP32-OPTION");
  mainStep = 0;
  trangThaiHoatDong = 0;
  loadSetup();
  tinhToanCaiDat();
  trangThaiHoatDong = 1;
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
      //log("Nhận vải");
      
    } else {
      trangThaiNhanVai = false;
      //log("Không có vải");
    }
    muiChiCuoiCungThayDoiTrangThai = soMuiChiTrongChuKi;
    //log("Mũi cuối thay đổi trạng thái là: " + String(muiChiCuoiCungThayDoiTrangThai));
    lastStatusFabricSensor = statusFabricSensor;
  }
}

/*void funcCountSensor(){
  bool statusCountSensor = digitalRead(sensorCount);
  if (statusCountSensor != lastStatusCountSensor){
    soMuiChiTrongChuKi ++;
    showProgress(soMuiChiTrongChuKi,muiChiCuoiCungThayDoiTrangThai,muiChiKetThucChuKi);
    if (statusCountSensor){
      
      //log("số mũi chỉ trong chu kì là: " + String(soMuiChiTrongChuKi));
      //log("bước là: " + String(mainStep));
      if (ketThucChuKi){
        muiChiKetThucChuKi ++;
        //log("đếm số mũi đầu ra là :" + String(muiChiKetThucChuKi));
      }
      if (cheDoThoiHoi == 4 && !thoiHoiFull){
        thoiHoiFull = true;
        funcKichHoatThoiHoi(thoiGianThoiHoiKhiChay);
        //log("thổi hơi full step");
      }
    } 
    lastStatusCountSensor = statusCountSensor; 
  }
}*/

void funcCountSensor(){
  bool statusCountSensor = digitalRead(sensorCount);
  if (statusCountSensor != lastStatusCountSensor) {
    soMuiChiTrongChuKi ++;
    lastStatusCountSensor = statusCountSensor;
  }
}

void funcKetThucChuKy(){
  if (ketThucChuKi) {
    if (soMuiChiTrongChuKi - muiChiKetThucChuKi > soDuMuiDauRa){
        if (cheDoHoatDong == 1 || cheDoHoatDong == 3 ){
          if (!catDauRaChuKi){
              funcKichHoatDaoCat();
              //log("Cắt đầu ra");
              catDauRaChuKi = true;
          }
        } else {
          catDauRaChuKi = true;
        }
      ketThucChuKi = false;
    }
  }
}

void mainRun(){
  switch (mainStep) {
    case 0:
      if (soMuiChiTrongChuKi - muiChiCuoiCungThayDoiTrangThai > soMuiChongNhieu && trangThaiNhanVai){
        mainStep ++;
        muiChiBatDauChuKi = soMuiChiTrongChuKi;
        //soMuiChiTrongChuKi = 0;
        //log("Khởi tạo chu kì");
        //log("bước là: " + String(mainStep)); 
      }
      break;
    case 1: 
      if (cheDoHoatDong == 1 || cheDoHoatDong == 2 ) {
        if (!catDauVaoChuKi){
          if (soMuiChiTrongChuKi - muiChiCuoiCungThayDoiTrangThai > soDuMuiDauVao){
            if (!ketThucChuKi && trangThaiNhanVai){
              funcKichHoatDaoCat();
              //log("Cắt đầu vào");
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
          //log("Thổi đầu vào");
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
            //log("Thổi đầu ra");
            thoiDauRaChuKi = true;
          }
        } else {
          thoiDauRaChuKi = true;
        }
        catDauVaoChuKi = false;
        catDauRaChuKi = false;
        thoiDauVaoChuKi = false;
        thoiDauRaChuKi = false;
        ketThucChuKi = true;
        muiChiKetThucChuKi = soMuiChiTrongChuKi;
        muiChiChuKiTruoc = muiChiKetThucChuKi - muiChiBatDauChuKi;
        mainStep = 0;
      }
      break;
  }
  static unsigned long muiCuoiHienThi;
  if (muiCuoiHienThi != soMuiChiTrongChuKi){
    if (cheDoThoiHoi == 4){
      if (!thoiHoiFull){
        thoiHoiFull = true;
        funcKichHoatThoiHoi(thoiGianThoiHoiKhiChay);
      } else {
        thoiDiemCuoiThoiHoi = millis();
      }
    }
    showProgress(soMuiChiTrongChuKi - muiChiCuoiCungThayDoiTrangThai,muiChiChuKiTruoc,trangThaiNhanVai);
    muiCuoiHienThi = soMuiChiTrongChuKi;
  }
  funcKetThucChuKy();
  funcBlowAir();
  funcCut();
}



void setup() {

  Serial.begin(115200);     // Khởi tạo Serial và màn hình

  u8g2.begin();  // Khởi tạo màn hình OLED
  u8g2.enableUTF8Print(); // Kích hoạt hỗ trợ UTF-8

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
  pinMode(4, INPUT);
  pinMode(0, INPUT);
  pinMode(2, INPUT);
  pinMode(5, INPUT);


  pinMode(outRelayCut, OUTPUT);
  pinMode(outRelayAir, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  digitalWrite(outRelayCut,LOW);
  digitalWrite(outRelayAir,LOW);


  if (!LittleFS.begin()) {
    showSetup("Error", "E003", "LittleFS Mount Failed");
    Serial.println("LittleFS Mount Failed");
    return;
  }

  

  // Kiểm tra xem file config.json có tồn tại không
  if (!LittleFS.exists(configFile)) {
    DeserializationError error = deserializeJson(jsonDoc, jsonString); // Phân tích chuỗi JSON
    if (error) {
        showSetup("Error", "E005", "JsonString Error");
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    showSetup("Error", "E007", "JsonString Load Mode");
    Serial.println("Read Data From JsonString");
    loadSetup();
    Serial.println("File config.json does not exist.");
    return;
  }

  readConfigFile();

  Serial.println("Load toàn bộ dữ liệu thành công");
  khoiDong();

  xTaskCreatePinnedToCore(TaskCountSensor, "TaskCountSensor", 10000, NULL, 2, &TaskCountSensorHandle, 0); // Task ưu tiên 2
}

void loop() {
  switch (trangThaiHoatDong){
  case 0:
    btnMenu.tick();
    btnSet.tick();
    btnUp.tick();
    btnDown.tick();
    break;
  case 1:
    btnMenu.tick();
    funcFabricSensor();
    //funcCountSensor();
    mainRun();
    break;
  case 2:
    
    break;
  case 200:        //ESTOP dừng khẩn cấp
    btnMenu.tick();
    break;
  case 201:         // Func Test Mode
    btnMenu.tick();
    btnUp.tick();
    btnDown.tick();
    testMode();
    break;
  case 202:        // Func Test Input
    btnMenu.tick();
    testInput();
    break;
  case 203:      // Func Test Output
    btnMenu.tick();
    btnSet.tick();
    btnUp.tick();
    btnDown.tick();
    testOutput();
    break;
  case 204:
    handleOTA(); // Xử lý OTA khi điều kiện đúng
    break;  
  default:
    break;
  }
}
