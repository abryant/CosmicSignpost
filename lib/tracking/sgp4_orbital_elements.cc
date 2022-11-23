#include "sgp4_orbital_elements.h"

#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "angle_utils.h"
#include "time_utils.h"

const double MINUTES_PER_DAY = 24 * 60; // 1440

SGP4::Sgp4OrbitalElements::Sgp4OrbitalElements(OmmMessage omm) {
  name = omm.objectName;
  int64_t timeMillisSinceUnixEpoch = parseDateTimeToUnixMillis(omm.epoch);
  double julianDaysSinceUnixEpoch = millisToJulianDays(timeMillisSinceUnixEpoch);
  double julianDaysSince0thJanuary1950 =
      UNIX_EPOCH_JULIAN_DATE + julianDaysSinceUnixEpoch - JAN_0_1950_JULIAN_DATE;
  epoch = julianDaysSince0thJanuary1950;

  meanMotionRevsPerDay = omm.meanMotion;
  meanMotion = omm.meanMotion / (MINUTES_PER_DAY / (2 * M_PI));
  inclinationRadians = degreesToRadians(omm.inclination);
  rightAscensionOfAscendingNodeRadians = degreesToRadians(omm.rightAscensionOfAscendingNode);
  argumentOfPeriapsisRadians = degreesToRadians(omm.argumentOfPericenter);
  meanAnomalyRadians = degreesToRadians(omm.meanAnomaly);
  eccentricity = omm.eccentricity;
  bStarDragCoefficient = omm.bStarDragCoefficient;
}
