#ifndef SPACEPOINTER_LIB_MENU_NUMBER_MENU_ENTRY_H_
#define SPACEPOINTER_LIB_MENU_NUMBER_MENU_ENTRY_H_

#include <functional>
#include <string>
#include <vector>

#include "menu_entry.h"

/**
 * Rotary encoder entry logic for numbers. Supports arbitrary number formats, by filling in blanks
 * in a format string with digits.
 */
class NumberMenuEntry : public MenuEntry {
  private:
    std::string numberFormatString;
    // The positions of digits in the format string, and their current values. Ordered by position.
    // May also contain signs (+/-).
    std::vector<std::pair<uint32_t, char>> digitsByPosition;
    uint32_t currentDigitIndex; // index into digitsByPosition
    std::string formattedString;
    std::function<void(std::string)> submitFunction;
    void updatedFormattedString();

  public:
    static const char SIGN_FORMAT_CHAR = '~';
    static const char DIGIT_FORMAT_CHAR = '#';
    NumberMenuEntry(std::string name, std::string numberFormatString, std::function<void(std::string)> submitFunction);
    virtual void onSelect();
    virtual void onBack();
    virtual void onRotateClockwise();
    virtual void onRotateAnticlockwise();
    virtual std::string getDisplayedText();
};

#endif
