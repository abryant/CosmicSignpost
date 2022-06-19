#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TIME_UTILS_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TIME_UTILS_H_

#include <cstdint>

// January 1, 2000, 11:58:55.816 UTC
const int64_t J2000_UTC_MILLIS = 946727935816;

double daysSinceJ2000(int64_t unixTimeMillis);

#endif
