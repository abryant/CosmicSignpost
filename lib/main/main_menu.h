#ifndef COSMIC_SIGNPOST_LIB_MAIN_MAIN_MENU_H_
#define COSMIC_SIGNPOST_LIB_MAIN_MAIN_MENU_H_

#include <functional>
#include <memory>

#include "boolean_menu_entry.h"
#include "menu.h"
#include "menu_entry.h"
#include "tracker.h"

namespace main_menu {
  extern std::shared_ptr<BooleanMenuEntry> gpsEnabledMenuEntry;

  void updateGpsMenuEntry(bool gpsActive);

  std::shared_ptr<Menu> buildMainMenu(Tracker &tracker, std::function<std::optional<std::string>(std::string)> urlFetchFunction);

  std::shared_ptr<Menu> buildInfoMenu(Tracker &tracker);
  std::shared_ptr<BooleanMenuEntry> buildGpsEnabledMenuEntry();
  std::shared_ptr<MenuEntry> buildSetCurrentLocationEntry(Tracker &tracker);
  std::shared_ptr<MenuEntry> buildCalibrateCompassEntry(Tracker &tracker);
  std::shared_ptr<Menu> buildConfigMenu(Tracker &tracker);
}

#endif
