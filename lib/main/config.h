#ifndef COSMIC_SIGNPOST_LIB_MAIN_CONFIG_H_
#define COSMIC_SIGNPOST_LIB_MAIN_CONFIG_H_

#include <string>

#include "location.h"

namespace config {

  Location readDefaultLocation();
  void writeDefaultLocation(Location location);

  std::string readFile(std::string filename);
  void writeFile(std::string filename, std::string contents);

  extern bool initialised;
  void initialise();
};

#endif
