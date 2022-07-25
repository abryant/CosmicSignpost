#include "space_pointer_menu.h"

#include <memory>
#include <string>

#include "Arduino.h"

#include "menu.h"
#include "menu_entry.h"
#include "action_menu_entry.h"
#include "boolean_menu_entry.h"
#include "info_menu_entry.h"
#include "number_menu_entry.h"

std::shared_ptr<Menu> buildSpacePointerMenu() {
  std::vector<std::shared_ptr<MenuEntry>> mainEntries = {
    std::make_shared<ActionMenuEntry>("Action", []() {}),
    std::make_shared<InfoMenuEntry>("Info", []() {return "Some info";}),
    std::make_shared<BooleanMenuEntry>("Boolean", [](bool b) {}, false),
    std::make_shared<NumberMenuEntry>("Latitude", "Lt: ~###.######N", [](std::string value) {})
  };
  return std::make_shared<Menu>("Main menu", mainEntries);
}
