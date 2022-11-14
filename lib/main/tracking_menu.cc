#include "tracking_menu.h"

#include <iomanip>
#include <cmath>

#include "menu.h"
#include "menu_entry.h"
#include "action_menu_entry.h"
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
    ss << "\nAzi: " << dir.getAzimuth();
    ss << "\nAlt: " << dir.getAltitude();
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

std::shared_ptr<Menu> buildTrackingMenu(Tracker &tracker) {
  std::vector<std::shared_ptr<MenuEntry>> satelliteEntries = {
    buildTrackableObjectsMenu("LEO Sats", "Low earth orbit", TrackableObjects::LOW_EARTH_ORBIT_SATELLITES, tracker),
    buildTrackableObjectsMenu("GEO Sats", "Geosynchronous", TrackableObjects::GEOSYNCHRONOUS_SATELLITES, tracker),
  };
  std::vector<std::shared_ptr<MenuEntry>> categoryEntries = {
    std::make_shared<InfoMenuEntry>("Current", []() {
      return currentInfoFunction();
    }, INFO_UPDATE_INTERVAL_MICROS),
    std::make_shared<Menu>("Satellites", satelliteEntries),
    buildTrackableObjectsMenu("Planets", TrackableObjects::PLANETS, tracker),
    buildTrackableObjectsMenu("Stars", TrackableObjects::STARS, tracker),
    buildTrackableObjectsMenu("Cities", TrackableObjects::CITIES, tracker),
    buildTrackableObjectsMenu("Other", TrackableObjects::OTHER, tracker),
  };
  // Default to tracking the ISS.
  setTrackedObject("ISS", tracker);
  return std::make_shared<Menu>("Tracking", categoryEntries);
}
