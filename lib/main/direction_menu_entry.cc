#include "direction_menu_entry.h"

#include <functional>
#include <sstream>
#include <iomanip>
#include <cmath>

DirectionMenuEntry::DirectionMenuEntry(std::string name, Tracker &tracker)
  : MenuEntry(name),
    tracker(tracker),
    currentAzimuth(0),
    currentAltitude(0),
    adjustingAzimuth(true) {
}

Direction DirectionMenuEntry::getCurrentDirection(int64_t timeMillis) {
  return Direction(currentAzimuth, currentAltitude);
}

void DirectionMenuEntry::onActivate(Menu *parent) {
  MenuEntry::onActivate(parent);
  tracker.setDirectionFunction([this](int64_t timeMillis) {
    return this->getCurrentDirection(timeMillis);
  });
}

void DirectionMenuEntry::onSelect() {
  adjustingAzimuth = !adjustingAzimuth;
}

void DirectionMenuEntry::onBack() {
  tracker.setDirectionFunction(std::nullopt);
  deactivate(/* goToFollowOn= */ false);
}

void DirectionMenuEntry::onRotateClockwise() {
  if (adjustingAzimuth) {
    currentAzimuth = (currentAzimuth + 5) % 360;
  } else {
    currentAltitude = std::min(currentAltitude + 5, 90);
  }
}

void DirectionMenuEntry::onRotateAnticlockwise() {
  if (adjustingAzimuth) {
    currentAzimuth = (currentAzimuth - 5) + ((currentAzimuth - 5) < 0 ? 360 : 0);
  } else {
    currentAltitude = std::max(currentAltitude - 5, -90);
  }
}

std::string DirectionMenuEntry::getDisplayedText() {
  std::ostringstream display;
  display << (adjustingAzimuth ? "> " : "  ");
  display << "Azimuth: ";
  display << currentAzimuth;
  display << "\n";
  display << (adjustingAzimuth ? "  " : "> ");
  display << "Altitude: ";
  display << currentAltitude;
  return display.str();
}
