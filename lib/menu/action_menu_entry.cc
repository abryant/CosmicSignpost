#include "action_menu_entry.h"

ActionMenuEntry::ActionMenuEntry(
    std::string name,
    std::function<void()> activatedFunction,
    std::function<void()> deactivatedFunction,
    std::function<std::string()> infoFunction,
    uint64_t updateIntervalMicros)
  : InfoMenuEntry(name, infoFunction, updateIntervalMicros),
    activatedFunction(activatedFunction),
    deactivatedFunction(deactivatedFunction) {
}

void ActionMenuEntry::onActivate(Menu *parent) {
  InfoMenuEntry::onActivate(parent);
  activatedFunction();
}

void ActionMenuEntry::onDeactivate() {
  deactivatedFunction();
}
