#pragma once

#ifndef CLASSES_SETTINGS_CORE_TOGGLESETTINGSPAGE_HPP
#define CLASSES_SETTINGS_CORE_TOGGLESETTINGSPAGE_HPP

#include <unordered_map>
#include <Geode/Geode.hpp>

#include "settingspage.hpp"

class ToggleSettingsPage : public SettingsPage {
protected:
    std::unordered_map<uint32_t, std::string> _info_text;
    uint32_t _toggle_count = 0;
    std::vector<std::vector<cocos2d::CCNode*>> _pages;
    uint32_t _current_page = 0;
    bool _has_page_counter = false;

    static constexpr int MAX_TOGGLE_PER_PAGE = 5;
    static constexpr float TOGGLE_SPACING = 45.0f;

    void addToggle(const char* name, bool default_on, cocos2d::SEL_MenuHandler callback = nullptr, const char* desc = nullptr);
    void addSpacer(int inc = 1);

    float getNextToggleY();
    float getNextToggleMenuY();

    void onPageNext(cocos2d::CCObject*);

public:
    template <uint32_t V>
    void onToggleVariable(cocos2d::CCObject* /* target */)
    {
        auto var_string = cocos2d::CCString::createWithFormat("%04d", V);

        GameManager::sharedState()->toggleGameVariable(var_string->getCString());
    }

    void onInfoText(cocos2d::CCObject* target);

    void onToggleVisibility(bool visible) override;
};

#endif //CLASSES_SETTINGS_CORE_TOGGLESETTINGSPAGE_HPP
