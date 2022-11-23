#ifndef COSMIC_SIGNPOST_LIB_MENU_INFO_MENU_ENTRY_H_
#define COSMIC_SIGNPOST_LIB_MENU_INFO_MENU_ENTRY_H_

#include <functional>
#include <string>
#include <stdint.h>

#include "menu_entry.h"

/**
 * Rotary encoder menu entry logic for info screens. Retrieves info from a function when activated.
 */
class InfoMenuEntry : public MenuEntry {
  private:
    std::function<std::string()> infoFunction;
    int64_t updateIntervalMicros;
    std::string lastInfo;
    int64_t lastUpdateMicros;
    size_t scrollPosition;

  public:
    InfoMenuEntry(
        std::string name,
        std::function<std::string()> infoFunction,
        int64_t updateIntervalMicros = 0);
    virtual void onSelect();
    virtual void onBack();
    virtual void onRotateClockwise();
    virtual void onRotateAnticlockwise();
    virtual std::string getDisplayedText();
    std::string getScrolledText(std::string text);
};

#endif
