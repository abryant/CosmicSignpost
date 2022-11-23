#ifndef COSMIC_SIGNPOST_LIB_MENU_BRIGHTNESS_MENU_ENTRY_H_
#define COSMIC_SIGNPOST_LIB_MENU_BRIGHTNESS_MENU_ENTRY_H_

#include "menu_entry.h"

/**
 * Menu entry for adjusting the LCD screen brightness.
 */
class BrightnessMenuEntry : public MenuEntry {
  private:
    int8_t brightnessPercent;
    void setBrightness();

  public:
    BrightnessMenuEntry();
    virtual void onActivate(Menu *parent);
    virtual void onSelect();
    virtual void onBack();
    virtual void onRotateClockwise();
    virtual void onRotateAnticlockwise();
    virtual std::string getDisplayedText();
};

#endif
