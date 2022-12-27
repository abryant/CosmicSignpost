#include "boolean_menu_entry.h"

BooleanMenuEntry::BooleanMenuEntry(std::string name, std::function<void(bool)> updateFunction, bool initialState, std::string disabledText, bool enabled)
  : MenuEntry(""), name(name), updateFunction(updateFunction), state(initialState), disabledText(disabledText), enabled(enabled) {
  updateMenuEntryName();
}

void BooleanMenuEntry::updateMenuEntryName() {
  setName(name + ": " + (enabled ? (state ? "ON" : "OFF") : disabledText));
}

void BooleanMenuEntry::setEnabled(bool enabled) {
  this->enabled = enabled;
  updateMenuEntryName();
}

void BooleanMenuEntry::setState(bool newState) {
  state = newState;
  updateMenuEntryName();
  updateFunction(state);
}

void BooleanMenuEntry::onActivate(Menu *parent) {
  MenuEntry::onActivate(parent);
  if (enabled) {
    setState(!state);
  }
  deactivate(/* goToFollowOn= */ true);
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
