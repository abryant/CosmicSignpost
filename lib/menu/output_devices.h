#ifndef SPACEPOINTER_LIB_MENU_OUTPUT_DEVICES_H_
#define SPACEPOINTER_LIB_MENU_OUTPUT_DEVICES_H_

#include <stdint.h>
#include <string>
#include <vector>

namespace OutputDevices {
  extern uint8_t lcdAddress;
  extern std::string lastString;
  extern std::vector<uint8_t> settings;
  extern bool updatedSettings;

  void initLcd(uint8_t lcdAddress);
  void display(std::string text);
  void setCursor(uint8_t row, uint8_t column);
  void disableCursor();

  void sendToLcd(std::vector<uint8_t> data);
};

#endif
