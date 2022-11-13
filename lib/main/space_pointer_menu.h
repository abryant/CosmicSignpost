#ifndef SPACEPOINTER_LIB_SPACEPOINTER_SPACE_POINTER_MENU_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_SPACE_POINTER_MENU_H_

#include <memory>

#include "menu.h"
#include "tracker.h"

std::shared_ptr<Menu> buildSpacePointerMenu(Tracker &tracker);

#endif
