#ifndef COSMIC_SIGNPOST_LIB_MAIN_GPS_H_
#define COSMIC_SIGNPOST_LIB_MAIN_GPS_H_

#include <functional>
#include <optional>

#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

#include "location.h"
#include "time_utils.h"

namespace gps {
  extern SFE_UBLOX_GNSS receiver;
  extern bool connected;
  extern bool enabled;
  extern TimeMillisMicros lastUpdateTime;
  extern std::function<void(Location)> updateFunction;

  void initGps(std::function<void(Location)> updateFunction);
  void setEnabled(bool newEnabled);
  void checkForUpdates();
  std::optional<Location> getCurrentLocation(UBX_NAV_PVT_data_t *data);
  bool hasFix(UBX_NAV_PVT_data_t *data);
  void processPvt(UBX_NAV_PVT_data_t *data);
};

#endif
