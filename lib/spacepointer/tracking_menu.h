#ifndef SPACEPOINTER_LIB_SPACEPOINTER_TRACKING_MENU_H_
#define SPACEPOINTER_LIB_SPACEPOINTER_TRACKING_MENU_H_

#include <memory>

#include "menu.h"
#include "trackable_objects.h"

typedef std::function<void(TrackableObjects::tracking_function)> tracking_selector_function;

std::shared_ptr<Menu> buildTrackingMenu(tracking_selector_function chooseTrackedObject);

#endif
