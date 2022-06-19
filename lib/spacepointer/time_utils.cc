#include "time_utils.h"

#include <cstdint>

double daysSinceJ2000(int64_t unixTimeMillis) {
  int64_t timeSinceJ2000Millis = unixTimeMillis - J2000_UTC_MILLIS;
  return timeSinceJ2000Millis / 1000.0 / 60.0 / 60.0 / 24.0;
}
