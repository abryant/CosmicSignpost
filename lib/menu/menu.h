#ifndef SPACEPOINTER_LIB_MENU_MENU_H_
#define SPACEPOINTER_LIB_MENU_MENU_H_

#include <functional>
#include <memory>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "menu_entry.h"

class Menu : public MenuEntry {
  private:
    std::vector<std::shared_ptr<MenuEntry>> entries;
    uint32_t currentPosition;
    bool isCurrentActive;
    std::string displayedText;
    void updateDisplayedText();

  public:
    Menu(std::string name, std::vector<std::shared_ptr<MenuEntry>> entries);
    void deactivateChild();
    virtual void onSelect();
    virtual void onBack();
    virtual void onRotateClockwise();
    virtual void onRotateAnticlockwise();
    virtual std::string getDisplayedText();
};

#endif
