#include "time_utils.h"

#include <gtest/gtest.h>
#include <cmath>

#include "vector.h"

TEST(TimeUtils, ParseDateTimeToUnixMillisAt1970) {
  int64_t millis = parseDateTimeToUnixMillis("1970-01-01T00:00:00.000Z");
  EXPECT_EQ(millis, 0);
}

TEST(TimeUtils, ParseDateTimeToUnixMillisAt2000) {
  int64_t millis = parseDateTimeToUnixMillis("2000-02-03T04:05:06Z");
  EXPECT_EQ(millis, 949550706000LL);
}

TEST(TimeUtils, ParseDateTimeToUnixMillisAt2000FractionalSeconds) {
  int64_t millis = parseDateTimeToUnixMillis("2000-02-03T04:05:06.789Z");
  EXPECT_EQ(millis, 949550706789LL);
}

TEST(TimeUtils, ParseDateTimeToUnixMillisAfterFebruary29th) {
  int64_t millis = parseDateTimeToUnixMillis("2004-04-09T04:05:06.789Z");
  EXPECT_EQ(millis, 1081483506789LL);
}

TEST(TimeUtils, ParseDateTimeToUnixMillisWithoutMonth) {
  int64_t millis = parseDateTimeToUnixMillis("2004-100T04:05:06.789Z");
  EXPECT_EQ(millis, 1081483506789LL);
}

#include "test_runner.inc"
