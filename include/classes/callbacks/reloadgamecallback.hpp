#pragma once

#ifndef CLASSES_CALLBACKS_RELOADGAMECALLBACK_HPP
#define CLASSES_CALLBACKS_RELOADGAMECALLBACK_HPP

#include <Geode/Geode.hpp>

class ReloadGameCallback : public cocos2d::CCNode {

public:
    void onReload(cocos2d::CCNode*);
    CREATE_FUNC(ReloadGameCallback)

    static void reloadGame();
};

#endif //CLASSES_CALLBACKS_RELOADGAMECALLBACK_HPP
