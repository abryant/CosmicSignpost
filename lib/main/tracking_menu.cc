#include "tracking_menu.h"

#include <iomanip>
#include <cmath>

#include "menu.h"
#include "menu_entry.h"
#include "action_menu_entry.h"
#include "time_utils.h"

#include "trackable_objects.h"

std::vector<std::string> DISTANCE_SUFFIXES = {"m", "km", "Mm", "Gm", "Tm", "Pm", "Em", "Zm", "Ym"};

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

std::shared_ptr<Menu> buildTrackableObjectsMenu(
    std::string menuName,
    std::vector<std::string> trackableObjectNames,
    Tracker &tracker) {
  std::vector<std::shared_ptr<MenuEntry>> menuEntries = {};
  for (std::string &name : trackableObjectNames) {
    TrackableObjects::tracking_function trackingFunction = TrackableObjects::getTrackingFunction(name);
    menuEntries.push_back(
        std::make_shared<ActionMenuEntry>(
            name,
            [&tracker, trackingFunction]() {
              tracker.setTrackingFunction(trackingFunction);
            },
            [&tracker, name, trackingFunction]() {
              TimeMillisMicros now = TimeMillisMicros::now();
              return name + "\nDistance: " + formatDistance(tracker.getDistanceAt(now.millis));
            }));
  }
  return std::make_shared<Menu>(menuName, menuEntries);
}

std::shared_ptr<Menu> buildTrackingMenu(Tracker &tracker) {
  std::vector<std::shared_ptr<MenuEntry>> satelliteEntries = {
    buildTrackableObjectsMenu("LEO Sats", TrackableObjects::LOW_EARTH_ORBIT_SATELLITES, tracker),
    buildTrackableObjectsMenu("GEO Sats", TrackableObjects::GEOSTATIONARY_SATELLITES, tracker),
  };
  std::vector<std::shared_ptr<MenuEntry>> categoryEntries = {
    std::make_shared<Menu>("Satellites", satelliteEntries),
    buildTrackableObjectsMenu("Planets", TrackableObjects::PLANETS, tracker),
    buildTrackableObjectsMenu("Stars", TrackableObjects::STARS, tracker),
    buildTrackableObjectsMenu("Cities", TrackableObjects::CITIES, tracker),
    buildTrackableObjectsMenu("Other", TrackableObjects::OTHER, tracker),
  };
  return std::make_shared<Menu>("Tracking", categoryEntries);
}