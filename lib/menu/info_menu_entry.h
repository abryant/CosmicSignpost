#ifndef SPACEPOINTER_LIB_MENU_INFO_MENU_ENTRY_H_
#define SPACEPOINTER_LIB_MENU_INFO_MENU_ENTRY_H_

#include <functional>
#include <string>

#include "menu_entry.h"

/**
 * Rotary encoder menu entry logic for info screens. Retrieves info from a function when activated.
 */
class InfoMenuEntry : public MenuEntry {
  private:
    std::function<std::string()> infoFunction;

  public:
    InfoMenuEntry(std::string name, std::function<std::string()> infoFunction);
    virtual void onSelect();
    virtual void onBack();
    virtual void onRotateClockwise();
    virtual void onRotateAnticlockwise();
    virtual std::string getDisplayedText();
};

#endif