#pragma once

#ifndef CLASSES_SETTINGS_TEXTURESETTINGSPOPUP_HPP
#define CLASSES_SETTINGS_TEXTURESETTINGSPOPUP_HPP

#include <Geode/Geode.hpp>

#include "base/platform_helper.hpp"

class TextureSettingsPopup : public FLAlertLayer {
    bool init() override;
    void keyBackClicked() override;

    void onClose(cocos2d::CCObject*);
    void onTextures(cocos2d::CCObject*);
    void onClear(cocos2d::CCObject*);
    void onClassic(cocos2d::CCObject*);

    bool isTexturePackInstalled();

public:
    CREATE_FUNC(TextureSettingsPopup); // NOLINT(modernize-use-auto)
};

#endif //CLASSES_SETTINGS_TEXTURESETTINGSPOPUP_HPP
