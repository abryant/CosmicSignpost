#include "output_devices.h"

#include <Arduino.h>
#include <Wire.h>

uint8_t OutputDevices::lcdAddress;
std::string OutputDevices::lastString;
std::vector<uint8_t> OutputDevices::settings;

void OutputDevices::initLcd(uint8_t lcdAddress) {
  OutputDevices::lcdAddress = lcdAddress;
  OutputDevices::lastString = "";
  OutputDevices::settings = {};

  setTwoLines();
  disableCursor();
  setBacklightColour(255, 0, 0);
}

void OutputDevices::display(std::string str) {
  if (str == lastString && settings.size() == 0) {
    return;
  }
  if (settings.size() > 0) {
    sendToLcd(settings);
    settings = {};
  }
  std::vector<uint8_t> data {
    0x7C, // Setting mode
    0x2D  // Clear and home
  };
  data.insert(data.end(), str.begin(), str.end());
  sendToLcd(data);
  lastString = str;
}

void OutputDevices::setTwoLines() {
  std::vector<uint8_t> twoLinesSettings = {
    0x7C, // Setting mode
    0x06, // 2 lines
  };
  settings.insert(settings.end(), twoLinesSettings.begin(), twoLinesSettings.end());
}

void OutputDevices::setCursor(uint8_t row, uint8_t column) {
  uint8_t ROW_OFFSETS[4] = {0x00, 0x40, 0x14, 0x54};
  std::vector<uint8_t> cursorSettings = {
    254, // Special mode
    0x08 | 0x04 | 0x02, // Display control: Display on, Cursor on
    254, // Special mode
    (uint8_t) (0x80 | (uint8_t) (ROW_OFFSETS[row] + column)) // Set cursor
  };
  settings.insert(settings.end(), cursorSettings.begin(), cursorSettings.end());
}

void OutputDevices::disableCursor() {
  std::vector<uint8_t> cursorSettings = {
    254, // Special mode
    0x08 | 0x04, // Display control: Display on, Cursor off
  };
  settings.insert(settings.end(), cursorSettings.begin(), cursorSettings.end());
}

void OutputDevices::setBacklightColour(uint8_t red, uint8_t green, uint8_t blue) {
  std::vector<uint8_t> colourSettings = {
    0x7C, // Setting mode
    '+', // Set colour
    red, green, blue
  };
  settings.insert(settings.end(), colourSettings.begin(), colourSettings.end());
}

void OutputDevices::sendToLcd(std::vector<uint8_t> data) {
  Wire.beginTransmission(OutputDevices::lcdAddress);
  Wire.write(&data[0], data.size());
  Wire.endTransmission();
}
