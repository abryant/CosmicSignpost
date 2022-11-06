#ifndef SPACEPOINTER_LIB_SPACEPOINTER_SATELLITE_ORBIT_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_SATELLITE_ORBIT_H_

#include <stdint.h>
#include <string>
#include <optional>
#include <functional>

#include "cartesian_location.h"
#include "omm_message.h"
#include "sgp4_state.h"

class SatelliteOrbit {
  public:
    SatelliteOrbit(std::string catalogNumber);
    bool fetchElements(std::function<std::optional<std::string>(std::string)> urlFetchFunction);
    CartesianLocation toCartesian(int64_t timeMillis);

  private:
    std::string catalogNumber;
    std::optional<OmmMessage> ommMessage;
    std::optional<SGP4::Sgp4State> sgp4PropagatorState;
};

#endif
