#ifndef COSMIC_SIGNPOST_LIB_MAIN_DIRECTION_MENU_ENTRY_H_
#define COSMIC_SIGNPOST_LIB_MAIN_DIRECTION_MENU_ENTRY_H_

#include <string>

#include "tracker.h"
#include "menu_entry.h"

/**
 * Menu entry for changing the angle of the Cosmic Signpost.
 */
class DirectionMenuEntry : public MenuEntry {
  private:
    Tracker &tracker;
    int32_t currentAzimuth;
    int32_t currentAltitude;
    bool adjustingAzimuth;
    Direction getCurrentDirection(int64_t timeMillis);

  public:
    DirectionMenuEntry(std::string name, Tracker &tracker);
    virtual void onActivate(Menu *parent);
    virtual void onSelect();
    virtual void onBack();
    virtual void onRotateClockwise();
    virtual void onRotateAnticlockwise();
    virtual std::string getDisplayedText();
};

#endif
