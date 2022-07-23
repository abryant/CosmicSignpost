#include "boolean_menu_entry.h"

BooleanMenuEntry::BooleanMenuEntry(std::string name, std::function<void(bool)> updateFunction, bool initialState)
  : MenuEntry(""), name(name), updateFunction(updateFunction), state(initialState) {
  updateMenuEntryName();
}

void BooleanMenuEntry::updateMenuEntryName() {
  setName(name + ": " + (state ? "ON" : "OFF"));
}

void BooleanMenuEntry::onActivate(Menu *parent) {
  MenuEntry::onActivate(parent);
  state = !state;
  updateMenuEntryName();
  updateFunction(state);
  deactivate();
}

// This menu entry deactivates itself as soon as it's activated, so it won't get any events or
// display its own text.
void BooleanMenuEntry::onSelect() {
}

void BooleanMenuEntry::onBack() {
}

void BooleanMenuEntry::onRotateClockwise() {
}

void BooleanMenuEntry::onRotateAnticlockwise() {
}

std::string BooleanMenuEntry::getDisplayedText() {
  return "";
}
