#pragma once

#ifndef CLASSES_SETTINGS_ABOUTSETTINGSPAGE_HPP
#define CLASSES_SETTINGS_ABOUTSETTINGSPAGE_HPP

#include <Geode/Geode.hpp>

#include "base/config.hpp"
#include "base/cmake_config.hpp"

#include "classes/settings/settingspage.hpp"

class AboutSettingsPage : public SettingsPage {
public:
    void createPage() override;

    void onSource(cocos2d::CCObject*);
    void onMods(cocos2d::CCObject*);

    CREATE_FUNC(AboutSettingsPage); // NOLINT(modernize-use-auto)
};

#endif //CLASSES_SETTINGS_ABOUTSETTINGSPAGE_HPP
