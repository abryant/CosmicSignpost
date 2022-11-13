#include "space_pointer_menu.h"

#include <memory>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "Arduino.h"
#include "WiFi.h"

#include "menu.h"
#include "menu_entry.h"
#include "action_menu_entry.h"
#include "boolean_menu_entry.h"
#include "brightness_menu_entry.h"
#include "info_menu_entry.h"
#include "number_menu_entry.h"
#include "tracking_menu.h"

std::shared_ptr<Menu> buildViewMenu() {
  std::vector<std::shared_ptr<MenuEntry>> viewEntries = {
    std::make_shared<InfoMenuEntry>("Wireless IP", []() {
      return std::string(WiFi.localIP().toString().c_str());
    }),
    std::make_shared<InfoMenuEntry>("Date/Time", []() {
      std::time_t time = std::time(nullptr);
      std::tm *utcTime = std::gmtime(&time);
      std::ostringstream timeStr;
      timeStr << std::put_time(utcTime, "%Y-%m-%d\n%H:%M:%S");
      return timeStr.str();
    }),
  };
  return std::make_shared<Menu>("View", viewEntries);
}

std::shared_ptr<Menu> buildSpacePointerMenu(Tracker &tracker) {
  std::vector<std::shared_ptr<MenuEntry>> mainEntries = {
    buildTrackingMenu(tracker),
    std::make_shared<BrightnessMenuEntry>(),
    buildViewMenu(),
  };
  return std::make_shared<Menu>("Main menu", mainEntries);
}
