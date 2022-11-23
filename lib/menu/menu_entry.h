#ifndef COSMIC_SIGNPOST_LIB_MENU_MENU_ENTRY_H_
#define COSMIC_SIGNPOST_LIB_MENU_MENU_ENTRY_H_

#include <memory>
#include <string>

class Menu;

class MenuEntry {
  private:
    std::string name;
    Menu *parent;
    std::shared_ptr<MenuEntry> followOnMenuEntry;

  protected:
    void setName(std::string name);

  public:
    MenuEntry(std::string name);
    std::string getName();
    virtual void onActivate(Menu *parent);
    void deactivate(bool goToFollowOn);
    bool hasParent();
    virtual void onSelect() = 0;
    virtual void onBack() = 0;
    virtual void onRotateClockwise() = 0;
    virtual void onRotateAnticlockwise() = 0;
    virtual std::string getDisplayedText() = 0;

    void setFollowOnMenuEntry(std::shared_ptr<MenuEntry> followOnMenuEntry);
    std::shared_ptr<MenuEntry> getFollowOnMenuEntry();
};

#endif
