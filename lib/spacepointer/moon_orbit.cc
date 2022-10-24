#include "moon_orbit.h"

#include <cmath>
#include <cstdint>

#include "angle_utils.h"
#include "cartesian_location.h"
#include "keplerian_orbit.h"
#include "time_utils.h"

double EARTH_MASS = 5.97219e24; // kg
double MOON_MASS = 7.349e22; // kg
double EARTH_DISTANCE_SCALE = MOON_MASS / (EARTH_MASS + MOON_MASS);
double MOON_DISTANCE_SCALE = EARTH_MASS / (EARTH_MASS + MOON_MASS);

// Frequencies, amplitudes, and phases of oscillations in orbital parameters, based on orbital
// element datasets from JPL horizons:
//
// https://ssd.jpl.nasa.gov/horizons/app.html
// Ephemeris Type: Osculating Orbital Elements
// Target Body: Moon [Luna]
// Coordinate Center: Geocentric [code: 500]
// Time Specification: Start=2000-01-01 TDB, Stop=2044-11-08, Step=1 (days)
//   (this gives 16384 data points, a power of 2, which is useful for FFTs)
// Output units: km and days
//
// Each element has the top 8 frequency components listed. The frequency bounds were isolated using
// fourier transforms, and the frequencies, amplitudes, and phases were optimised using curve
// fitting with scipy.
//
// The curve-fitting approximation function used equations like (for a time parameter d):
//   approx_value =
//       mean
//       + (linear_movement * d)
//       + sum([A_s[i] * sin(d * freq[i] * 2 * PI) + A_c[i] * cos(d * freq[i] * 2 * PI)])
//
// This gives the optimisation process two independent sin and cos parameters to vary, rather than
// directly altering the phase. The final amplitudes and phases were derived using:
//   amplitude = sqrt(A_s * A_s + A_c * A_c)
//   phase = atan2(A_s, A_c)

// Frequencies are measured in full cycles/day.
double SEMI_MAJOR_AXIS_FREQUENCIES[] = {0.06772638349, 0.03143472835, 0.06498858223, 0.03629166334, 0.1040180432, 0.07046408247, 0.02869694247, 0.0991610887};
double ECCENTRICITY_FREQUENCIES[] = {0.03143473373, 0.004856918063, 0.1040180729, 0.03629167405, 0.06286949647, 0.02869703424, 0.02657779864, 0.067726415};
double INCLINATION_FREQUENCIES[] = {0.005770004324, 0.07349638589, 0.06772640071, 0.0009130410138, 0.008507841671, 0.0001466503767, 0.03143483876, 0.00303243609};
double LONG_ASC_NODE_FREQUENCIES[] = {0.00576999357, 0.002737620551, 0.07349638524, 0.06772640887, 0.0009136893178, 0.008507759005, 0.0001471483624, 0.03143459637};
double ARG_PERIAPSIS_FREQUENCIES[] = {0.03143475612, 0.004856936128, 0.02657796095, 0.03629171247, 0.06286953077, 0.00576996744, 0.1040181049, 0.009713612633};
double MEAN_ANOMALY_FREQUENCIES[] = {0.03143473152, 0.004856933366, 0.02657779206, 0.03629167167, 0.06286944694, 0.1040181084, 0.009713654448, 0.02869717196};

// Amplitudes are measured in units of each individual component.
double SEMI_MAJOR_AXIS_AMPLITUDES[] = {3400.506464, 635.496481, 218.0140472, 235.5330406, 181.0128617, 38.27166298, 39.81318227, 33.75273401};
double ECCENTRICITY_AMPLITUDES[] = {0.01421641992, 0.008549618872, 0.001356584749, 0.001383179429, 0.0009139492834, 0.0008680749145, 0.001147372626, 0.0006255931723};
double INCLINATION_AMPLITUDES[] = {0.1350670446, 0.0104210706, 0.0111493115, 0.007160668345, 0.005544635081, 0.00550205681, 0.00399417967, 0.002909178423};
double LONG_ASC_NODE_AMPLITUDES[] = {1.497892577, 0.1495745118, 0.117643539, 0.1226676502, 0.08039557106, 0.06151440593, 0.05657740791, 0.04896894511};
double ARG_PERIAPSIS_AMPLITUDES[] = {15.49853717, 9.656242965, 2.613289426, 2.752976113, 2.090104007, 1.498474386, 1.485001486, 0.9729725974};
double MEAN_ANOMALY_AMPLITUDES[] = {15.78244015, 9.688997916, 2.608721454, 2.831755234, 2.087637974, 1.511231558, 0.9725794168, 0.9721823331};

