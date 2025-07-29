#pragma once

#ifndef CLASSES_CALLBACKS_LEVELIMPORTCALLBACK_HPP
#define CLASSES_CALLBACKS_LEVELIMPORTCALLBACK_HPP

#include <Geode/Geode.hpp>
#include <Geode/cocos/support/base64.h>

#include <filesystem>
#include <span>

class LevelImportCallback : public cocos2d::CCNode, public FLAlertLayerProtocol {
public:
    void onLevelImported(cocos2d::CCNode*);

    void goToScene();

    void FLAlert_Clicked(FLAlertLayer*, bool) override;

    LevelImportCallback() : isNewLevel_(false), toLevel_(nullptr) {}

    CC_SYNTHESIZE(bool, isNewLevel_, IsNewLevel);
    CC_SYNTHESIZE(GJGameLevel*, toLevel_, ToLevel);

    CREATE_FUNC(LevelImportCallback)

    static void importLevel(const std::filesystem::path path);
    static void importLevelBytes(const std::span<const std::uint8_t> data);
};

#endif //CLASSES_CALLBACKS_LEVELIMPORTCALLBACK_HPP
