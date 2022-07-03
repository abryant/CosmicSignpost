#include "time_utils.h"

#include <cstdint>
#include <cstddef>

#include <sys/time.h>

double daysSinceJ2000(int64_t unixTimeMillis) {
  int64_t timeSinceJ2000Millis = unixTimeMillis - J2000_UTC_MILLIS;
  return timeSinceJ2000Millis / 1000.0 / 60.0 / 60.0 / 24.0;
}

TimeMillisMicros::TimeMillisMicros()
    : millis(0), micros(0) {}

TimeMillisMicros::TimeMillisMicros(int64_t millis, int32_t micros)
    : millis(millis), micros(micros) {}

TimeMillisMicros TimeMillisMicros::now() {
  struct timeval time_now;
  while (true) {
    int result = gettimeofday(&time_now, NULL);
    if (result == 0) {
      break;
    }
  }
  int64_t resultMillis = (((int64_t) time_now.tv_sec) * 1000) + (((int64_t) time_now.tv_usec) / 1000);
  int32_t resultMicros = time_now.tv_usec % 1000;
  return TimeMillisMicros(resultMillis, resultMicros);
}

TimeMillisMicros TimeMillisMicros::ofMicros(int32_t micros) {
  return TimeMillisMicros(0, micros);
}

TimeMillisMicros TimeMillisMicros::plusMicros(int32_t plusMicros) const {
  int32_t newMicros = micros + plusMicros;
  int32_t extraMillis = newMicros / 1000;
  return TimeMillisMicros(millis + extraMillis, newMicros % 1000);
}

int32_t positiveMod1000(int32_t micros) {
  int32_t result = micros % 1000;
  if (result < 0) {
    result += 1000;
  }
  return result;
}

TimeMillisMicros TimeMillisMicros::minusMicros(int32_t subtractedMicros) const {
  int32_t newMicros = micros - subtractedMicros;
  int32_t newMod1000 = positiveMod1000(newMicros);
  int64_t extraMillis = (newMicros - newMod1000) / 1000;
  return TimeMillisMicros(millis + extraMillis, newMod1000);
}

int64_t TimeMillisMicros::deltaMicrosSince(TimeMillisMicros other) const {
  int64_t millisDiff = millis - other.millis;
  int32_t microsDiff = micros - other.micros;
  return millisDiff * 1000 + microsDiff;
}
