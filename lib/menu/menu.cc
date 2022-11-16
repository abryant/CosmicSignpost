#include "menu.h"

#include <functional>
#include <stdint.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "output_devices.h"

const uint32_t MAX_ENTRY_LENGTH = OutputDevices::DISPLAY_LENGTH - 4;

Menu::Menu(std::string name, std::vector<std::shared_ptr<MenuEntry>> entries)
    : Menu(name, name, entries) {}

Menu::Menu(std::string name, std::string title, std::vector<std::shared_ptr<MenuEntry>> entries)
    : MenuEntry(name),
      title(title),
      entries(entries),
      currentPosition(0),
      activeMenuEntry(NULL),
      displayedText("") {
  updateDisplayedText();
}

std::string centreText(std::string text, int32_t totalLength) {
  int32_t currentLength = text.length();
  int32_t prefixLength = (totalLength - currentLength) / 2;
  return std::string(prefixLength, ' ')
      + text
      + std::string(totalLength - prefixLength - currentLength, ' ');
}

void Menu::updateDisplayedText() {
  std::ostringstream display;
  display << centreText(title, OutputDevices::DISPLAY_LENGTH) << "\n";
  display << (currentPosition == 0 ? "  " : "< ");
  std::string name = entries[currentPosition]->getName();
  name = name.substr(0, MAX_ENTRY_LENGTH);
  display << centreText(name, MAX_ENTRY_LENGTH);
  display << (currentPosition == entries.size() - 1 ? "  " : " >");
  displayedText = display.str();
}

void Menu::deactivateChild(bool goToFollowOn) {
  if (goToFollowOn) {
    activeMenuEntry = activeMenuEntry->getFollowOnMenuEntry();
    if (activeMenuEntry != NULL) {
      activeMenuEntry->onActivate(this);
      updateDisplayedText();
    }
  } else {
    activeMenuEntry = NULL;
  }
}

std::string Menu::getDisplayedText() {
  if (activeMenuEntry != NULL) {
    return activeMenuEntry->getDisplayedText();
  }
  return displayedText;
}

void Menu::onBack() {
  if (activeMenuEntry != NULL) {
    activeMenuEntry->onBack();
    updateDisplayedText();
    return;
  }
  currentPosition = 0;
  updateDisplayedText();
  if (hasParent()) {
    // The back button generally doesn't produce the follow-on, so there isn't an obvious use-case
    // for a follow-on for a menu.
    deactivate(/* goToFollowOn= */ false);
  }
}

void Menu::onRotateClockwise() {
  if (activeMenuEntry != NULL) {
    activeMenuEntry->onRotateClockwise();
    updateDisplayedText();
    return;
  }
  if (this->currentPosition < entries.size() - 1) {
    this->currentPosition++;
    updateDisplayedText();
  }
}

void Menu::onRotateAnticlockwise() {
  if (activeMenuEntry != NULL) {
    activeMenuEntry->onRotateAnticlockwise();
    updateDisplayedText();
    return;
  }
  if (this->currentPosition > 0) {
    this->currentPosition--;
    updateDisplayedText();
  }
}

void Menu::onSelect() {
  if (activeMenuEntry != NULL) {
    activeMenuEntry->onSelect();
    updateDisplayedText();
    return;
  }
  activeMenuEntry = entries[currentPosition];
  activeMenuEntry->onActivate(this);
  updateDisplayedText();
}
