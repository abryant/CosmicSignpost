#include "tracker.h"

#include "trackable_objects.h"

#include <gtest/gtest.h>

#include "Arduino.h"

TEST(BenchmarkTracking, MarsForTenSeconds) {
  Tracker tracker = Tracker(Location(51.500804, -0.124340, 10), Direction(0, 0), TRACKABLE_OBJECTS.at("Mars"));
  const int ITERATIONS = 10000;
  // Direction *benchmarkData = (Direction*) malloc(ITERATIONS * sizeof(Direction));
  Direction benchmarkData;
  int64_t start = millis();
  for (int64_t i = 0; i < ITERATIONS; ++i) {
    benchmarkData = tracker.getDirectionAt(i + 1656157859000);
    std::cout << ".";
  }
  int64_t end = millis();
  // free(benchmarkData);
  std::cout << std::endl << "Elapsed: " << (end - start) << std::endl;
}

#include "test_runner.inc"
