#include "error_utils.h"

#include <iostream>
#include <string>

void checkArgument(bool check, std::string errorMessage) {
  if (!check) {
    failWithError(errorMessage);
  }
}

#ifdef ARDUINO

#include <Arduino.h>

void failWithError(std::string errorMessage) {
  std::cerr << errorMessage << std::endl;
  delay(10000);
  std::cerr << "Restarting..." << std::endl;
  ESP.restart();
}

#else

void failWithError(std::string errorMessage) {
  std::cerr << errorMessage << std::endl;
  abort();
}

#endif
