#ifndef SPACEPOINTER_LIB_TRACKING_SATELLITE_ORBIT_H_
#define SPACEPOINTER_LIB_TRACKING_SATELLITE_ORBIT_H_

#include <stdint.h>
#include <string>
#include <optional>
#include <functional>

#include "cartesian_location.h"
#include "omm_message.h"
#include "sgp4_orbital_elements.h"
#include "sgp4_state.h"

class SatelliteOrbit {
  public:
    SatelliteOrbit(std::string catalogNumber);
    bool fetchElements(std::function<std::optional<std::string>(std::string)> urlFetchFunction);
    CartesianLocation toCartesian(int64_t timeMillis);
    std::string getCatalogNumber();
    std::string getName();
    double getOrbitalPeriodSeconds();
    bool hasOrbitalElements();

  private:
    std::string catalogNumber;
    std::optional<SGP4::Sgp4OrbitalElements> sgp4OrbitalElements;

    // The ESP32 doesn't have enough memory to store an Sgp4State for every satellite it knows
    // about, so we only store the one we're currently tracking, identified by catalog number.
    // This is not thread-safe, as the SatelliteOrbit functions should only be used by one thread.
    static std::string currentCatalogNumber;
    static std::optional<SGP4::Sgp4State> currentSgp4State;
};

#endif
