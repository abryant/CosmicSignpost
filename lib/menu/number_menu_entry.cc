#include "number_menu_entry.h"

NumberMenuEntry::NumberMenuEntry(std::string name, std::string numberFormatString, std::function<void(std::string)> submitFunction)
  : MenuEntry(name) {
  this->numberFormatString = numberFormatString;
  this->submitFunction = submitFunction;

  for (uint32_t i = 0; i < numberFormatString.length(); ++i) {
    if (numberFormatString.at(i) == DIGIT_FORMAT_CHAR) {
      digitsByPosition.push_back(std::make_pair(i, '0'));
    }
    if (numberFormatString.at(i) == SIGN_FORMAT_CHAR) {
      digitsByPosition.push_back(std::make_pair(i, '+'));
    }
  }
}

void NumberMenuEntry::updatedFormattedString() {
  std::string formatted = numberFormatString;
  for (auto it = digitsByPosition.begin(); it != digitsByPosition.end(); ++it) {
    formatted[it->first] = it->second;
  }
  formattedString = formatted;
}

void NumberMenuEntry::onSelect() {
  if (currentDigitIndex == digitsByPosition.size() - 1) {
    submitFunction(formattedString);
    deactivate();
    return;
  }
  currentDigitIndex++;
}

void NumberMenuEntry::onBack() {
  if (currentDigitIndex == 0) {
    deactivate();
    return;
  }
  currentDigitIndex--;
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
  return formattedString;
}