// Phases are measured in radians.
double SEMI_MAJOR_AXIS_PHASES[] = {-0.5984661711, 0.1880016571, -0.5543994762, 0.7839019075, 1.75634865, 2.50377339, 0.2328002838, 1.161410278};
double ECCENTRICITY_PHASES[] = {-2.953908914, 2.167388574, 1.754151193, 0.7824298317, 1.945561831, -2.914994875, -0.4081489954, 2.542342191};
double INCLINATION_PHASES[] = {0.7127635476, -1.456695712, 2.542230993, 0.1159413536, 0.6673081066, 2.599032877, -2.961336747, -2.398391785};
double LONG_ASC_NODE_PHASES[] = {-0.8574885622, 3.11394799, -3.027332147, 0.9708366929, -1.482928014, -0.9000677385, 0.9755938314, 1.763148884};
double ARG_PERIAPSIS_PHASES[] = {-1.384350906, 0.5961669064, 1.154308757, -0.7895320081, -2.769590518, 2.286336451, 0.1822568472, -1.936642031};
double MEAN_ANOMALY_PHASES[] = {1.758550465, -2.545351399, -1.978996698, 2.354253871, 0.3764641201, -2.95919172, 1.202392777, 1.790234857};

double applyOscillations(double d, double frequencies[], double amplitudes[], double phases[]) {
  double result = 0;
  for (int i = 0; i < 8; ++i) {
    result += amplitudes[i] * std::sin((d * frequencies[i] * 2 * M_PI) + phases[i]);
  }
  return result;
}

CartesianLocation MoonOrbit::positionAt(int64_t timeMillis) {
  double timeJulianDaysSinceJ2000 = daysSinceJ2000(timeMillis);
  double d = timeJulianDaysSinceJ2000;
  double semiMajorAxisKilometres =
      383397.78344354825
      + applyOscillations(
          d, SEMI_MAJOR_AXIS_FREQUENCIES, SEMI_MAJOR_AXIS_AMPLITUDES, SEMI_MAJOR_AXIS_PHASES);
  double eccentricity =
      0.055543125288357496
      + applyOscillations(
          d, ECCENTRICITY_FREQUENCIES, ECCENTRICITY_AMPLITUDES, ECCENTRICITY_PHASES);
  double inclinationDegrees =
      5.1567510400046395
      + applyOscillations(
          d, INCLINATION_FREQUENCIES, INCLINATION_AMPLITUDES, INCLINATION_PHASES);
  double longitudeOfAscendingNodeDegrees =
      125.03400862998522
      + (-0.05299064864418411 * d)
      + applyOscillations(
          d, LONG_ASC_NODE_FREQUENCIES, LONG_ASC_NODE_AMPLITUDES, LONG_ASC_NODE_PHASES);
  double argumentOfPeriapsisDegrees =
      318.3451737393957
      + (0.1643518821113679 * d)
      + applyOscillations(
          d, ARG_PERIAPSIS_FREQUENCIES, ARG_PERIAPSIS_AMPLITUDES, ARG_PERIAPSIS_PHASES);
  double meanAnomalyDegrees =
      134.93566650328088
      + (13.06499687529952 * d)
      + applyOscillations(d, MEAN_ANOMALY_FREQUENCIES, MEAN_ANOMALY_AMPLITUDES, MEAN_ANOMALY_PHASES);

  CartesianLocation moonFromEarth =
      KeplerianOrbit::findPosition(
          ReferenceFrame::EARTH_ECLIPTIC,
          semiMajorAxisKilometres * 1000,
          eccentricity,
          degreesToRadians(wrapDegrees(inclinationDegrees)),
          degreesToRadians(wrapDegrees(longitudeOfAscendingNodeDegrees)),
          degreesToRadians(wrapDegrees(argumentOfPeriapsisDegrees)),
          degreesToRadians(wrapDegrees(meanAnomalyDegrees)));
  return moonFromEarth;
}

CartesianLocation MoonOrbit::earthMoonBarycentreAt(int64_t timeMillis) {
  CartesianLocation moonLocationFromEarth = positionAt(timeMillis);
  return CartesianLocation(
      moonLocationFromEarth.position * EARTH_DISTANCE_SCALE,
      ReferenceFrame::EARTH_ECLIPTIC);
}
