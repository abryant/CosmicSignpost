#include "tracker.h"

#include "trackable_objects.h"

#include <gtest/gtest.h>

#ifdef ARDUINO
#include "Arduino.h"

TEST(BenchmarkTracking, MarsForTenSeconds) {
  Tracker tracker = Tracker(Location(51.500804, -0.124340, 10), Direction(0, 0), TRACKABLE_OBJECTS.at("Mars"));
  const int ITERATIONS = 10000;
  Direction benchmarkData;
  int64_t start = millis();
  for (int64_t i = 0; i < ITERATIONS; ++i) {
    benchmarkData = tracker.getDirectionAt(i + 1656157859000);
    std::cout << ".";
  }
  int64_t end = millis();
  std::cout << std::endl << "Elapsed: " << (end - start) << std::endl;
}
#else

TEST(BenchmarkTracking, MarsForTenSeconds) {
  // This test only works on the ESP32.
}

#endif

#include "test_runner.inc"
