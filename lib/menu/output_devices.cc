#include "output_devices.h"

#include <Arduino.h>
#include <Wire.h>

const std::vector<uint8_t> INIT_COMMANDS {
  // Set colour to red
  0x7C, // Setting mode
  '+', // Set colour
  255, 0, 0, // RGB

  // Set to two lines
  0x7C, // Setting mode
  0x06, // 2 lines
};

uint8_t OutputDevices::lcdAddress;
std::string OutputDevices::lastString;
bool OutputDevices::updatedSettings;
std::vector<uint8_t> OutputDevices::settings;

void OutputDevices::initLcd(uint8_t lcdAddress) {
  OutputDevices::lcdAddress = lcdAddress;
  OutputDevices::lastString = "";
  OutputDevices::settings = {};
  OutputDevices::updatedSettings = false;

  sendToLcd(INIT_COMMANDS);
  disableCursor();
}

void OutputDevices::display(std::string str) {
  if (str == lastString && !updatedSettings) {
    return;
  }
  std::vector<uint8_t> data {
    0x7C, // Setting mode
    0x2D  // Clear and home
  };
  data.insert(data.end(), str.begin(), str.end());
  sendToLcd(data);
  sendToLcd(settings);
  lastString = str;
  updatedSettings = false;
}

void OutputDevices::setCursor(uint8_t row, uint8_t column) {
  char ROW_OFFSETS[4] = {0x00, 0x40, 0x14, 0x54};
  settings = {
    254, // Special mode
    0x08 | 0x04 | 0x02, // Display control: Display on, Cursor on
    254, // Special mode
    (uint8_t) (0x80 | (uint8_t) (ROW_OFFSETS[row] + column)) // Set cursor
  };
  updatedSettings = true;
}

void OutputDevices::disableCursor() {
  settings = {
    254, // Special mode
    0x08 | 0x04, // Display control: Display on, Cursor off
  };
  updatedSettings = true;
}

void OutputDevices::sendToLcd(std::vector<uint8_t> data) {
  Wire.beginTransmission(OutputDevices::lcdAddress);
  Wire.write(&data[0], data.size());
  Wire.endTransmission();
}
