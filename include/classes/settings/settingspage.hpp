#pragma once

#ifndef CLASSES_SETTINGS_CORE_SETTINGSPAGE_HPP
#define CLASSES_SETTINGS_CORE_SETTINGSPAGE_HPP

#include <Geode/Geode.hpp>

class SettingsPage : public cocos2d::CCLayer {
public:
    virtual void createPage() = 0;
    virtual void onToggleVisibility(bool visible);
    virtual void onClose() {}

    void setDimensions(const cocos2d::CCSize& dimensions);
    void setWindowDimensions(const cocos2d::CCSize& dimensions);
    void setInternalMenu(cocos2d::CCMenu* menu);

protected:
    std::vector<cocos2d::CCNode*> _menu_objects;
    cocos2d::CCMenu* _internal_menu = nullptr;
    cocos2d::CCSize _dimensions;
    cocos2d::CCSize _window_dimensions;
};

#endif //CLASSES_SETTINGS_CORE_SETTINGSPAGE_HPP
