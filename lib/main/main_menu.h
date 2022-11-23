#ifndef SPACEPOINTER_LIB_MAIN_MAIN_MENU_H_
#define SPACEPOINTER_LIB_MAIN_MAIN_MENU_H_

#include <memory>

#include "menu.h"
#include "tracker.h"

std::shared_ptr<Menu> buildMainMenu(Tracker &tracker, std::function<std::optional<std::string>(std::string)> urlFetchFunction);

#endif
