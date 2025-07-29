#pragma once

#ifndef CLASSES_SETTINGS_CORE_SETTINGSPOPUP_HPP
#define CLASSES_SETTINGS_CORE_SETTINGSPOPUP_HPP

#include <array>
#include <Geode/Geode.hpp>

#include "settingspage.hpp"
#include "settingpages.hpp"

struct SettingsItem {
    const char* title;
    const char* icon;
    float size;
    SettingsPage* page;
};

class SettingsPopup : public FLAlertLayer {
protected:
    bool init(int page);

    virtual void keyBackClicked();

    void onClose(cocos2d::CCObject*);

    void onToggleLayer(cocos2d::CCObject* target);

    void toggleLayer(uint32_t layer);

    cocos2d::CCArray* buttons_;
    cocos2d::CCArray* layers_;

public:
    static SettingsPopup* create(int page = -1) {
        auto pRet = new SettingsPopup();
        if (pRet && pRet->init(page)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }
};

#endif //CLASSES_SETTINGS_CORE_SETTINGSPOPUP_HPP
