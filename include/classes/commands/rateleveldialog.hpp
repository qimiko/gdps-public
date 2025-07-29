#pragma once

#ifndef CLASSES_COMMANDS_RATELEVELDIALOG_HPP
#define CLASSES_COMMANDS_RATELEVELDIALOG_HPP

#include <Geode/Geode.hpp>

#include "classes/uploadactionpopup.hpp"
#include "commanddispatcher.hpp"

#include <optional>

class RateLevelDialog : public FLAlertLayer, public CommandDispatcherProtocol, public TextInputDelegate, public UploadPopupDelegate {
	GJGameLevel* m_level{nullptr};

	int m_selectedStars{0};
	int m_selectedRating{0};

	int m_initialStars{0};
	int m_initialRating{0};

	std::vector<ButtonSprite*> m_starButtons{};
	cocos2d::CCSprite* m_difficultySprite{nullptr};
	cocos2d::CCSprite* m_ratingSprite{nullptr};
	TextArea* m_reasonArea{nullptr};
	CCTextInputNode* m_reasonInput{nullptr};
	std::string m_reason{};
	UploadActionPopup* m_uploadAction{nullptr};
	std::optional<CommandDispatcher> m_commandDispatcher{};
	ButtonSprite* m_submitSprite{nullptr};
	cocos2d::CCLabelBMFont* m_title{nullptr};
	cocos2d::CCSprite* m_sendIndicator{nullptr};
	cocos2d::CCLabelBMFont* m_noStarIndicator{nullptr};
	bool m_isSend{false};

protected:
	bool init(GJGameLevel*, bool);
	virtual void keyBackClicked() override;

	void onClose(cocos2d::CCObject*);
	void onSubmit(cocos2d::CCObject*);

	void updateDifficulty();
	void onStar(cocos2d::CCObject*);
	void onToggleRating(cocos2d::CCObject*);
	void toggleSend(cocos2d::CCObject*);
	void onInfo(cocos2d::CCObject*);

	bool hasMadeChanges() const;
	const char* currentDifficultyName() const;

	virtual void textChanged(CCTextInputNode*) override;
	virtual void textInputClosed(CCTextInputNode*) override;
	virtual void textInputOpened(CCTextInputNode*) override;

	virtual void onClosePopup(UploadActionPopup*) override;

	virtual void onDispatchSuccess(CommandDispatcher*) override;
	virtual void onDispatchFailure(CommandDispatcher*) override;

	void updateReason(const std::string&);

public:
	static RateLevelDialog* create(GJGameLevel* level, bool isSend) {
		auto pRet = new RateLevelDialog();
		if (pRet && pRet->init(level, isSend)) {
			pRet->autorelease();
			return pRet;
		} else {
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}
};

#endif // CLASSES_COMMANDS_RATELEVELDIALOG_HPP
