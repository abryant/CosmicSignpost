#include "info_menu_entry.h"

#include <sstream>
#include <iomanip>
#include <cmath>

#include "Arduino.h"

#include "output_devices.h"

InfoMenuEntry::InfoMenuEntry(
  std::string name,
  std::function<std::string()> infoFunction,
  uint64_t updateIntervalMicros)
  : MenuEntry(name),
    infoFunction(infoFunction),
    updateIntervalMicros(updateIntervalMicros),
    lastInfo(""),
    lastUpdateMicros(0),
    scrollPosition(0) {
}

void InfoMenuEntry::onSelect() {
  deactivate(/* goToFollowOn= */ true);
}

void InfoMenuEntry::onBack() {
  deactivate(/* goToFollowOn= */ false);
}

void InfoMenuEntry::onRotateClockwise() {
  // This may go past the limit, but getScrolledText() will keep it in bounds.
  scrollPosition++;
}

void InfoMenuEntry::onRotateAnticlockwise() {
  if (scrollPosition > 0) {
    scrollPosition--;
  }
}

std::string InfoMenuEntry::getDisplayedText() {
  uint64_t time = micros();
  if (time - lastUpdateMicros > updateIntervalMicros) {
    lastInfo = infoFunction();
    lastUpdateMicros = time;
  }
  // For two-line info entries, show scrolled text and no title.
  if (lastInfo.find('\n') != std::string::npos) {
    return getScrolledText(lastInfo);
  }
  std::ostringstream display;
  display << getName() << "\n";
  display << lastInfo;
  return display.str();
}

std::string InfoMenuEntry::getScrolledText(std::string text) {
  std::vector<std::string> lines = {};
  std::istringstream input(text);
  std::string item;
  while (std::getline(input, item)) {
    // Limit scrolled lines to 15 characters, so we can show the scroll bar.
    lines.push_back(item.substr(0, OutputDevices::DISPLAY_LENGTH - 1));
  }
  // scrollPosition is the index of the top line on the screen. It can't go higher than the second
  // last line's index (i.e. size - 2).
  scrollPosition = std::min<size_t>(scrollPosition, lines.size() - 2);

  std::ostringstream display;
  display << lines[scrollPosition];
  display << std::string(OutputDevices::DISPLAY_LENGTH - 1 - OutputDevices::countChars(lines[scrollPosition]), ' ');
  display << (scrollPosition > 0 ? "↑" : " ");
  display << "\n";
  display << lines[scrollPosition + 1];
  display << std::string(OutputDevices::DISPLAY_LENGTH - 1 - OutputDevices::countChars(lines[scrollPosition + 1]), ' ');
  display << (scrollPosition < lines.size() - 2 ? "↓" : " ");
  return display.str();
}
