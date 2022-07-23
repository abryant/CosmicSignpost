#ifndef SPACEPOINTER_LIB_MENU_ACTION_MENU_ENTRY_H_
#define SPACEPOINTER_LIB_MENU_ACTION_MENU_ENTRY_H_

#include <functional>
#include <string>

#include "menu_entry.h"

/**
 * Rotary encoder menu entry logic for actions. Calls a function when activated.
 */
class ActionMenuEntry : public MenuEntry {
  private:
    std::function<void()> activatedFunction;

  public:
    ActionMenuEntry(std::string name, std::function<void()> activatedFunction);
    virtual void onActivate(Menu *parent);
    virtual void onSelect();
    virtual void onBack();
    virtual void onRotateClockwise();
    virtual void onRotateAnticlockwise();
    virtual std::string getDisplayedText();
};

#endif
