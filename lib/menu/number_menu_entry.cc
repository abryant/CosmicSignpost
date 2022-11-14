#include "number_menu_entry.h"

#include <sstream>

#include "output_devices.h"

NumberMenuEntry::NumberMenuEntry(std::string name, std::string numberFormatString, std::function<void(std::string)> submitFunction)
  : MenuEntry(name) {
  this->numberFormatString = numberFormatString;
  this->submitFunction = submitFunction;
}

void NumberMenuEntry::onActivate(Menu *parent) {
  MenuEntry::onActivate(parent);
  digitsByPosition.clear();
  for (uint32_t i = 0; i < numberFormatString.length(); ++i) {
    if (numberFormatString.at(i) == DIGIT_FORMAT_CHAR) {
      digitsByPosition.push_back(std::make_pair(i, '0'));
    }
    if (numberFormatString.at(i) == SIGN_FORMAT_CHAR) {
      digitsByPosition.push_back(std::make_pair(i, '+'));
    }
  }
  currentDigitIndex = 0;
  updatedFormattedString();
}

void NumberMenuEntry::updatedFormattedString() {
  std::string formatted = numberFormatString;
  int32_t cursorPos = 0;
  for (int32_t i = 0; i < digitsByPosition.size(); ++i) {
    std::pair<uint32_t, char> indexAndDigit = digitsByPosition[i];
    formatted[indexAndDigit.first] = indexAndDigit.second;
    if (i == currentDigitIndex) {
      cursorPos = indexAndDigit.first;
    }
  }
  size_t startOfSecondLinePos = numberFormatString.find('\n') + 1;
  int32_t cursorLine = 0;
  if (cursorPos >= startOfSecondLinePos) {
    cursorLine = 1;
    cursorPos -= startOfSecondLinePos;
  }
  OutputDevices::setCursor(cursorLine, cursorPos);
  formattedString = formatted;
}

void NumberMenuEntry::onSelect() {
  if (currentDigitIndex == digitsByPosition.size() - 1) {
    submitFunction(formattedString);
    OutputDevices::disableCursor();
    deactivate(/* goToFollowOn= */ true);
    return;
  }
  currentDigitIndex++;
  updatedFormattedString();
}

void NumberMenuEntry::onBack() {
  if (currentDigitIndex == 0) {
    OutputDevices::disableCursor();
    deactivate(/* goToFollowOn= */ false);
    return;
  }
  currentDigitIndex--;
  updatedFormattedString();
}

void NumberMenuEntry::onRotateClockwise() {
  std::pair<uint32_t, char> entry = digitsByPosition[currentDigitIndex];
  char current = entry.second;
  if (current == '+') {
    current = '-';
  } else if (current == '-') {
    current = '+';
  } else if (current == '9') {
    current = '0';
  } else {
    current++;
  }
  digitsByPosition[currentDigitIndex] = std::make_pair(entry.first, current);
  updatedFormattedString();
}

void NumberMenuEntry::onRotateAnticlockwise() {
  std::pair<uint32_t, char> entry = digitsByPosition[currentDigitIndex];
  char current = entry.second;
  if (current == '+') {
    current = '-';
  } else if (current == '-') {
    current = '+';
  } else if (current == '0') {
    current = '9';
  } else {
    current--;
  }
  digitsByPosition[currentDigitIndex] = std::make_pair(entry.first, current);
  updatedFormattedString();
}

std::string NumberMenuEntry::getDisplayedText() {
  std::ostringstream ss;
  // For single-line entries, show a title.
  if (formattedString.find('\n') == std::string::npos) {
    ss << getName() << "\n";
  }
  ss << formattedString;
  return ss.str();
}
