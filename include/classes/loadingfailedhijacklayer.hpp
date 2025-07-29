#pragma once

#ifndef CLASSES_LOADINGFAILEDHIJACKLAYER_HPP
#define CLASSES_LOADINGFAILEDHIJACKLAYER_HPP

#include <Geode/Geode.hpp>

#include <fmt/format.h>

class LoadingFailedHijackLayer : public cocos2d::CCLayer, FLAlertLayerProtocol {
public:
    static bool gHasDestroyedState;
    static int gReasonCode;
    static geode::Patch* gDestroyPatch;

public:

    static bool hasDestroyedState();

    void FLAlert_Clicked(FLAlertLayer*, bool) override;
    bool init(int code);

    void onReset(cocos2d::CCObject*);

    static void destroyGameState();
    static void restoreGameState();

    static void setCode(int code);

    static cocos2d::CCScene* scene();

    static LoadingFailedHijackLayer* create(int code);
    static LoadingFailedHijackLayer* create();
};

#endif //CLASSES_LOADINGFAILEDHIJACKLAYER_HPP
