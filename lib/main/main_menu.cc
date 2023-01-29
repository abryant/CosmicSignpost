#include "main_menu.h"

#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "Arduino.h"
#include "WiFi.h"

#include "menu.h"
#include "menu_entry.h"
#include "action_menu_entry.h"
#include "boolean_menu_entry.h"
#include "brightness_menu_entry.h"
#include "info_menu_entry.h"
#include "number_menu_entry.h"
#include "tracking_menu.h"
#include "tracker.h"

#include "config.h"
#include "gps.h"
#include "orientation.h"

std::shared_ptr<BooleanMenuEntry> main_menu::gpsEnabledMenuEntry;

void main_menu::updateGpsMenuEntry(bool gpsActive) {
  if (gpsEnabledMenuEntry != NULL) {
    gpsEnabledMenuEntry->setEnabled(gpsActive);
  }
}

std::shared_ptr<Menu> main_menu::buildInfoMenu(Tracker &tracker) {
  std::vector<std::shared_ptr<MenuEntry>> infoEntries = {
    std::make_shared<InfoMenuEntry>("Lat/Long/El", [&tracker]() {
      Location location = tracker.getCurrentLocation();
      std::ostringstream locationStr;
      locationStr << "Lat: ";
      locationStr << std::fixed << std::setprecision(5);
      locationStr << std::right << std::setw(9) << location.getLatitude();
      locationStr << "N\nLng:";
      locationStr << std::fixed << std::setprecision(5);
      locationStr << std::right << std::setw(10) << location.getLongitude();
      locationStr << "E\nElev: ";
      locationStr << std::fixed << std::setprecision(0);
      locationStr << std::right << std::setw(6) << location.getElevation();
      locationStr << "m";
      return locationStr.str();
    }),
    std::make_shared<InfoMenuEntry>("Date/Time", []() {
      std::time_t time = std::time(nullptr);
      std::tm *utcTime = std::gmtime(&time);
      std::ostringstream timeStr;
      timeStr << std::put_time(utcTime, "%Y-%m-%d\n%H:%M:%S");
      return timeStr.str();
    }),
    std::make_shared<InfoMenuEntry>("Wireless IP", []() {
      return std::string(WiFi.localIP().toString().c_str());
    }),
  };
  return std::make_shared<Menu>("Info", infoEntries);
}

std::shared_ptr<BooleanMenuEntry> main_menu::buildGpsEnabledMenuEntry() {
  return std::make_shared<BooleanMenuEntry>(
      "GPS",
      [](bool newValue) {
        gps::setEnabled(newValue);
      },
      /* initialState= */ true,
      /* disabledText= */ "N/A",
      /* enabled= */ true);
}

std::shared_ptr<MenuEntry> main_menu::buildSetCurrentLocationEntry(Tracker &tracker) {
  static std::string lastEnteredGpsLocation = "";
  std::shared_ptr<MenuEntry> manualGpsMenuEntry =
      std::make_shared<NumberMenuEntry>(
          "Current Loc",
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
            Location newLocation = Location(latitude, longitude, elevation);
            config::writeDefaultLocation(newLocation);
            gpsEnabledMenuEntry->setState(false);
            tracker.setCurrentLocation(newLocation);
          });
  manualGpsMenuEntry->setFollowOnMenuEntry(elevationMenuEntry);
  return manualGpsMenuEntry;
}

std::shared_ptr<MenuEntry> main_menu::buildCalibrateCompassEntry(Tracker &tracker) {
  std::shared_ptr<MenuEntry> calibrateMenuEntry =
      std::make_shared<ActionMenuEntry>(
          "Compass Cal",
          [&tracker]() {
            // reset calibration state
            orientation::init();
            orientation::calibration::startCalibration(tracker);
          },
          [&tracker]() {
            orientation::calibration::stopCalibration(tracker);
          },
          []() {
            if (!orientation::calibration::calibrating) {
              return std::string("Done");
            }
            std::ostringstream orientationStr;
            if (orientation::connected) {
              orientationStr << std::to_string(orientation::calibration::systemCalibrationStatus) << " ";
              orientationStr << std::to_string(orientation::calibration::gyroscopeCalibrationStatus) << " ";
              orientationStr << std::to_string(orientation::calibration::accelerometerCalibrationStatus) << " ";
              orientationStr << std::to_string(orientation::calibration::magnetometerCalibrationStatus);
            } else {
              orientationStr << "No Compass";
            }
            return orientationStr.str();
          });
  return calibrateMenuEntry;
}

std::shared_ptr<Menu> main_menu::buildConfigMenu(Tracker &tracker) {
  gpsEnabledMenuEntry = buildGpsEnabledMenuEntry();
  std::vector<std::shared_ptr<MenuEntry>> sensorsEntries = {
    gpsEnabledMenuEntry,
    buildSetCurrentLocationEntry(tracker),
    buildCalibrateCompassEntry(tracker),
    std::make_shared<BooleanMenuEntry>("Spin", [&tracker](bool newValue) {
      tracker.setSpinning(newValue);
    }, false),
  };
  return std::make_shared<Menu>("Config", sensorsEntries);
}

std::shared_ptr<Menu> main_menu::buildMainMenu(Tracker &tracker, std::function<std::optional<std::string>(std::string)> urlFetchFunction) {
  std::vector<std::shared_ptr<MenuEntry>> mainEntries = {
    TrackingMenu::buildTrackingMenu(tracker, urlFetchFunction),
    buildConfigMenu(tracker),
    std::make_shared<BrightnessMenuEntry>(),
    buildInfoMenu(tracker),
  };
  return std::make_shared<Menu>("Cosmic Signpost", mainEntries);
}
