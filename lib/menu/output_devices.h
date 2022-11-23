#ifndef COSMIC_SIGNPOST_LIB_MENU_OUTPUT_DEVICES_H_
#define COSMIC_SIGNPOST_LIB_MENU_OUTPUT_DEVICES_H_

#include <stdint.h>
#include <string>
#include <vector>

namespace OutputDevices {
  extern const uint32_t DISPLAY_LENGTH;
  extern uint8_t lcdAddress;
  extern std::string lastString;
  extern std::vector<uint8_t> settings;

  void initLcd(uint8_t lcdAddress);
  void display(std::string text);

  void disableSystemMessages();
  void createUpDownArrows();
  void createDegreesSymbol();
  void setTwoLines();
  void setCursor(uint8_t row, uint8_t column);
  void disableCursor();
  void displayAndSetSplashScreen(std::string text);
  void setBacklightColour(uint8_t red, uint8_t green, uint8_t blue);
  int32_t countChars(std::string str);

  void sendToLcd(std::vector<uint8_t> data);
};

#endif
