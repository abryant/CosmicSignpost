#include "time_utils.h"

#include <cstdint>
#include <cstddef>

#include <sys/time.h>

double daysSinceJ2000(int64_t unixTimeMillis) {
  return millisToJulianDays(unixTimeMillis - J2000_UTC_MILLIS);
}

double millisToJulianDays(int64_t milliseconds) {
  return milliseconds / 1000.0 / 60.0 / 60.0 / 24.0;
}

TimeMillisMicros::TimeMillisMicros()
    : millis(0), micros(0) {}

TimeMillisMicros::TimeMillisMicros(int64_t millis, int64_t micros)
    : millis(millis + (micros / 1000)), micros(micros % 1000) {}

TimeMillisMicros TimeMillisMicros::now() {
  struct timeval time_now;
  while (true) {
    int result = gettimeofday(&time_now, NULL);
    if (result == 0) {
      break;
    }
  }
  int64_t resultMillis = (((int64_t) time_now.tv_sec) * 1000) + (((int64_t) time_now.tv_usec) / 1000);
  int64_t resultMicros = time_now.tv_usec % 1000;
  return TimeMillisMicros(resultMillis, resultMicros);
}

TimeMillisMicros TimeMillisMicros::ofMicros(int64_t micros) {
  return TimeMillisMicros(0, micros);
}

TimeMillisMicros TimeMillisMicros::plusMicros(int64_t plusMicros) const {
  return TimeMillisMicros(millis, micros + plusMicros);
}

int64_t positiveMod1000(int64_t micros) {
  int64_t result = micros % 1000;
  if (result < 0) {
    result += 1000;
  }
  return result;
}

TimeMillisMicros TimeMillisMicros::minusMicros(int64_t subtractedMicros) const {
  int64_t newMicros = micros - subtractedMicros;
  int64_t newMod1000 = positiveMod1000(newMicros);
  int64_t extraMillis = (newMicros - newMod1000) / 1000;
  return TimeMillisMicros(millis + extraMillis, newMod1000);
}

int64_t TimeMillisMicros::deltaMicrosSince(TimeMillisMicros other) const {
  int64_t millisDiff = millis - other.millis;
  int64_t microsDiff = micros - other.micros;
  return millisDiff * 1000 + microsDiff;
}
