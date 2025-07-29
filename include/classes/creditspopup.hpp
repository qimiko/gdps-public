#pragma once

#ifndef CLASSES_CREDITSPOPUP_HPP
#define CLASSES_CREDITSPOPUP_HPP

#include <Geode/Geode.hpp>

#include "creditsnode.hpp"

class CreditsPopup : public FLAlertLayer {
protected:
    virtual bool init() override;
    virtual void keyBackClicked() override;

    void createPoweredBy(cocos2d::CCPoint offset);
    void createOriginalGame(cocos2d::CCPoint offset);

    void onClose(cocos2d::CCObject*);

    void onGeode(cocos2d::CCObject*);
    void onRobTop(cocos2d::CCObject*);

public:
    static CreditsPopup* create() {
        auto pRet = new CreditsPopup();
        if (pRet && pRet->init()) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }
};

#endif //CLASSES_CREDITSPOPUP_HPP
