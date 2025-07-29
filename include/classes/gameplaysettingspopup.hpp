#pragma once

#ifndef CLASSES_GAMEPLAYSETTINGSPOPUP_HPP
#define CLASSES_GAMEPLAYSETTINGSPOPUP_HPP

#include <Geode/Geode.hpp>
#include <unordered_map>

#include "classes/speedhack/speedhackcarddelegate.hpp"

class GameplaySettingsPopup : public FLAlertLayer, public SpeedhackCardDelegate {
private:
	std::unordered_map<uint32_t, std::string> m_infoText{};
	int m_toggleIdx{0};
	cocos2d::CCPoint m_centerPoint{0.0f, 0.0f};
	PlayLayer* m_playLayer{nullptr};

	cocos2d::CCLabelBMFont* m_cheatIndicator{nullptr};
	void updateCheatIndicator();

	cocos2d::CCPoint getNextTogglePoint();
	void addToggle(const char* title, bool default_on, cocos2d::SEL_MenuHandler callback, const char* description);

protected:
	virtual bool init() override;
	virtual void keyBackClicked() override;
	virtual void onSpeedhackValueChanged(float interval) override;

	void onClose(cocos2d::CCObject*);

	void onInfoText(cocos2d::CCObject*);
	void onShowPercentage(cocos2d::CCObject*);
	void onPracticeMusic(cocos2d::CCObject*);
	void onIgnoreDamage(cocos2d::CCObject*);
	void onDeathHitboxes(cocos2d::CCObject*);
	void onPracticeHitboxes(cocos2d::CCObject*);

public:
	static GameplaySettingsPopup* create() {
		auto pRet = new GameplaySettingsPopup();
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

#endif //CLASSES_GAMEPLAYSETTINGSPOPUP_HPP
