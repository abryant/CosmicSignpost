#include "menu_entry.h"

#include "menu.h"

MenuEntry::MenuEntry(std::string name)
    : name(name),
      parent(NULL),
      followOnMenuEntry(NULL) {}

std::string MenuEntry::getName() {
  return name;
}

void MenuEntry::setName(std::string name) {
  this->name = name;
}

void MenuEntry::onActivate(Menu *parent) {
  this->parent = parent;
}

void MenuEntry::deactivate(bool goToFollowOn) {
  parent->deactivateChild(goToFollowOn);
  parent = NULL;
}

bool MenuEntry::hasParent() {
  return parent != NULL;
}

void MenuEntry::setFollowOnMenuEntry(std::shared_ptr<MenuEntry> followOnMenuEntry) {
  this->followOnMenuEntry = followOnMenuEntry;
}

std::shared_ptr<MenuEntry> MenuEntry::getFollowOnMenuEntry() {
  return followOnMenuEntry;
}
