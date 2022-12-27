#include "gps.h"

#include "Arduino.h"

const int16_t GNSS_MAX_WAIT = 250;

SFE_UBLOX_GNSS gps::receiver;
bool gps::connected = false;
bool gps::enabled = true;
TimeMillisMicros gps::lastUpdateTime;
std::function<void(Location)> gps::updateFunction;
std::function<void(bool)> gps::connectionStatusFunction;

void gps::processPvt(UBX_NAV_PVT_data_t *data) {
  if (enabled && updateFunction) {
    std::optional<Location> newLocation = getCurrentLocation(data);
    if (newLocation.has_value()) {
      updateFunction(newLocation.value());
    }
  }
}

void gps::initGps(std::function<void(Location)> updateFunction, std::function<void(bool)> connectionStatusFunction) {
  connected = receiver.begin();
  gps::updateFunction = updateFunction;
  gps::connectionStatusFunction = connectionStatusFunction;
  if (connected) {
    Serial.println("GPS connected at startup. Setting frequency to 1Hz");
    receiver.setNavigationFrequency(1, GNSS_MAX_WAIT);
    bool setupAuto = receiver.setAutoPVTcallbackPtr(&processPvt, GNSS_MAX_WAIT);
    if (!setupAuto) {
      Serial.println("Failed to set up automatic PVT");
    }
  } else {
    Serial.println("No GPS at startup.");
  }
  connectionStatusFunction(connected);
  lastUpdateTime = TimeMillisMicros::now();
}

void gps::setEnabled(bool newEnabled) {
  gps::enabled = newEnabled;
}

void gps::checkForUpdates() {
  TimeMillisMicros now = TimeMillisMicros::now();
  if (lastUpdateTime.plusMicros(1000000) < now) {
    bool checkResult = receiver.checkUblox(GNSS_MAX_WAIT);
    if (checkResult) {
      receiver.checkCallbacks();
    }

    bool newConnected = true;
    if (!checkResult) {
      // checkResult being false can also mean we didn't receive any data, so in that case we need
      // a separate connectivity check.
      newConnected = receiver.isConnected(GNSS_MAX_WAIT);
    }
    if (newConnected && !connected) {
      Serial.println("GPS connected. Setting frequency to 1Hz");
      receiver.setNavigationFrequency(1, GNSS_MAX_WAIT);
      receiver.setAutoPVTcallbackPtr(&processPvt, GNSS_MAX_WAIT);
    }
    if (newConnected != connected) {
      connectionStatusFunction(newConnected);
      connected = newConnected;
    }
    lastUpdateTime = TimeMillisMicros::now();
  }
}

std::optional<Location> gps::getCurrentLocation(UBX_NAV_PVT_data_t *data) {
  if (!hasFix(data)) {
    return std::nullopt;
  }
  int32_t rawLatitude = data->lat;
  int32_t rawLongitude = data->lon;
  int32_t rawAltitude = data->height;

  double latitudeDegrees = rawLatitude / 1.0e7;
  double longitudeDegrees = rawLongitude / 1.0e7;
  double altitudeMetres = rawAltitude / 1.0e3;
  return Location(latitudeDegrees, longitudeDegrees, altitudeMetres);
}

bool gps::hasFix(UBX_NAV_PVT_data_t *data) {
  int8_t fixType = data->fixType;
  int32_t satsInView = data->numSV;
  Serial.printf("Fix type: %d, sats=%d\n", fixType, satsInView);
  if (fixType == 0) {
    // No fix
  } else if (fixType == 1) {
    // Dead reckoning
  } else if (fixType == 2) {
    // 2D
  } else if (fixType == 3) {
    // 3D
    return true;
  } else if (fixType == 4) {
    // GNSS
    return true;
  } else if (fixType == 5) {
    // Time only
  }
  return false;
}
