#ifndef SPACEPOINTER_LIB_SPACEPOINTER_SPACE_POINTER_MENU_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_SPACE_POINTER_MENU_H_

#include <memory>

#include "menu.h"

std::shared_ptr<Menu> buildSpacePointerMenu(std::function<void(int32_t)> updateAngle);

#endif
