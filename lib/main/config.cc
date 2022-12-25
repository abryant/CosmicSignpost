#include "config.h"

#include <sstream>

#include "SPIFFS.h"
#include "FS.h"

const std::string DEFAULT_LOCATION_FILENAME = "default_location.txt";

bool config::initialised = false;

Location config::readDefaultLocation() {
  std::istringstream ss(readFile(DEFAULT_LOCATION_FILENAME));
  double latitude, longitude, elevation;
  ss >> latitude;
  if (ss.fail()) {
    return Location(0, 0, 0);
  }
  ss.ignore(1, ',');
  ss >> longitude;
  if (ss.fail()) {
    return Location(0, 0, 0);
  }
  ss.ignore(1, ',');
  ss >> elevation;
  if (ss.fail()) {
    return Location(0, 0, 0);
  }
  return Location(latitude, longitude, elevation);
}

void config::writeDefaultLocation(Location location) {
  std::ostringstream ss;
  ss << location.getLatitude() << "," << location.getLongitude() << "," << location.getElevation() << "\n";
  writeFile(DEFAULT_LOCATION_FILENAME, ss.str());
}

std::string config::readFile(std::string filename) {
  filename = "/" + filename;
  initialise();
  File configFile = SPIFFS.open(filename.c_str(), FILE_READ);
  String contents = configFile.readString();
  configFile.close();
  return std::string(contents.c_str());
}

void config::writeFile(std::string filename, std::string contents) {
  filename = "/" + filename;
  initialise();
  File configFile = SPIFFS.open(filename.c_str(), FILE_WRITE);
  configFile.write((const uint8_t*) contents.c_str(), contents.length());
  configFile.close();
}

void config::initialise() {
  if (initialised) {
    return;
  }
  SPIFFS.begin(/* formatOnFail= */ true);
  initialised = true;
}
