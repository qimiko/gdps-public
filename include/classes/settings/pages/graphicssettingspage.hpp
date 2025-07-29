#pragma once

#ifndef CLASSES_SETTINGS_GRAPHICSSETTINGSPAGE_HPP
#define CLASSES_SETTINGS_GRAPHICSSETTINGSPAGE_HPP

#include <Geode/Geode.hpp>

#include "base/game_variables.hpp"

#include "classes/settings/togglesettingspage.hpp"
#include "classes/settings/texturesettingspopup.hpp"

class GraphicsSettingsPage : public ToggleSettingsPage {
public:
    void createPage() override;

    void onPrev(cocos2d::CCObject*);
    void onNext(cocos2d::CCObject*);
    void onApply(cocos2d::CCObject*);
    void onTextures(cocos2d::CCObject*);
    void onLimitAspectRatio(cocos2d::CCObject*);

    CREATE_FUNC(GraphicsSettingsPage); // NOLINT(modernize-use-auto)

protected:
    const char* labelForQuality(cocos2d::TextureQuality quality);

    void updateToggles(uint32_t btn);

    cocos2d::CCLabelBMFont* _quality_label = nullptr;
    CCMenuItemSpriteExtra* _apply_button = nullptr;
    cocos2d::TextureQuality _selected_quality = cocos2d::kTextureQualityLow;
};

#endif //CLASSES_SETTINGS_GRAPHICSSETTINGSPAGE_HPP
