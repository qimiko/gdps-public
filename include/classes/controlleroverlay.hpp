#pragma once

#ifndef CLASSES_CONTROLLEROVERLAY_HPP
#define CLASSES_CONTROLLEROVERLAY_HPP

#include <Geode/Geode.hpp>

class ControllerOverlay : public cocos2d::CCNode {
private:
    cocos2d::CCSprite* overlaySprite_{nullptr};
    cocos2d::CCAction* fadeAction_{nullptr};

    // hardcoded id, Android uses pointer id for touches
    // this id is beyond the max supported by the normal touch interface
    int controllerID_{5};
    cocos2d::CCTouch* currentTouch_{nullptr};

    bool cursorHidden_{false};

    static float clampValueToMax(float value, float max);

    void regenerateFade(bool isShort = false);

    // positionY is flipped for some reason. this fixes it
    float getFixedPositionY();

    void updateTouchInfo();
    void updateTouch();

public:
    void update(float dt) override;
    bool init() override;

    void resetCursor(bool withFade = true);
    void pushCursor(float x, float y);

    void pushDown();
    void pushUp();

    void setCursorHidden(bool hidden);

    const cocos2d::CCPoint& getPosition() override;

    CREATE_FUNC(ControllerOverlay);
};

#endif //CLASSES_CONTROLLEROVERLAY_HPP
