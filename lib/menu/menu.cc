#include "menu.h"

#include <functional>
#include <stdint.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

const uint32_t DISPLAY_LENGTH = 16;
const uint32_t MAX_ENTRY_LENGTH = DISPLAY_LENGTH - 4;

Menu::Menu(std::string name, std::vector<std::shared_ptr<MenuEntry>> entries)
    : Menu(name, name, entries) {}

Menu::Menu(std::string name, std::string title, std::vector<std::shared_ptr<MenuEntry>> entries)
    : MenuEntry(name),
      title(title),
      entries(entries),
      currentPosition(0),
      isCurrentActive(false),
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
  display << centreText(title, DISPLAY_LENGTH) << "\n";
  display << (currentPosition == 0 ? "  " : "< ");
  std::string name = entries[currentPosition]->getName();
  name = name.substr(0, MAX_ENTRY_LENGTH);
  display << centreText(name, MAX_ENTRY_LENGTH);
  display << (currentPosition == entries.size() - 1 ? "  " : " >");
  displayedText = display.str();
}

void Menu::deactivateChild() {
  isCurrentActive = false;
}

std::string Menu::getDisplayedText() {
  if (isCurrentActive) {
    return entries[currentPosition]->getDisplayedText();
  }
  return displayedText;
}

void Menu::onBack() {
  if (isCurrentActive) {
    entries[currentPosition]->onBack();
    updateDisplayedText();
    return;
  }
  if (hasParent()) {
    deactivate();
  } else {
    currentPosition = 0;
    updateDisplayedText();
  }
}

void Menu::onRotateClockwise() {
  if (isCurrentActive) {
    entries[currentPosition]->onRotateClockwise();
    updateDisplayedText();
    return;
  }
  if (this->currentPosition < entries.size() - 1) {
    this->currentPosition++;
    updateDisplayedText();
  }
}

void Menu::onRotateAnticlockwise() {
  if (isCurrentActive) {
    entries[currentPosition]->onRotateAnticlockwise();
    updateDisplayedText();
    return;
  }
  if (this->currentPosition > 0) {
    this->currentPosition--;
    updateDisplayedText();
  }
}

void Menu::onSelect() {
  if (isCurrentActive) {
    entries[currentPosition]->onSelect();
    updateDisplayedText();
    return;
  }
  isCurrentActive = true;
  entries[currentPosition]->onActivate(this);
  updateDisplayedText();
}
