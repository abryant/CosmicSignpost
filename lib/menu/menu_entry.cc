#include "menu_entry.h"

MenuEntry::MenuEntry(std::string name) : name(name), parent(NULL) {}

std::string MenuEntry::getName() {
  return name;
}

void MenuEntry::setName(std::string name) {
  this->name = name;
}

void MenuEntry::onActivate(Menu *parent) {
  this->parent = parent;
}

void MenuEntry::deactivate() {
  parent->deactivateChild();
  parent = NULL;
}