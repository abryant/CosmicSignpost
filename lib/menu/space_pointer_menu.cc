#include "space_pointer_menu.h"

#include <memory>
#include <string>

#include "Arduino.h"
#include "WiFi.h"

#include "menu.h"
#include "menu_entry.h"
#include "action_menu_entry.h"
#include "boolean_menu_entry.h"
#include "brightness_menu_entry.h"
#include "info_menu_entry.h"
#include "number_menu_entry.h"

std::shared_ptr<Menu> buildSpacePointerMenu(std::function<void(int32_t)> updateAngle) {
  std::vector<std::shared_ptr<MenuEntry>> mainEntries = {
    std::make_shared<InfoMenuEntry>("Wireless IP", []() {
      return std::string(WiFi.localIP().toString().c_str());
    }),
    std::make_shared<ActionMenuEntry>("Action", []() {
      Serial.println("Action pressed");
    }),
    std::make_shared<BooleanMenuEntry>("Boolean", [](bool b) {
      Serial.print("Boolean changed to: ");
      Serial.println(b);
    }, false),
    std::make_shared<NumberMenuEntry>("Latitude", "Lt: ~##.######N", [](std::string value) {
      Serial.print("Latitude: ");
      Serial.println(value.c_str());
    }),
    std::make_shared<NumberMenuEntry>("Angle", "Angle: ~###d", [updateAngle](std::string text) {
      std::string numericValue = text.substr(7, 4);
      int angleInt = std::stoi(numericValue);
      updateAngle(angleInt);
    }),
    std::make_shared<BrightnessMenuEntry>(),
  };
  return std::make_shared<Menu>("Main menu", mainEntries);
}
