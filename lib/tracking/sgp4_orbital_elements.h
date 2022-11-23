#ifndef COSMIC_SIGNPOST_LIB_TRACKING_SGP4_ORBITALELEMENTS_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_SGP4_ORBITALELEMENTS_H_

#include <string>

#include "omm_message.h"

namespace SGP4 {

  class Sgp4OrbitalElements {
    public:
      std::string name;
      double epoch; // julian days since the 0th of January 1950 (i.e. 31st December 1949) at noon
      double meanMotionRevsPerDay;
      double meanMotion; // radians/minute
      double eccentricity; // of the ellipse in the orbital plane, 0 to 1
      double inclinationRadians;
      double rightAscensionOfAscendingNodeRadians;
      double argumentOfPeriapsisRadians;
      double meanAnomalyRadians;
      double bStarDragCoefficient; // units are 1 / earth-radii

      Sgp4OrbitalElements(OmmMessage omm);
  };

}

#endif
