#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TRACKING_MENU_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TRACKING_MENU_H_

#include <memory>

#include "menu.h"
#include "tracker.h"

std::shared_ptr<Menu> buildTrackingMenu(Tracker &tracker, std::function<std::optional<std::string>(std::string)> urlFetchFunction);

#endif
