#ifndef SPACEPOINTER_LIB_MENU_MENU_ENTRY_H_
#define SPACEPOINTER_LIB_MENU_MENU_ENTRY_H_

#include <string>

#include "menu.h"

class MenuEntry {
  private:
    std::string name;
    Menu *parent;

  protected:
    void setName(std::string name);

  public:
    MenuEntry(std::string name);
    std::string getName();
    virtual void onActivate(Menu *parent);
    void deactivate();
    virtual void onSelect() = 0;
    virtual void onBack() = 0;
    virtual void onRotateClockwise() = 0;
    virtual void onRotateAnticlockwise() = 0;
    virtual std::string getDisplayedText() = 0;
};

#endif
