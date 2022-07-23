#include "info_menu_entry.h"

InfoMenuEntry::InfoMenuEntry(std::string name, std::function<std::string()> infoFunction)
  : MenuEntry(name), infoFunction(infoFunction) {
}

void InfoMenuEntry::onSelect() {
  deactivate();
}

void InfoMenuEntry::onBack() {
  deactivate();
}

void InfoMenuEntry::onRotateClockwise() {
}

void InfoMenuEntry::onRotateAnticlockwise() {
}

std::string InfoMenuEntry::getDisplayedText() {
  return infoFunction();
}
