#ifndef SPACEPOINTER_SRC_OTA_H_
#define SPACEPOINTER_SRC_OTA_H_

#include <stdlib.h>

#include <string>

namespace ota {
  void setUp(int32_t port, std::string password);
  void checkForOta();
};

#endif
