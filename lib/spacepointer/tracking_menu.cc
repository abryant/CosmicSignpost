#include "tracking_menu.h"

#include "menu.h"
#include "menu_entry.h"
#include "action_menu_entry.h"

#include "trackable_objects.h"

std::shared_ptr<Menu> buildTrackableObjectsMenu(
    std::string menuName,
    std::map<std::string, tracking_function> trackableObjects,
    std::function<void(tracking_function)> chooseTrackedObject) {
  std::vector<std::shared_ptr<MenuEntry>> menuEntries = {};
  for (std::map<std::string, tracking_function>::iterator it = trackableObjects.begin(); it != trackableObjects.end(); ++it) {
    tracking_function trackingFunction = it->second;
    menuEntries.push_back(std::make_shared<ActionMenuEntry>(it->first, [chooseTrackedObject, trackingFunction]() {
      chooseTrackedObject(trackingFunction);
    }));
  }
  return std::make_shared<Menu>(menuName, menuEntries);
}

std::shared_ptr<Menu> buildTrackingMenu(std::function<void(tracking_function)> chooseTrackedObject) {
  std::vector<std::shared_ptr<MenuEntry>> satelliteEntries = {
    buildTrackableObjectsMenu("LEO Sats", TRACKABLE_LOW_EARTH_ORBIT_SATELLITES, chooseTrackedObject),
    buildTrackableObjectsMenu("GEO Sats", TRACKABLE_GEOSTATIONARY_SATELLITES, chooseTrackedObject),
  };
  std::vector<std::shared_ptr<MenuEntry>> categoryEntries = {
    std::make_shared<Menu>("Satellites", satelliteEntries),
    buildTrackableObjectsMenu("Planets", TRACKABLE_PLANETS, chooseTrackedObject),
    buildTrackableObjectsMenu("Stars", TRACKABLE_STARS, chooseTrackedObject),
    buildTrackableObjectsMenu("Cities", TRACKABLE_CITIES, chooseTrackedObject),
    buildTrackableObjectsMenu("Other", TRACKABLE_OTHER, chooseTrackedObject),
  };
  return std::make_shared<Menu>("Tracking", categoryEntries);
}
