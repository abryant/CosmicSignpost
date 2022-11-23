#ifndef COSMIC_SIGNPOST_LIB_MENU_MENU_H_
#define COSMIC_SIGNPOST_LIB_MENU_MENU_H_

#include <functional>
#include <memory>
#include <stdint.h>
#include <string>
#include <utility>
#include <vector>

#include "menu_entry.h"

class Menu : public MenuEntry {
  private:
    std::string title;
    std::vector<std::shared_ptr<MenuEntry>> entries;
    uint32_t currentPosition;
    // The active menu entry does not have to be a menu entry in entries, it could be a follow-on.
    std::shared_ptr<MenuEntry> activeMenuEntry;
    std::string displayedText;
    void updateDisplayedText();

  public:
    Menu(std::string name, std::vector<std::shared_ptr<MenuEntry>> entries);
    Menu(std::string name, std::string title, std::vector<std::shared_ptr<MenuEntry>> entries);
    void deactivateChild(bool goToFollowOn);
    virtual void onSelect();
    virtual void onBack();
    virtual void onRotateClockwise();
    virtual void onRotateAnticlockwise();
    virtual std::string getDisplayedText();
};

#endif
