#include "info_menu_entry.h"

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
  return lastInfo;
}
