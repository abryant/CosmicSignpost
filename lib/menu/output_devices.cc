#include "output_devices.h"

#include <sstream>

#include <Arduino.h>
#include <Wire.h>

uint8_t OutputDevices::lcdAddress;
std::string OutputDevices::lastString;
std::vector<uint8_t> OutputDevices::settings;

uint8_t ROW_OFFSETS[4] = {0x00, 0x40, 0x14, 0x54};

void OutputDevices::initLcd(uint8_t lcdAddress) {
  OutputDevices::lcdAddress = lcdAddress;
  OutputDevices::lastString = "";

  // Send the setup code in batches, because we can't send more than 31 bytes at a time.
  settings = {};
  disableSystemMessages();
  createUpDownArrows();
  sendToLcd(settings);

  settings = {};
  createDegreesSymbol();
  setTwoLines();
  disableCursor();
  setBacklightColour(255, 0, 0);
  sendToLcd(settings);

  settings = {};
}

void replaceAll(std::string &str, std::string from, std::string to) {
  for (size_t startPos = str.find(from, 0);
       startPos != std::string::npos;
       startPos = str.find(from, startPos + to.length())) {
    str.replace(startPos, from.length(), to);
  }
}

void OutputDevices::display(std::string str) {
  if (str == lastString && settings.size() == 0) {
    return;
  }
  std::istringstream input(str);
  std::string line1;
  std::string line2;
  std::getline(input, line1);
  std::getline(input, line2);
  replaceAll(line1, "↑", "\x7C\x23");
  replaceAll(line1, "↓", "\x7C\x24");
  replaceAll(line1, "°", "\x7C\x25");
  replaceAll(line2, "↑", "\x7C\x23");
  replaceAll(line2, "↓", "\x7C\x24");
  replaceAll(line2, "°", "\x7C\x25");

  std::vector<uint8_t> data {
    0x7C, // Setting mode
    0x2D  // Clear and home
  };
  data.insert(data.end(), line1.begin(), line1.end());
  sendToLcd(data);
  data.clear();

  if (line2.length() > 0) {
    std::vector<uint8_t> changeLine {
      254, // Special mode
      (uint8_t) (0x80 | ROW_OFFSETS[1]) // Set cursor position to start of second line.
    };
    data.insert(data.end(), changeLine.begin(), changeLine.end());
    data.insert(data.end(), line2.begin(), line2.end());
    sendToLcd(data);
  }
  lastString = str;
  if (settings.size() > 0) {
    sendToLcd(settings);
    settings = {};
  }
}

void OutputDevices::disableSystemMessages() {
  std::vector<uint8_t> twoLinesSettings = {
    0x7C, // Setting mode
    0x2F, // Disable system messages
  };
  settings.insert(settings.end(), twoLinesSettings.begin(), twoLinesSettings.end());
}

void OutputDevices::createUpDownArrows() {
  std::vector<uint8_t> upDownArrows = {
    0x7C, // Setting mode
    0x1B, // Write first custom char, accessible as 0x7C,0x23
    0x04, 0x0E, 0x15, 0x04, 0x04, 0x04, 0x04, 0x04, // Bitmap for up arrow
    0x7C, // Setting mode
    0x1C, // Write second custom char, accessible as 0x7C,0x24
    0x04, 0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04, // Bitmap for down arrow
  };
  settings.insert(settings.end(), upDownArrows.begin(), upDownArrows.end());
}

void OutputDevices::createDegreesSymbol() {
  std::vector<uint8_t> degreesSymbol = {
    0x7C, // Setting mode
    0x1D, // Write first custom char, accessible as 0x7C,0x25
    0x0C, 0x12, 0x12, 0x0C, 0x00, 0x00, 0x00, 0x00, // Bitmap for degrees symbol
  };
  settings.insert(settings.end(), degreesSymbol.begin(), degreesSymbol.end());
}

void OutputDevices::setTwoLines() {
  std::vector<uint8_t> twoLinesSettings = {
    0x7C, // Setting mode
    0x06, // 2 lines
  };
  settings.insert(settings.end(), twoLinesSettings.begin(), twoLinesSettings.end());
}

void OutputDevices::setCursor(uint8_t row, uint8_t column) {
  std::vector<uint8_t> cursorSettings = {
    254, // Special mode
    0x08 | 0x04 | 0x02, // Display control: Display on, Cursor on
    254, // Special mode
    (uint8_t) (0x80 | (uint8_t) (ROW_OFFSETS[row] + column)) // Set cursor
  };
  settings.insert(settings.end(), cursorSettings.begin(), cursorSettings.end());
}

void OutputDevices::disableCursor() {
  std::vector<uint8_t> cursorSettings = {
    254, // Special mode
    0x08 | 0x04, // Display control: Display on, Cursor off
  };
  settings.insert(settings.end(), cursorSettings.begin(), cursorSettings.end());
}

void OutputDevices::displayAndSetSplashScreen(std::string str) {
  std::vector<uint8_t> cursorSettings = {
    0x7C, // Setting mode
    0x0A, // Save current display as splash screen
  };
  settings.insert(settings.end(), cursorSettings.begin(), cursorSettings.end());
  display(str);
}

void OutputDevices::setBacklightColour(uint8_t red, uint8_t green, uint8_t blue) {
  std::vector<uint8_t> colourSettings = {
    0x7C, // Setting mode
    '+', // Set colour
    red, green, blue
  };
  settings.insert(settings.end(), colourSettings.begin(), colourSettings.end());
}

int32_t OutputDevices::countChars(std::string str) {
  int32_t count = 0;
  for (char c : str) {
    // The first byte of a unicode character starts with either 0 or 11.
    // All other bytes start with 10.
    if (!(c & 0x80) || (c & 0x40)) {
      count++;
    }
  }
  return count;
}

void OutputDevices::sendToLcd(std::vector<uint8_t> data) {
  Wire.beginTransmission(OutputDevices::lcdAddress);
  Wire.write(&data[0], data.size());
  Wire.endTransmission();
}
