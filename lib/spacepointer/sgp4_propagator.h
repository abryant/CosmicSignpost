#ifndef SPACEPOINTER_LIB_SPACEPOINTER_SGP4_PROPAGATOR_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_SGP4_PROPAGATOR_H_

#include "sgp4_orbital_elements.h"
#include "sgp4_state.h"

namespace SGP4 {
  enum class ResultCode {
    SUCCESS,
    BAD_MEAN_ELEMENTS,
    NEGATIVE_MEAN_MOTION,
    BAD_PERTURBATION_ELEMENTS,
    BAD_SEMILATUS_RECTUM,
    SUBORBITAL_ELEMENTS,
    SATELLITE_DECAYED,
  };

  // The version of the world geodetic system we are using.
  // See https://en.wikipedia.org/wiki/World_Geodetic_System
  enum class WgsVersion {
    WGS_72,
    WGS_84,
  };

  class Sgp4Result {
    public:
      ResultCode code;
      double x, y, z;
      double vx, vy, vz;
  };

  Sgp4GeodeticConstants getGravitationalConstants(WgsVersion wgsVersion);

  Sgp4State initialiseSgp4(
    WgsVersion wgsVersion,
    OperationMode operationMode,
    Sgp4OrbitalElements elements);
  void initl(Sgp4State &state, Sgp4InitState &initState);
  void dscom(Sgp4State &state, Sgp4InitState &initState);
  void dpper(const Sgp4State &state, Sgp4DpperOutputs &outputs);
  void dsinit(Sgp4State &state, Sgp4InitState &initState);
  void dspace(const Sgp4State &state, Sgp4DspaceOutputs &outputs);
  double greenwichSiderealTime(double julianDateUt1);

  double findTimeSinceEpochMinutes(const Sgp4State &state, int64_t timeUtcMillis);

  Sgp4Result runSgp4(Sgp4State &state, double timeSinceEpochMinutes);
}

#endif
