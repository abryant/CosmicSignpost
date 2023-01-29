#include "satellite_tracking_menu.h"

#include <cmath>
#include <sstream>

#include "async_queue.h"
#include "menu_entry.h"
#include "action_menu_entry.h"
#include "number_menu_entry.h"
#include "output_devices.h"
#include "tracking_menu.h"

std::string splitTextIntoLines(std::string text, int32_t singleLineLength, int32_t lineLength) {
  if (text.length() <= singleLineLength) {
    return text;
  }
  // Only add an initial newline if we don't have the whole first line to ourselves. If the single
  // line length is lower, then there is a heading on the first line that we need to add a newline
  // after.
  bool addNewline = singleLineLength < lineLength;
  std::ostringstream ss;
  while (text.length() > 0) {
    if (addNewline) {
      ss << "\n";
    }
    addNewline = true;
    ss << text.substr(0, lineLength);
    text = text.length() > lineLength ? text.substr(lineLength) : "";
  }
  return ss.str();
}

std::string formatOrbitalPeriod(double orbitalPeriodSeconds) {
  std::ostringstream ss;
  if (orbitalPeriodSeconds >= 24*60*60) {
    ss << ((int32_t) (orbitalPeriodSeconds / (24*60*60))) << "d";
    orbitalPeriodSeconds = std::fmod(orbitalPeriodSeconds, 24*60*60);
  }
  if (orbitalPeriodSeconds >= 60*60) {
    ss << ((int32_t) (orbitalPeriodSeconds / (60*60))) << "h";
    orbitalPeriodSeconds = std::fmod(orbitalPeriodSeconds, 60*60);
  }
  if (orbitalPeriodSeconds >= 60) {
    ss << ((int32_t) (orbitalPeriodSeconds / (60))) << "m";
    orbitalPeriodSeconds = std::fmod(orbitalPeriodSeconds, 60);
  }
  ss << ((int32_t) orbitalPeriodSeconds) << "s";
  return ss.str();
}

std::string getSatelliteInfo(SatelliteOrbit &orbit) {
  std::ostringstream ss;
  ss << splitTextIntoLines(
      orbit.getName(),
      OutputDevices::DISPLAY_LENGTH - 1,
      OutputDevices::DISPLAY_LENGTH - 1);
  ss << "\nNORAD ID: " << orbit.getCatalogNumber();
  ss << "\nPer: ";
  ss << splitTextIntoLines(
      formatOrbitalPeriod(orbit.getOrbitalPeriodSeconds()),
      OutputDevices::DISPLAY_LENGTH - 6,
      OutputDevices::DISPLAY_LENGTH - 1);
  return ss.str();
}

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
              std::string downloadingInfo = name + "\nDownloading...";
              TrackingMenu::currentInfoFunction = [downloadingInfo]() { return downloadingInfo; };
              std::function<void()> fetchAndContinueFunction = [&tracker, &sat, name, urlFetchFunction]() -> void {
                bool success = sat.fetchElements(urlFetchFunction);
                if (success) {
                  std::string info = getSatelliteInfo(sat);
                  tracker.setTrackingFunction(TrackableObjects::getTrackingFunction(name));
                  TrackingMenu::currentInfoFunction = TrackingMenu::buildInfoFunction(info, tracker, /* includeDistance= */ true);
                } else {
                  std::string info = name + "\nFailed to load.";
                  tracker.setTrackingFunction([](int64_t timeMillis) {
                    return CartesianLocation::fixed(Vector(0, 0, 0));
                  });
                  TrackingMenu::currentInfoFunction = [info]() { return info; };
                }
              };
              if (sat.hasOrbitalElements()) {
                fetchAndContinueFunction();
              } else {
                AsyncQueue::addToQueue(fetchAndContinueFunction);
              }
            },
            []() {},
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
            std::string downloadingInfo = "NORAD ID: " + noradId + "\nDownloading...";
            TrackingMenu::currentInfoFunction = [downloadingInfo]() { return downloadingInfo; };
            AsyncQueue::addToQueue([&tracker, noradId, urlFetchFunction]() -> void {
              bool success = currentOrbit.fetchElements(urlFetchFunction);
              if (success) {
                std::string info = getSatelliteInfo(currentOrbit);
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
