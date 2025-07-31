#pragma once

#ifndef CLASSES_CREDITSNODE_HPP
#define CLASSES_CREDITSNODE_HPP

#include <Geode/Geode.hpp>
#include <unordered_map>

class CreditsNode : public cocos2d::CCNode, public FLAlertLayerProtocol {
private:
    cocos2d::CCMenu* m_names{nullptr};

    virtual void FLAlert_Clicked(FLAlertLayer*, bool) override;

    int m_willOpenUser{-1};
    bool m_noTitle{false};

    float m_menuScale{0.75f};

protected:
    bool init(const char*);
    void onCreator(cocos2d::CCObject*);

    void onUpdateLayout();

public:
    void addUser(const char* name, int userId, int accountId);

    static CreditsNode* create(const char* title) {
        auto pRet = new CreditsNode();
        if (pRet && pRet->init(title)) {
            pRet->autorelease();
            return pRet;
        } else {
            delete pRet;
            pRet = nullptr;
            return nullptr;
        }
    }

    static bool s_disableOnline;
};

#endif //CLASSES_CREDITSPOPUP_HPP
