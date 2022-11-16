#include "tracking_menu.h"

#include <iomanip>
#include <cmath>

#include "menu.h"
#include "menu_entry.h"
#include "action_menu_entry.h"
#include "number_menu_entry.h"
#include "time_utils.h"

#include "trackable_objects.h"

std::vector<std::string> DISTANCE_SUFFIXES = {"m", "km", "Mm", "Gm", "Tm", "Pm", "Em", "Zm", "Ym"};

const int64_t INFO_UPDATE_INTERVAL_MICROS = 500000; // 0.5 seconds

std::string formatDistance(double distanceMetres) {
  std::ostringstream ss;
  for (std::string suffix : DISTANCE_SUFFIXES) {
    if (distanceMetres < 1000) {
      // If after rounding to 1 decimal place, we still get less than 10, then show the 1 decimal
      // place. This should keep the distance to <= 3 characters, plus 2 for the suffix.
      if (std::round(distanceMetres * 10) < 100) {
        ss << std::fixed << std::showpoint << std::setprecision(1) << distanceMetres;
      } else {
        ss << std::fixed << std::setprecision(0) << distanceMetres;
      }
      ss << suffix;
      return ss.str();
    }
    distanceMetres /= 1000.0;
  }
  return "###m";
}

static std::function<std::string()> currentInfoFunction = []() {
  return "";
};

std::function<std::string()> buildInfoFunction(std::string constantPrefix, Tracker &tracker, bool includeDistance) {
  return [&tracker, constantPrefix, includeDistance]() {
    TimeMillisMicros now = TimeMillisMicros::now();
    std::ostringstream ss;
    ss << constantPrefix;
    if (includeDistance) {
      ss << "\nDistance: " << formatDistance(tracker.getDistanceAt(now.millis));
    }
    Direction dir = tracker.getDirectionAt(now.millis);
    ss << "\nAzi: " << std::fixed << std::setprecision(5) << std::right << std::setw(10) << dir.getAzimuth();
    ss << "\nAlt: " << std::fixed << std::setprecision(5) << std::right << std::setw(10) << dir.getAltitude();
    return ss.str();
  };
}

void setTrackedObject(std::string name, Tracker &tracker) {
  currentInfoFunction = buildInfoFunction(name, tracker, /* includeDistance= */ true);
  tracker.setTrackingFunction(TrackableObjects::getTrackingFunction(name));
}

std::shared_ptr<Menu> buildTrackableObjectsMenu(
    std::string menuName,
    std::vector<std::string> trackableObjectNames,
    Tracker &tracker,
    bool includeDistance = true) {
  std::vector<std::shared_ptr<MenuEntry>> menuEntries = {};
  for (std::string &name : trackableObjectNames) {
    std::function<std::string()> infoFunction = buildInfoFunction(name, tracker, includeDistance);
    menuEntries.push_back(
        std::make_shared<ActionMenuEntry>(
            name,
            [&tracker, name]() {
              setTrackedObject(name, tracker);
            },
            infoFunction,
            INFO_UPDATE_INTERVAL_MICROS));
  }
  return std::make_shared<Menu>(menuName, menuName, menuEntries);
}

std::shared_ptr<Menu> buildSatellitesMenu(
    std::string menuName,
    std::string menuTitle,
    std::vector<std::string> satelliteNames,
    Tracker &tracker,
    std::function<std::optional<std::string>(std::string)> urlFetchFunction) {
  std::vector<std::shared_ptr<MenuEntry>> menuEntries = {};
  for (std::string &name : satelliteNames) {
    SatelliteOrbit &sat = TrackableObjects::getSatelliteOrbit(name);
    menuEntries.push_back(
        std::make_shared<ActionMenuEntry>(
            name,
            [&tracker, &sat, name, urlFetchFunction]() {
              bool success = sat.fetchElements(urlFetchFunction);
              if (success) {
                tracker.setTrackingFunction(TrackableObjects::getTrackingFunction(name));
                currentInfoFunction = buildInfoFunction(name, tracker, /* includeDistance= */ true);
              } else {
                std::string info = name + "\nFailed to load.";
                tracker.setTrackingFunction([](int64_t timeMillis) {
                  return CartesianLocation::fixed(Vector(0, 0, 0));
                });
                currentInfoFunction = [info]() { return info; };
              }
            },
            []() { return currentInfoFunction(); },
            INFO_UPDATE_INTERVAL_MICROS));
  }
  return std::make_shared<Menu>(menuName, menuTitle, menuEntries);
}

std::shared_ptr<MenuEntry> buildManualGpsMenuEntry(Tracker &tracker, std::shared_ptr<MenuEntry> currentInfoEntry) {
  static std::string lastEnteredGpsLocation = "";
  std::shared_ptr<MenuEntry> manualGpsMenuEntry =
      std::make_shared<NumberMenuEntry>(
          "GPS",
          "Lat: ~##.#####N\nLng:~###.#####E",
          [](std::string location) {
            lastEnteredGpsLocation = location;
          });
  std::shared_ptr<MenuEntry> elevationMenuEntry =
      std::make_shared<NumberMenuEntry>(
          "Elevation",
          "Elev: ~#####m",
          [&tracker](std::string elevationStr) {
            double latitude = std::stod(lastEnteredGpsLocation.substr(5, 9));
            double longitude = std::stod(lastEnteredGpsLocation.substr(20, 10));
            double elevation = std::stod(elevationStr.substr(6, 6));
            CartesianLocation cartesian = Location(latitude, longitude, elevation).getCartesian();
            std::string info = "Manual Coords\n" + lastEnteredGpsLocation + "\n" + elevationStr;
            tracker.setTrackingFunction([cartesian](int64_t timeMillis) { return cartesian; });
            currentInfoFunction = buildInfoFunction(info, tracker, /* includeDistance= */ true);
          });
  manualGpsMenuEntry->setFollowOnMenuEntry(elevationMenuEntry);
  elevationMenuEntry->setFollowOnMenuEntry(currentInfoEntry);
  return manualGpsMenuEntry;
}

