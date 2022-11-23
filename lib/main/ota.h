#ifndef COSMIC_SIGNPOST_LIB_MAIN_OTA_H_
#define COSMIC_SIGNPOST_LIB_MAIN_OTA_H_

#include <stdlib.h>

#include <string>

namespace ota {
  void setUp(int32_t port, std::string password);
  void checkForOta();
};

#endif
