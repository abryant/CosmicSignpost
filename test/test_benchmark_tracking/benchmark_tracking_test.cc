#include "tracker.h"

#include <optional>
#include <string>
#include <gtest/gtest.h>

#include "trackable_objects.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <HTTPClient.h>

// Required for fetching satellite information from Celestrak.
std::optional<std::string> fetchUrl(std::string url) {
  HTTPClient http;
  http.begin(url.c_str());
  http.GET();
  std::string payload = std::string(http.getString().c_str());
  return std::optional(payload);
}

TEST(BenchmarkTracking, MarsForOneSecond) {
  Tracker tracker = Tracker(Location(51.500804, -0.124340, 10), Direction(0, 0), TRACKABLE_PLANETS.at("Mars"));
  const int ITERATIONS = 1000;
  Direction benchmarkData;
  int64_t start = millis();
  for (int64_t i = 0; i < ITERATIONS; ++i) {
    benchmarkData = tracker.getDirectionAt(i + 1656157859000);
    std::cout << ".";
  }
  int64_t end = millis();
  std::cout << std::endl << "Elapsed: " << (end - start) << std::endl;
}

TEST(BenchmarkTracking, IssForOneSecond) {
  initSatellites(fetchUrl);
  Tracker tracker = Tracker(Location(51.500804, -0.124340, 10), Direction(0, 0), TRACKABLE_LOW_EARTH_ORBIT_SATELLITES.at("ISS"));
  const int ITERATIONS = 1000;
  Direction benchmarkData;
  int64_t start = millis();
  for (int64_t i = 0; i < ITERATIONS; ++i) {
    benchmarkData = tracker.getDirectionAt(i + 1656157859000);
    std::cout << ".";
  }
  int64_t end = millis();
  std::cout << std::endl << "Elapsed: " << (end - start) << std::endl;
}

TEST(BenchmarkTracking, Sxm8ForOneSecond) {
  initSatellites(fetchUrl);
  Tracker tracker = Tracker(Location(51.500804, -0.124340, 10), Direction(0, 0), TRACKABLE_GEOSTATIONARY_SATELLITES.at("Sirius XM-8"));
  const int ITERATIONS = 1000;
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

TEST(BenchmarkTracking, IssForTenSeconds) {
  // This test only works on the ESP32.
}

TEST(BenchmarkTracking, Sxm8ForTenSeconds) {
  // This test only works on the ESP32.
}

#endif

#include "test_runner.inc"
