#pragma once

#ifndef CLASSES_SESSIONWIDGET_HPP
#define CLASSES_SESSIONWIDGET_HPP

#include <Geode/Geode.hpp>
#include "classes/delegates/sessiondelegate.hpp"

class SessionWidget : public cocos2d::CCNode, SessionDelegate {
private:
    cocos2d::CCLabelBMFont* statusLabel_{nullptr};
    cocos2d::CCMenu* statusMenu_{nullptr};
    cocos2d::extension::CCScale9Sprite* background_{nullptr};
    std::string errorMessage_{};
    CCMenuItemSpriteExtra* infoButton_{nullptr};
    bool isFirstChange_{true};

public:
    virtual bool init() override;

    virtual void onSessionCreated() override;
    virtual void onSessionFailed(const char* reason) override;

    void sessionStatusChanged(const char* message = nullptr);

    void onCreateSession(cocos2d::CCObject*);
    void onInfo(cocos2d::CCObject*);

    CREATE_FUNC(SessionWidget);

    virtual ~SessionWidget();
};

#endif //CLASSES_SESSIONWIDGET_HPP
