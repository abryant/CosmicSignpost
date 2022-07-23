#include "action_menu_entry.h"

ActionMenuEntry::ActionMenuEntry(std::string name, std::function<void()> activatedFunction)
  : MenuEntry(name), activatedFunction(activatedFunction) {
}

void ActionMenuEntry::onActivate(Menu *parent) {
  MenuEntry::onActivate(parent);
  activatedFunction();
  deactivate();
}

// This menu entry deactivates itself as soon as it's activated, so it won't get any events or
// display its own text.
void ActionMenuEntry::onSelect() {
}

void ActionMenuEntry::onBack() {
}

void ActionMenuEntry::onRotateClockwise() {
}

void ActionMenuEntry::onRotateAnticlockwise() {
}

std::string ActionMenuEntry::getDisplayedText() {
  return "";
}
