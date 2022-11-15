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

const int64_t INFO_UPDATE_INTERVAL_MICROS = 200000; // 0.2 seconds

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

std::function<std::string()> buildInfoFunction(std::string name, Tracker &tracker) {
  return [&tracker, name]() {
    TimeMillisMicros now = TimeMillisMicros::now();
    std::ostringstream ss;
    ss << name;
    ss << "\nDistance: " << formatDistance(tracker.getDistanceAt(now.millis));
    Direction dir = tracker.getDirectionAt(now.millis);
    ss << "\nAzi: " << std::fixed << std::setprecision(5) << std::right << std::setw(10) << dir.getAzimuth();
    ss << "\nAlt: " << std::fixed << std::setprecision(5) << std::right << std::setw(10) << dir.getAltitude();
    return ss.str();
  };
}

void setTrackedObject(std::string name, Tracker &tracker) {
  currentInfoFunction = buildInfoFunction(name, tracker);
  tracker.setTrackingFunction(TrackableObjects::getTrackingFunction(name));
}

std::shared_ptr<Menu> buildTrackableObjectsMenu(
    std::string menuName,
    std::string menuTitle,
    std::vector<std::string> trackableObjectNames,
    Tracker &tracker) {
  std::vector<std::shared_ptr<MenuEntry>> menuEntries = {};
  for (std::string &name : trackableObjectNames) {
    std::function<std::string()> infoFunction = buildInfoFunction(name, tracker);
    menuEntries.push_back(
        std::make_shared<ActionMenuEntry>(
            name,
            [&tracker, name]() {
              setTrackedObject(name, tracker);
            },
            infoFunction,
            INFO_UPDATE_INTERVAL_MICROS));
  }
  return std::make_shared<Menu>(menuName, menuTitle, menuEntries);
}

std::shared_ptr<Menu> buildTrackableObjectsMenu(
    std::string menuName,
    std::vector<std::string> trackableObjectNames,
    Tracker &tracker) {
  return buildTrackableObjectsMenu(menuName, menuName, trackableObjectNames, tracker);
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
  std::shared_ptr<MenuEntry> altitudeMenuEntry =
      std::make_shared<NumberMenuEntry>(
          "Altitude",
          "Alt: ~#####m",
          [&tracker](std::string altitudeStr) {
            double latitude = std::stod(lastEnteredGpsLocation.substr(5, 9));
            double longitude = std::stod(lastEnteredGpsLocation.substr(20, 10));
            double altitude = std::stod(altitudeStr.substr(5, 6));
            CartesianLocation cartesian = Location(latitude, longitude, altitude).getCartesian();
            std::string info = "Manual Coords\n" + lastEnteredGpsLocation + "\n" + altitudeStr;
            tracker.setTrackingFunction([cartesian](int64_t timeMillis) { return cartesian; });
            currentInfoFunction = buildInfoFunction(info, tracker);
          });
  manualGpsMenuEntry->setFollowOnMenuEntry(altitudeMenuEntry);
  altitudeMenuEntry->setFollowOnMenuEntry(currentInfoEntry);
  return manualGpsMenuEntry;
}

std::shared_ptr<MenuEntry> buildManualRaDeclMenuEntry(Tracker &tracker, std::shared_ptr<MenuEntry> currentInfoEntry) {
  std::shared_ptr<MenuEntry> manualRaDeclMenuEntry =
      std::make_shared<NumberMenuEntry>(
          "RA and Dec",
          "RA:  ##h##m##s\nDec:~##d##'##\"",
          [&tracker](std::string raDeclStr) {
            int32_t raHour = std::stoi(raDeclStr.substr(5, 2));
            int32_t raMinute = std::stoi(raDeclStr.substr(8, 2));
            int32_t raSecond = std::stoi(raDeclStr.substr(11, 2));
            int32_t decDegrees = std::stoi(raDeclStr.substr(19, 3));
            int32_t decArcminute = std::stoi(raDeclStr.substr(23, 2));
            int32_t decArcsecond = std::stoi(raDeclStr.substr(26, 2));
            CartesianLocation cartesian =
                EquatorialLocation(
                    raHour, raMinute, raSecond,
                    decDegrees, decArcminute, decArcsecond)
                .farCartesian();
            std::string info = "Manual RA & Dec\n" + raDeclStr;
            tracker.setTrackingFunction([cartesian](int64_t timeMillis) { return cartesian; });
            currentInfoFunction = buildInfoFunction(info, tracker);
          });
  manualRaDeclMenuEntry->setFollowOnMenuEntry(currentInfoEntry);
  return manualRaDeclMenuEntry;
}

std::shared_ptr<Menu> buildTrackingMenu(Tracker &tracker) {
  std::shared_ptr<MenuEntry> currentInfoEntry =
      std::make_shared<InfoMenuEntry>(
        "Current",
        []() {
          return currentInfoFunction();
        },
        INFO_UPDATE_INTERVAL_MICROS);

  std::vector<std::shared_ptr<MenuEntry>> satelliteEntries = {
    buildTrackableObjectsMenu("LEO Sats", "Low earth orbit", TrackableObjects::LOW_EARTH_ORBIT_SATELLITES, tracker),
    buildTrackableObjectsMenu("GEO Sats", "Geosynchronous", TrackableObjects::GEOSYNCHRONOUS_SATELLITES, tracker),
  };
  std::vector<std::shared_ptr<MenuEntry>> manualEntries = {
    buildManualGpsMenuEntry(tracker, currentInfoEntry),
    buildManualRaDeclMenuEntry(tracker, currentInfoEntry),
  };
  std::vector<std::shared_ptr<MenuEntry>> categoryEntries = {
    currentInfoEntry,
    std::make_shared<Menu>("Satellites", satelliteEntries),
    buildTrackableObjectsMenu("Planets", TrackableObjects::PLANETS, tracker),
    buildTrackableObjectsMenu("Stars", TrackableObjects::STARS, tracker),
    buildTrackableObjectsMenu("Cities", TrackableObjects::CITIES, tracker),
    buildTrackableObjectsMenu("Other", TrackableObjects::OTHER, tracker),
    std::make_shared<Menu>("Manual", manualEntries),
  };
  // Default to tracking the ISS.
  setTrackedObject("ISS", tracker);
  return std::make_shared<Menu>("Tracking", categoryEntries);
}
