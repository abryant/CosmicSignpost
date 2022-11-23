#ifndef COSMIC_SIGNPOST_LIB_TRACKING_TIME_UTILS_H_
#define COSMIC_SIGNPOST_LIB_TRACKING_TIME_UTILS_H_

#include <cstdint>
#include <string>

// January 1, 2000, 11:58:55.816 UTC
const int64_t J2000_UTC_MILLIS = 946727935816LL;

// January 1, 2000, 12:00:00.000 UTC
const int64_t JANUARY_1_2000_12PM_UTC_MILLIS = 946728000000LL;

// January 1, 2000, 00:00:00.000 UTC
const int64_t JANUARY_1_2000_MIDNIGHT_UTC_MILLIS = 946684800000LL;

// January 1, 1970, 00:00:00.000 UTC
const double UNIX_EPOCH_JULIAN_DATE = 2440587.5;

// January 0, 1950, 00:00:00.000 UTC = December 31, 1949, 00:00:00.000 UTC
const double JAN_0_1950_JULIAN_DATE = 2433281.5;

double daysSinceJ2000(int64_t unixTimeMillis);

double millisToJulianDays(int64_t milliseconds);

int64_t unixTimeToApproxUt1(int64_t unixTimeMillis);
int64_t approxUt1ToUnixTime(int64_t timeUt1Millis);

// Converts certain ISO date+time strings with optional fractional components to UTC milliseconds
// since 1970-01-01.
// This depends on the TZ environment variable being set to UTC, because it uses std::mktime().
int64_t parseDateTimeToUnixMillis(std::string dateTimeString);

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
