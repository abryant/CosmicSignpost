#include "trackable_objects.h"

// To fit on the LCD menu, all names must be at most 12 characters long.

std::map<std::string, SatelliteOrbit> TRACKABLE_SATELLITES = {
  // Space stations
  {"ISS", SatelliteOrbit("25544")},
  {"Tiangong", SatelliteOrbit("48274")},
  // Low orbits
  {"Worldview 3", SatelliteOrbit("40115")},
  {"CartoSat 3", SatelliteOrbit("44804")},
  {"KMSL", SatelliteOrbit("47950")}, // Korea Microgravity Science Laboratory
  // Geosynchronous orbits
  {"Sirius XM-8", SatelliteOrbit("48838")}, // Longitude: -120
  {"SES-17", SatelliteOrbit("49055")}, // Longitude: -61.7 (eventually)
  {"Skynet 5A", SatelliteOrbit("30794")}, // Longitude: -1.12
  {"INMARSAT6 F1", SatelliteOrbit("50319")}, // Longitude: 60
  {"Koreasat 7", SatelliteOrbit("42691")}, // Longitude: 116
  {"Intelsat 18", SatelliteOrbit("37834")}, // Longitude: 180
};

std::map<std::string, TrackableObjects::tracking_function> TRACKABLE_OBJECTS = {
  // Planets
  {"Mercury", [](int64_t timeMillis) { return PlanetaryOrbit::MERCURY.toCartesian(timeMillis); }},
  {"Venus", [](int64_t timeMillis) { return PlanetaryOrbit::VENUS.toCartesian(timeMillis); }},
  {"Earth", [](int64_t timeMillis) { return CartesianLocation::fixed(Vector(0, 0, 0)); }},
  {"Mars", [](int64_t timeMillis) { return PlanetaryOrbit::MARS.toCartesian(timeMillis); }},
  {"Jupiter", [](int64_t timeMillis) { return PlanetaryOrbit::JUPITER.toCartesian(timeMillis); }},
  {"Saturn", [](int64_t timeMillis) { return PlanetaryOrbit::SATURN.toCartesian(timeMillis); }},
  {"Uranus", [](int64_t timeMillis) { return PlanetaryOrbit::URANUS.toCartesian(timeMillis); }},
  {"Neptune", [](int64_t timeMillis) { return PlanetaryOrbit::NEPTUNE.toCartesian(timeMillis); }},
  // Stars
  {"Polaris", [](int64_t timeMillis) { return EquatorialLocation(37.9500, 89.2642).farCartesian(); }},
  {"Ursa Major", [](int64_t timeMillis) { return EquatorialLocation(160.05, 55.38).farCartesian(); }},
  // Cities
  {"Brasilia", [](int64_t timeMillis) { return Location(-15.805268, -47.914144, 1110).getCartesian(); }},
  {"Cape Town", [](int64_t timeMillis) { return Location(-33.904166, 18.401101, 7).getCartesian(); }},
  {"Kyoto", [](int64_t timeMillis) { return Location(34.979871, 135.748719, 20).getCartesian(); }},
  {"London", [](int64_t timeMillis) { return Location(51.500804, -0.124340, 10).getCartesian(); }},
  {"New York", [](int64_t timeMillis) { return Location(40.777447, -73.969175, 25).getCartesian(); }},
  {"Rome", [](int64_t timeMillis) { return Location(41.890082, 12.492372, 20).getCartesian(); }},
  {"SanFrancisco", [](int64_t timeMillis) { return Location(37.802362, -122.405843, 90).getCartesian(); }},
  {"Sydney", [](int64_t timeMillis) { return Location(-33.857165, 151.215157, 10).getCartesian(); }},
  {"Vilnius", [](int64_t timeMillis) { return Location(54.686888, 25.291395, 95).getCartesian(); }},
  // Other
  {"Sun", [](int64_t timeMillis) { return CartesianLocation(Vector(0, 0, 0), ReferenceFrame::SUN_ECLIPTIC); }},
  {"Moon", [](int64_t timeMillis) { return MoonOrbit::positionAt(timeMillis); }},
  {"EMBarycentre", [](int64_t timeMillis) { return PlanetaryOrbit::EARTH_MOON_BARYCENTRE.toCartesian(timeMillis); }},
  {"North Pole", [](int64_t timeMillis) { return Location(90.0, 0.0, 0).getCartesian(); }},
  {"South Pole", [](int64_t timeMillis) { return Location(-90.0, 0.0, 0).getCartesian(); }},
  {"GPS 0,0", [](int64_t timeMillis) { return Location(0.0, 0.0, 0).getCartesian(); }},
};

TrackableObjects::tracking_function getSatellite(std::string name) {
  return [name](int64_t timeMillis) { return TRACKABLE_SATELLITES.at(name).toCartesian(timeMillis); };
}

TrackableObjects::tracking_function TrackableObjects::getTrackingFunction(std::string name) {
  if (TRACKABLE_SATELLITES.count(name) != 0) {
    return getSatellite(name);
  }
  return TRACKABLE_OBJECTS.at(name);
}

bool TrackableObjects::initSatellites(std::function<std::optional<std::string>(std::string)> urlFetchFunction) {
  for (auto it = TRACKABLE_SATELLITES.begin(); it != TRACKABLE_SATELLITES.end(); it++) {
    bool success = it->second.fetchElements(urlFetchFunction);
    if (!success) {
      return false;
    }
  }
  return true;
}

const std::vector<std::string> TrackableObjects::LOW_EARTH_ORBIT_SATELLITES = {
  "ISS",
  "Tiangong",
  "Worldview 3",
  "CartoSat 3",
  "KMSL",
};

const std::vector<std::string> TrackableObjects::GEOSTATIONARY_SATELLITES = {
  "Sirius XM-8",
  "SES-17",
  "Skynet 5A",
  "INMARSAT6 F1",
  "Koreasat 7",
  "Intelsat 18",
};

const std::vector<std::string> TrackableObjects::PLANETS = {
  "Mercury",
  "Venus",
  "Earth",
  "Mars",
  "Jupiter",
  "Saturn",
  "Uranus",
  "Neptune",
};

const std::vector<std::string> TrackableObjects::STARS = {
  "Polaris",
  "Ursa Major"
};

const std::vector<std::string> TrackableObjects::CITIES = {
  "Brasilia",
  "Cape Town",
  "Kyoto",
  "London",
  "New York",
  "Rome",
  "SanFrancisco",
  "Sydney",
  "Vilnius",
};

const std::vector<std::string> TrackableObjects::OTHER = {
  "Sun",
  "Moon",
  "EMBarycentre",
  "North Pole",
  "South Pole",
  "GPS 0,0",
};
