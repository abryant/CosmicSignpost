#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TIME_UTILS_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TIME_UTILS_H_

#include <cstdint>

// January 1, 2000, 11:58:55.816 UTC
const int64_t J2000_UTC_MILLIS = 946727935816;

// January 1, 2000, 12:00:00.000 UTC
const int64_t JANUARY_1_2000_12PM_UTC_MILLIS = 946728000000;

double daysSinceJ2000(int64_t unixTimeMillis);

double millisToJulianDays(int64_t milliseconds);

class TimeMillisMicros {
  public:
    // always positive
    int64_t millis;
    // always positive, and in the range [0,1000)
    int32_t micros;

    TimeMillisMicros();
    TimeMillisMicros(int64_t millis, int64_t micros);
    static TimeMillisMicros now();
    static TimeMillisMicros ofMicros(int64_t micros);
    TimeMillisMicros plusMicros(int64_t addedMicros) const;
    TimeMillisMicros minusMicros(int64_t subtractedMicros) const;
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
