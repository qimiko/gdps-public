#pragma once

#ifndef CLASSES_MAINLEVELINFOPOPUP_HPP
#define CLASSES_MAINLEVELINFOPOPUP_HPP

#include <Geode/Geode.hpp>

#include "creditsline.hpp"

class MainLevelInfoPopup : public FLAlertLayer, FLAlertLayerProtocol {
	GJGameLevel* m_level;
	bool init(GJGameLevel*);

	bool canCopyLevel() const;
	void cloneLevel();

	void addCredits(CreditsLine*);

protected:

	virtual void keyBackClicked() override;
	virtual void FLAlert_Clicked(FLAlertLayer*, bool) override;

	void onClose(cocos2d::CCObject*);
	void onCopy(cocos2d::CCObject*);

public:
	static MainLevelInfoPopup* create(GJGameLevel*);

};

#endif
