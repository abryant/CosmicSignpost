#include "satellite_tracking_menu.h"

#include "menu_entry.h"
#include "action_menu_entry.h"
#include "number_menu_entry.h"
#include "tracking_menu.h"

std::shared_ptr<Menu> SatelliteTrackingMenu::buildSatellitesMenu(
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
                TrackingMenu::currentInfoFunction = TrackingMenu::buildInfoFunction(name, tracker, /* includeDistance= */ true);
              } else {
                std::string info = name + "\nFailed to load.";
                tracker.setTrackingFunction([](int64_t timeMillis) {
                  return CartesianLocation::fixed(Vector(0, 0, 0));
                });
                TrackingMenu::currentInfoFunction = [info]() { return info; };
              }
            },
            []() { return TrackingMenu::currentInfoFunction(); },
            TrackingMenu::INFO_UPDATE_INTERVAL_MICROS));
  }
  return std::make_shared<Menu>(menuName, menuTitle, menuEntries);
}

std::shared_ptr<MenuEntry> SatelliteTrackingMenu::buildManualNoradIdEntry(
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
              TrackingMenu::currentInfoFunction =
                  TrackingMenu::buildInfoFunction(info, tracker, /* includeDistance= */ true);
            } else {
              std::string info = "NORAD ID: " + noradId + "\nFailed to load.";
              tracker.setTrackingFunction([](int64_t timeMillis) {
                return CartesianLocation::fixed(Vector(0, 0, 0));
              });
              TrackingMenu::currentInfoFunction = [info]() { return info; };
            }
          });
  manualNoradIdMenuEntry->setFollowOnMenuEntry(currentInfoEntry);
  return manualNoradIdMenuEntry;
}

std::shared_ptr<Menu> SatelliteTrackingMenu::buildSatelliteTypesMenu(
    Tracker &tracker,
    std::shared_ptr<MenuEntry> currentInfoEntry,
    std::function<std::optional<std::string>(std::string)> urlFetchFunction) {
  std::vector<std::shared_ptr<MenuEntry>> satelliteEntries = {
    SatelliteTrackingMenu::buildSatellitesMenu(
        "LEO Sats",
        "Low earth orbit",
        TrackableObjects::LOW_EARTH_ORBIT_SATELLITES,
        tracker,
        urlFetchFunction),
    SatelliteTrackingMenu::buildSatellitesMenu(
        "GEO Sats",
        "Geosynchronous",
        TrackableObjects::GEOSYNCHRONOUS_SATELLITES,
        tracker,
        urlFetchFunction),
    SatelliteTrackingMenu::buildManualNoradIdEntry(
        tracker,
        currentInfoEntry,
        urlFetchFunction),
  };
  return std::make_shared<Menu>("Satellites", satelliteEntries);
}
