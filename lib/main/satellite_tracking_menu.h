#ifndef SPACEPOINTER_LIB_MAIN_SATELLITE_TRACKING_MENU_H_
#define SPACEPOINTER_LIB_MAIN_SATELLITE_TRACKING_MENU_H_

#include <functional>
#include <memory>
#include <string>

#include "menu.h"
#include "tracker.h"

namespace SatelliteTrackingMenu {

  std::shared_ptr<Menu> buildSatellitesMenu(
      std::string menuName,
      std::string menuTitle,
      std::vector<std::string> satelliteNames,
      Tracker &tracker,
      std::function<std::optional<std::string>(std::string)> urlFetchFunction);

  std::shared_ptr<MenuEntry> buildManualNoradIdEntry(
      Tracker &tracker,
      std::shared_ptr<MenuEntry> currentInfoEntry,
      std::function<std::optional<std::string>(std::string)> urlFetchFunction);

  std::shared_ptr<Menu> buildSatelliteTypesMenu(
      Tracker &tracker,
      std::shared_ptr<MenuEntry> currentInfoEntry,
      std::function<std::optional<std::string>(std::string)> urlFetchFunction);
}

#endif
