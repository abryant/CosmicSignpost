#include "brightness_menu_entry.h"

#include <sstream>

#include "output_devices.h"

BrightnessMenuEntry::BrightnessMenuEntry()
  : MenuEntry("Brightness"),
    brightnessPercent(100) {
}

void BrightnessMenuEntry::onActivate(Menu *parent) {
  MenuEntry::onActivate(parent);
}

void BrightnessMenuEntry::setBrightness() {
  uint8_t scaledRed = ((int) brightnessPercent * 255) / 100;
  OutputDevices::setBacklightColour(scaledRed, 0, 0);
}

void BrightnessMenuEntry::onSelect() {
  deactivate(/* goToFollowOn= */ true);
}

void BrightnessMenuEntry::onBack() {
  deactivate(/* goToFollowOn= */ false);
}

void BrightnessMenuEntry::onRotateClockwise() {
  brightnessPercent = std::min(brightnessPercent + 5, 100);
  setBrightness();
}

void BrightnessMenuEntry::onRotateAnticlockwise() {
  brightnessPercent = std::max(brightnessPercent - 5, 0);
  setBrightness();
}

std::string BrightnessMenuEntry::getDisplayedText() {
  std::ostringstream ss;
  ss << "Brightness\n";
  ss << (int) brightnessPercent << "%";
  return ss.str();
}
