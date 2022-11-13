#include "action_menu_entry.h"

ActionMenuEntry::ActionMenuEntry(
    std::string name,
    std::function<void()> activatedFunction,
    std::function<std::string()> infoFunction,
    int64_t updateIntervalMicros)
  : InfoMenuEntry(name, infoFunction, updateIntervalMicros),
    activatedFunction(activatedFunction) {
}

void ActionMenuEntry::onActivate(Menu *parent) {
  InfoMenuEntry::onActivate(parent);
  activatedFunction();
}
