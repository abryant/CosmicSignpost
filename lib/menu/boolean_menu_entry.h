#ifndef SPACEPOINTER_LIB_MENU_BOOLEAN_MENU_ENTRY_H_
#define SPACEPOINTER_LIB_MENU_BOOLEAN_MENU_ENTRY_H_

#include <functional>
#include <string>

#include "menu_entry.h"

/**
 * Rotary encoder entry logic for booleans. Displays current state in menu entry.
 */
class BooleanMenuEntry : public MenuEntry {
  private:
    std::string name;
    std::function<void(bool)> updateFunction;
    bool state;
    void updateMenuEntryName();

  public:
    BooleanMenuEntry(std::string name, std::function<void(bool)> updateFunction, bool initialState);
    virtual void onActivate(Menu *parent);
    virtual void onSelect();
    virtual void onBack();
    virtual void onRotateClockwise();
    virtual void onRotateAnticlockwise();
    virtual std::string getDisplayedText();
};

#endif