std::shared_ptr<MenuEntry> buildManualRaDeclMenuEntry(Tracker &tracker, std::shared_ptr<MenuEntry> currentInfoEntry) {
  std::shared_ptr<MenuEntry> manualRaDeclMenuEntry =
      std::make_shared<NumberMenuEntry>(
          "RA and Dec",
          "RA:  ##h##m##s\nDec:~##°##'##\"",
          [&tracker](std::string raDeclStr) {
            int32_t raHour = std::stoi(raDeclStr.substr(5, 2));
            int32_t raMinute = std::stoi(raDeclStr.substr(8, 2));
            int32_t raSecond = std::stoi(raDeclStr.substr(11, 2));
            int32_t decDegrees = std::stoi(raDeclStr.substr(19, 3));
            // The degree symbol (°) is two bytes long.
            int32_t decArcminute = std::stoi(raDeclStr.substr(24, 2));
            int32_t decArcsecond = std::stoi(raDeclStr.substr(27, 2));
            CartesianLocation cartesian =
                EquatorialLocation(
                    raHour, raMinute, raSecond,
                    decDegrees, decArcminute, decArcsecond)
                .farCartesian();
            std::string info = "Manual RA & Dec\n" + raDeclStr;
            tracker.setTrackingFunction([cartesian](int64_t timeMillis) { return cartesian; });
            currentInfoFunction = buildInfoFunction(info, tracker, /* includeDistance= */ false);
          });
  manualRaDeclMenuEntry->setFollowOnMenuEntry(currentInfoEntry);
  return manualRaDeclMenuEntry;
}

std::shared_ptr<MenuEntry> buildManualNoradIdEntry(
    Tracker &tracker,
    std::shared_ptr<MenuEntry> currentInfoEntry,
    std::function<std::optional<std::string>(std::string)> urlFetchFunction) {
  static SatelliteOrbit currentOrbit = SatelliteOrbit("");
  std::shared_ptr<MenuEntry> manualNoradIdMenuEntry =
      std::make_shared<NumberMenuEntry>(
          "NORAD ID",
          "ID: #####",
          [&tracker, urlFetchFunction](std::string idStr) {
            std::string noradId = idStr.substr(4, 5);
            currentOrbit = SatelliteOrbit(noradId);
            bool success = currentOrbit.fetchElements(urlFetchFunction);
            if (success) {
              std::string info = "NORAD ID: " + noradId;
              tracker.setTrackingFunction([](int64_t timeMillis) {
                return currentOrbit.toCartesian(timeMillis);
              });
              currentInfoFunction = buildInfoFunction(info, tracker, /* includeDistance= */ true);
            } else {
              std::string info = "NORAD ID: " + noradId + "\nFailed to load.";
              tracker.setTrackingFunction([](int64_t timeMillis) {
                return CartesianLocation::fixed(Vector(0, 0, 0));
              });
              currentInfoFunction = [info]() { return info; };
            }
          });
  manualNoradIdMenuEntry->setFollowOnMenuEntry(currentInfoEntry);
  return manualNoradIdMenuEntry;
}

std::shared_ptr<Menu> buildTrackingMenu(
    Tracker &tracker,
    std::function<std::optional<std::string>(std::string)> urlFetchFunction) {
  std::shared_ptr<MenuEntry> currentInfoEntry =
      std::make_shared<InfoMenuEntry>(
        "Current",
        []() { return currentInfoFunction(); },
        INFO_UPDATE_INTERVAL_MICROS);

  std::vector<std::shared_ptr<MenuEntry>> satelliteEntries = {
    buildSatellitesMenu(
        "LEO Sats",
        "Low earth orbit",
        TrackableObjects::LOW_EARTH_ORBIT_SATELLITES,
        tracker,
        urlFetchFunction),
    buildSatellitesMenu(
        "GEO Sats",
        "Geosynchronous",
        TrackableObjects::GEOSYNCHRONOUS_SATELLITES,
        tracker,
        urlFetchFunction),
  };
  std::vector<std::shared_ptr<MenuEntry>> manualEntries = {
    buildManualGpsMenuEntry(tracker, currentInfoEntry),
    buildManualRaDeclMenuEntry(tracker, currentInfoEntry),
    buildManualNoradIdEntry(tracker, currentInfoEntry, urlFetchFunction),
  };
  std::vector<std::shared_ptr<MenuEntry>> categoryEntries = {
    currentInfoEntry,
    std::make_shared<Menu>("Satellites", satelliteEntries),
    buildTrackableObjectsMenu("Planets", TrackableObjects::PLANETS, tracker),
    buildTrackableObjectsMenu("Stars", TrackableObjects::STARS, tracker, /* includeDistance= */ false),
    buildTrackableObjectsMenu("Cities", TrackableObjects::CITIES, tracker),
    buildTrackableObjectsMenu("Other", TrackableObjects::OTHER, tracker),
    std::make_shared<Menu>("Manual", manualEntries),
  };
  // Default to tracking the ISS.
  setTrackedObject("ISS", tracker);
  return std::make_shared<Menu>("Tracking", categoryEntries);
}
