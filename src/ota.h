#ifndef OTA_H
#define OTA_H

#include <ArduinoOTA.h>
#include <U8g2lib.h>

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

void setupOTA();
void handleOTA();
void setupWebServer();

#endif // OTA_H
