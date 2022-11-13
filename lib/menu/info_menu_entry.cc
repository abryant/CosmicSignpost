#include "info_menu_entry.h"

#include <sstream>

#include "Arduino.h"

InfoMenuEntry::InfoMenuEntry(
  std::string name,
  std::function<std::string()> infoFunction,
  int64_t updateIntervalMicros)
  : MenuEntry(name),
    infoFunction(infoFunction),
    updateIntervalMicros(updateIntervalMicros),
    lastInfo(""),
    lastUpdateMicros(0) {
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
  int64_t time = micros();
  if (time - lastUpdateMicros > updateIntervalMicros) {
    lastInfo = infoFunction();
    lastUpdateMicros = time;
  }
  // For two-line info entries, don't show the title.
  if (lastInfo[lastInfo.find('\n')] == '\n') {
    return lastInfo;
  }
  std::ostringstream display;
  display << getName() << "\n";
  display << lastInfo;
  return display.str();
}
