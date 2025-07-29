#pragma once

#ifndef CLASSES_MANAGERS_CONTROLLERMANAGER_HPP
#define CLASSES_MANAGERS_CONTROLLERMANAGER_HPP

#include <unordered_map>
#include <cmath>

#include <Geode/Geode.hpp>

#include "base/platform_helper.hpp"

#include "classes/controlleroverlay.hpp"

class ControllerManager : public cocos2d::CCObject {
public:
    struct Joystick {
        float axisX;
        float axisY;
    };

private:
    Joystick left_{0.0f, 0.0f};
    Joystick right_{0.0f, 0.0f};
    ControllerOverlay* controllerOverlay_{nullptr};

    bool isFloatNotCloseToZero(float value);

public:
    void update(float dt) override;

    void init();

    // this probably won't ever be called
    ~ControllerManager() override;

    ControllerManager();
    ControllerManager(ControllerManager&&) = delete;
    ControllerManager& operator=(ControllerManager&&) = delete;
    ControllerManager(const ControllerManager&) = delete;
    ControllerManager& operator=(const ControllerManager&) = delete;

    static ControllerManager& getManager();

    bool updateJoystickValues(const Joystick left, const Joystick right);

    void handleReset();
    void handleClickDown();
    void handleClickUp();

    bool pushCursor(float x, float y);

    void showCursor();
    void hideCursor();

    static bool isKeycodeControllerConfusable(int keyCode);
    static cocos2d::enumKeyCodes translateAndroidKeyCodeToWindows(int keyCode);
};

#endif //CLASSES_MANAGERS_CONTROLLERMANAGER_HPP
