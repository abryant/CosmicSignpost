#ifndef SPACEPOINTER_LIB_MENU_OUTPUT_DEVICES_H_
#define SPACEPOINTER_LIB_MENU_OUTPUT_DEVICES_H_

#include <stdint.h>
#include <string>
#include <vector>

namespace OutputDevices {
  extern uint8_t lcdAddress;

  void initLcd(uint8_t lcdAddress);
  void display(std::string text);

  void sendToLcd(std::vector<uint8_t> data);
};

#endif
