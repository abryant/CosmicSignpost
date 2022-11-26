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
  {"PolarCube", SatelliteOrbit("47310")},
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
  {"Alpha Cen", [](int64_t timeMillis) { return EquatorialLocation(14, 39, 35.06311, -60, -50, -2.3737).farCartesian(); }},
  {"Andromeda", [](int64_t timeMillis) { return EquatorialLocation(0, 42, 44.3, 41, 16, 9).farCartesian(); }},
  {"Betelgeuse", [](int64_t timeMillis) { return EquatorialLocation(5, 55, 10.30536, 7, 24, 25.4304).farCartesian(); }},
  {"CanisMajoris", [](int64_t timeMillis) { return EquatorialLocation(7, 22, 58.32877, -25, -46, -3.2355).farCartesian(); }},
  {"Crab Nebula", [](int64_t timeMillis) { return EquatorialLocation(5, 34, 31.94, 22, 0, 52.2).farCartesian(); }},
  {"Polaris", [](int64_t timeMillis) { return EquatorialLocation(37.9500, 89.2642).farCartesian(); }},
  {"SagittariusA", [](int64_t timeMillis) { return EquatorialLocation(17, 45, 40.0409, -29, -0, -28.118).farCartesian(); }},
  {"Tabby's Star", [](int64_t timeMillis) { return EquatorialLocation(20, 6, 15.45265, 44, 27, 24.7909).farCartesian(); }},
  {"Ursa Major", [](int64_t timeMillis) { return EquatorialLocation(160.05, 55.38).farCartesian(); }},
  {"UY Scuti", [](int64_t timeMillis) { return EquatorialLocation(18, 27, 36.5334, -12, -27, -58.866).farCartesian(); }},
  // Cities
  {"Athens", [](int64_t timeMillis) { return Location(37.971480, 23.726622, 160).getCartesian(); }},
  {"Beijing", [](int64_t timeMillis) { return Location(39.908134, 116.391165, 46).getCartesian(); }},
  {"Berlin", [](int64_t timeMillis) { return Location(52.518592, 13.399677, 28).getCartesian(); }},
  {"Brasilia", [](int64_t timeMillis) { return Location(-15.805268, -47.914144, 1110).getCartesian(); }},
  {"Buenos Aires", [](int64_t timeMillis) { return Location(-34.584123, -58.396101, 14).getCartesian(); }},
  {"Cape Town", [](int64_t timeMillis) { return Location(-33.904166, 18.401101, 7).getCartesian(); }},
  {"Hong Kong", [](int64_t timeMillis) { return Location(22.301231, 114.170167, 28).getCartesian(); }},
  {"Jerusalem", [](int64_t timeMillis) { return Location(31.771935, 35.202376, 775).getCartesian(); }},
  {"Kyoto", [](int64_t timeMillis) { return Location(34.979871, 135.748719, 20).getCartesian(); }},
  {"London", [](int64_t timeMillis) { return Location(51.500804, -0.124340, 10).getCartesian(); }},
  {"Madrid", [](int64_t timeMillis) { return Location(40.416887, -3.703848, 644).getCartesian(); }},
  {"Mecca", [](int64_t timeMillis) { return Location(21.422855, 39.825731, 288).getCartesian(); }},
  {"New York", [](int64_t timeMillis) { return Location(40.777447, -73.969175, 25).getCartesian(); }},
  {"Paris", [](int64_t timeMillis) { return Location(48.856461, 2.352411, 34).getCartesian(); }},
  {"Rome", [](int64_t timeMillis) { return Location(41.890082, 12.492372, 20).getCartesian(); }},
  {"SanFrancisco", [](int64_t timeMillis) { return Location(37.802362, -122.405843, 90).getCartesian(); }},
  {"Singapore", [](int64_t timeMillis) { return Location(1.363051, 103.845340, 7).getCartesian(); }},
  {"Sydney", [](int64_t timeMillis) { return Location(-33.857165, 151.215157, 10).getCartesian(); }},
  {"Tokyo", [](int64_t timeMillis) { return Location(35.673496, 139.756797, 4).getCartesian(); }},
  {"Toronto", [](int64_t timeMillis) { return Location(43.716576, -79.338062, 119).getCartesian(); }},
  {"Ulaanbaatar", [](int64_t timeMillis) { return Location(47.917623, 106.920040, 1295).getCartesian(); }},
  {"Vilnius", [](int64_t timeMillis) { return Location(54.686888, 25.291395, 95).getCartesian(); }},
  {"WashingtonDC", [](int64_t timeMillis) { return Location(38.889827, -77.010380, 13).getCartesian(); }},
  {"Wellington", [](int64_t timeMillis) { return Location(-41.284321, 174.767276, 126).getCartesian(); }},
  {"Yerevan", [](int64_t timeMillis) { return Location(40.185360, 44.515033, 1002).getCartesian(); }},
  // Places
  {"ChallengerDp", [](int64_t timeMillis) { return Location(11.373322, 142.591655, -10920).getCartesian(); }},
  {"ChristmasIsl", [](int64_t timeMillis) { return Location(-10.430196, 105.689378, 301).getCartesian(); }},
  {"EasterIsland", [](int64_t timeMillis) { return Location(-27.125722, -109.276868, 6).getCartesian(); }},
  {"MountEverest", [](int64_t timeMillis) { return Location(27.988056, 86.925278, 8848.86).getCartesian(); }},
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

SatelliteOrbit& TrackableObjects::getSatelliteOrbit(std::string name) {
  return TRACKABLE_SATELLITES.at(name);
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
  "PolarCube",
};

const std::vector<std::string> TrackableObjects::GEOSYNCHRONOUS_SATELLITES = {
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
  "Alpha Cen",
  "Andromeda",
  "Betelgeuse",
  "CanisMajoris",
  "Crab Nebula",
  "Polaris",
  "SagittariusA",
  "Tabby's Star",
  "Ursa Major",
  "UY Scuti",
};

const std::vector<std::string> TrackableObjects::CITIES = {
  "Athens",
  "Beijing",
  "Berlin",
  "Brasilia",
  "Buenos Aires",
  "Cape Town",
  "Hong Kong",
  "Jerusalem",
  "Kyoto",
  "London",
  "Madrid",
  "Mecca",
  "New York",
  "Paris",
  "Rome",
  "SanFrancisco",
  "Singapore",
  "Sydney",
  "Tokyo",
  "Toronto",
  "Ulaanbaatar",
  "Vilnius",
  "WashingtonDC",
  "Wellington",
  "Yerevan",
};

const std::vector<std::string> TrackableObjects::PLACES = {
  "ChallengerDp",
  "ChristmasIsl",
  "EasterIsland",
  "MountEverest",
};

const std::vector<std::string> TrackableObjects::OTHER = {
  "Sun",
  "Moon",
  "EMBarycentre",
  "North Pole",
  "South Pole",
  "GPS 0,0",
};
