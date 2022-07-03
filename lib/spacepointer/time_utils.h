#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TIME_UTILS_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TIME_UTILS_H_

#include <cstdint>

// January 1, 2000, 11:58:55.816 UTC
const int64_t J2000_UTC_MILLIS = 946727935816;

double daysSinceJ2000(int64_t unixTimeMillis);

class TimeMillisMicros {
  public:
    // always positive
    int64_t millis;
    // always positive, and in the range [0,1000)
    int32_t micros;

    TimeMillisMicros();
    TimeMillisMicros(int64_t millis, int32_t micros);
    static TimeMillisMicros now();
    static TimeMillisMicros ofMicros(int32_t micros);
    TimeMillisMicros plusMicros(int32_t addedMicros) const;
    TimeMillisMicros minusMicros(int32_t subtractedMicros) const;
    int64_t deltaMicrosSince(TimeMillisMicros other) const;

    // operator<=> only works in C++20, and ESP32 can't use that yet.
    [[nodiscard]] constexpr bool operator==(const TimeMillisMicros& other) const {
      return millis == other.millis && micros == other.micros;
    }
    [[nodiscard]] constexpr bool operator!=(const TimeMillisMicros& other) const {
      return !(*this == other);
    }
    [[nodiscard]] constexpr bool operator<(const TimeMillisMicros& other) const {
      return millis < other.millis || (millis == other.millis && micros < other.micros);
    }
    [[nodiscard]] constexpr bool operator>(const TimeMillisMicros& other) const {
      return other < *this;
    }
    [[nodiscard]] constexpr bool operator<=(const TimeMillisMicros& other) const {
      return *this < other || *this == other;
    }
    [[nodiscard]] constexpr bool operator>=(const TimeMillisMicros& other) const {
      return other <= *this;
    }
};

#endif
