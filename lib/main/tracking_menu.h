#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TRACKING_MENU_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TRACKING_MENU_H_

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "menu.h"
#include "tracker.h"

namespace TrackingMenu {
  extern std::function<std::string()> currentInfoFunction;
  extern const int64_t INFO_UPDATE_INTERVAL_MICROS;

  std::string formatDistance(double distanceMetres);
  std::function<std::string()> buildInfoFunction(std::string constantPrefix, Tracker &tracker, bool includeDistance);

  std::shared_ptr<Menu> buildTrackableObjectsMenu(
      std::string menuName,
      std::vector<std::string> trackableObjectNames,
      Tracker &tracker,
      bool includeDistance = true);
  std::shared_ptr<MenuEntry> buildManualGpsMenuEntry(
      Tracker &tracker,
      std::shared_ptr<MenuEntry> currentInfoEntry);
  std::shared_ptr<MenuEntry> buildManualRaDeclMenuEntry(
      Tracker &tracker,
      std::shared_ptr<MenuEntry> currentInfoEntry);
  std::shared_ptr<MenuEntry> buildManualNoradIdEntry(
      Tracker &tracker,
      std::shared_ptr<MenuEntry> currentInfoEntry,
      std::function<std::optional<std::string>(std::string)> urlFetchFunction);
  std::shared_ptr<Menu> buildTrackingMenu(Tracker &tracker, std::function<std::optional<std::string>(std::string)> urlFetchFunction);
}

#endif
