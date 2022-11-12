#include "tracking_menu.h"

#include "menu.h"
#include "menu_entry.h"
#include "action_menu_entry.h"

#include "trackable_objects.h"

std::shared_ptr<Menu> buildTrackableObjectsMenu(
    std::string menuName,
    std::vector<std::string> trackableObjectNames,
    std::function<void(TrackableObjects::tracking_function)> chooseTrackedObject) {
  std::vector<std::shared_ptr<MenuEntry>> menuEntries = {};
  for (std::string &name : trackableObjectNames) {
    TrackableObjects::tracking_function trackingFunction = TrackableObjects::getTrackingFunction(name);
    menuEntries.push_back(std::make_shared<ActionMenuEntry>(name, [chooseTrackedObject, trackingFunction]() {
      chooseTrackedObject(trackingFunction);
    }));
  }
  return std::make_shared<Menu>(menuName, menuEntries);
}

std::shared_ptr<Menu> buildTrackingMenu(std::function<void(TrackableObjects::tracking_function)> chooseTrackedObject) {
  std::vector<std::shared_ptr<MenuEntry>> satelliteEntries = {
    buildTrackableObjectsMenu("LEO Sats", TrackableObjects::LOW_EARTH_ORBIT_SATELLITES, chooseTrackedObject),
    buildTrackableObjectsMenu("GEO Sats", TrackableObjects::GEOSTATIONARY_SATELLITES, chooseTrackedObject),
  };
  std::vector<std::shared_ptr<MenuEntry>> categoryEntries = {
    std::make_shared<Menu>("Satellites", satelliteEntries),
    buildTrackableObjectsMenu("Planets", TrackableObjects::PLANETS, chooseTrackedObject),
    buildTrackableObjectsMenu("Stars", TrackableObjects::STARS, chooseTrackedObject),
    buildTrackableObjectsMenu("Cities", TrackableObjects::CITIES, chooseTrackedObject),
    buildTrackableObjectsMenu("Other", TrackableObjects::OTHER, chooseTrackedObject),
  };
  return std::make_shared<Menu>("Tracking", categoryEntries);
}
