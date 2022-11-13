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
    std::pair<int32_t, char> indexAndDigit = digitsByPosition[i];
    formatted[indexAndDigit.first] = indexAndDigit.second;
    if (i == currentDigitIndex) {
      cursorPos = indexAndDigit.first;
    }
  }
  OutputDevices::setCursor(0, cursorPos);
  formattedString = formatted;
}

void NumberMenuEntry::onSelect() {
  if (currentDigitIndex == digitsByPosition.size() - 1) {
    submitFunction(formattedString);
    OutputDevices::disableCursor();
    deactivate();
    return;
  }
  currentDigitIndex++;
  updatedFormattedString();
}

void NumberMenuEntry::onBack() {
  if (currentDigitIndex == 0) {
    OutputDevices::disableCursor();
    deactivate();
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
  ss << getName() << "\n";
  ss << formattedString;
  return ss.str();
}
