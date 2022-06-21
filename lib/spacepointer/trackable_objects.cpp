#include "trackable_objects.h"

const std::map<std::string, std::function<CartesianLocation(int64_t)>> TRACKABLE_OBJECTS = {
  {"Brasilia", [](int64_t timeMillis) { return Location(-15.805268, -47.914144, 1110).getCartesian(); }},
  {"Cape Town", [](int64_t timeMillis) { return Location(-33.904166, 18.401101, 7).getCartesian(); }},
  {"Kyoto", [](int64_t timeMillis) { return Location(34.979871, 135.748719, 20).getCartesian(); }},
  {"London", [](int64_t timeMillis) { return Location(51.500804, -0.124340, 10).getCartesian(); }},
  {"New York", [](int64_t timeMillis) { return Location(40.777447, -73.969175, 25).getCartesian(); }},
  {"Rome", [](int64_t timeMillis) { return Location(41.890082, 12.492372, 20).getCartesian(); }},
  {"San Francisco", [](int64_t timeMillis) { return Location(37.802362, -122.405843, 90).getCartesian(); }},
  {"Sydney", [](int64_t timeMillis) { return Location(-33.857165, 151.215157, 10).getCartesian(); }},
  {"Vilnius", [](int64_t timeMillis) { return Location(54.686888, 25.291395, 95).getCartesian(); }},
  {"North Pole", [](int64_t timeMillis) { return Location(90.0, 0.0, 0).getCartesian(); }},
  {"South Pole", [](int64_t timeMillis) { return Location(90.0, 0.0, 0).getCartesian(); }},
  {"GPS 0,0", [](int64_t timeMillis) { return Location(0.0, 0.0, 0).getCartesian(); }},
  {"Polaris", [](int64_t timeMillis) { return EquatorialLocation(37.9500, 89.2642).farCartesian(); }},
  {"Ursa Major", [](int64_t timeMillis) { return EquatorialLocation(160.05, 55.38).farCartesian(); }},
  {"Sun", [](int64_t timeMillis) { return CartesianLocation(0, 0, 0, ReferenceFrame::SUN_ECLIPTIC); }},
  {"Moon", [](int64_t timeMillis) { return MoonOrbit::positionAt(timeMillis); }},
  {"Mercury", [](int64_t timeMillis) { return PlanetaryOrbit::MERCURY.toCartesian(timeMillis); }},
  {"Venus", [](int64_t timeMillis) { return PlanetaryOrbit::VENUS.toCartesian(timeMillis); }},
  {"EM Barycentre", [](int64_t timeMillis) { return PlanetaryOrbit::EARTH_MOON_BARYCENTRE.toCartesian(timeMillis); }},
  {"Mars", [](int64_t timeMillis) { return PlanetaryOrbit::MARS.toCartesian(timeMillis); }},
  {"Jupiter", [](int64_t timeMillis) { return PlanetaryOrbit::JUPITER.toCartesian(timeMillis); }},
  {"Saturn", [](int64_t timeMillis) { return PlanetaryOrbit::SATURN.toCartesian(timeMillis); }},
  {"Uranus", [](int64_t timeMillis) { return PlanetaryOrbit::URANUS.toCartesian(timeMillis); }},
  {"Neptune", [](int64_t timeMillis) { return PlanetaryOrbit::NEPTUNE.toCartesian(timeMillis); }},
};
