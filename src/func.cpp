#include <Arduino.h>  // Bao gồm thư viện Arduino.h để sử dụng isDigit

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
