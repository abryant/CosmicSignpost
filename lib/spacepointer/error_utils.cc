#include "error_utils.h"

#include <iostream>
#include <string>

#ifdef ARDUINO

#include <Arduino.h>

void checkArgumentOrReset(bool check, std::string errorMessage) {
  if (!check) {
    std::cerr << errorMessage << std::endl;
    delay(10000);
    std::cerr << "Restarting..." << std::endl;
    ESP.restart();
  }
}

#else

void checkArgumentOrReset(bool check, std::string errorMessage) {
  if (!check) {
    std::cerr << errorMessage << std::endl;
    abort();
  }
}

#endif
