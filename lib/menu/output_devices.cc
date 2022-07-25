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

void OutputDevices::initLcd(uint8_t lcdAddress) {
  OutputDevices::lcdAddress = lcdAddress;

  sendToLcd(INIT_COMMANDS);
}

void OutputDevices::display(std::string str) {
  std::vector<uint8_t> data {
    0x7C, // Setting mode
    0x2D  // Clear and home
  };
  data.insert(data.end(), str.begin(), str.end());
  sendToLcd(data);
}

void OutputDevices::sendToLcd(std::vector<uint8_t> data) {
  Wire.beginTransmission(OutputDevices::lcdAddress);
  Wire.write(&data[0], data.size());
  Wire.endTransmission();
}
