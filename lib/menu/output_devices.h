#ifndef SPACEPOINTER_LIB_MENU_OUTPUT_DEVICES_H_
#define SPACEPOINTER_LIB_MENU_OUTPUT_DEVICES_H_

#include <stdint.h>
#include <string>
#include <vector>

namespace OutputDevices {
  extern uint8_t lcdAddress;
  extern std::string lastString;
  extern std::vector<uint8_t> settings;

  void initLcd(uint8_t lcdAddress);
  void display(std::string text);

  void disableSystemMessages();
  void setTwoLines();
  void setCursor(uint8_t row, uint8_t column);
  void disableCursor();
  void displayAndSetSplashScreen(std::string text);
  void setBacklightColour(uint8_t red, uint8_t green, uint8_t blue);

  void sendToLcd(std::vector<uint8_t> data);
};

#endif
