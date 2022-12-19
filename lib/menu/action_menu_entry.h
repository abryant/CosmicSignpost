#ifndef COSMIC_SIGNPOST_LIB_MENU_ACTION_MENU_ENTRY_H_
#define COSMIC_SIGNPOST_LIB_MENU_ACTION_MENU_ENTRY_H_

#include <functional>
#include <string>

#include "info_menu_entry.h"

/**
 * Rotary encoder menu entry logic for actions.
 *
 * Calls a function when activated, in addition to showing info like an InfoMenuEntry.
 */
class ActionMenuEntry : public InfoMenuEntry {
  private:
    std::function<void()> activatedFunction;

  public:
    ActionMenuEntry(
        std::string name,
        std::function<void()> activatedFunction,
        std::function<std::string()> infoFunction,
        uint64_t updateIntervalMicros = 0);
    virtual void onActivate(Menu *parent);
};

#endif
