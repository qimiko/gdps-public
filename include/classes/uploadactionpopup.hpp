#pragma once

#ifndef CLASSES_UPLOADACTIONPOPUP_HPP
#define CLASSES_UPLOADACTIONPOPUP_HPP

#include <Geode/Geode.hpp>

// this is basically just a direct port of 2.0's UploadActionPopup

class UploadPopupDelegate;

class UploadActionPopup : public FLAlertLayer {
	TextArea* m_messageArea{nullptr};
	LoadingCircle* m_loadingCircle{nullptr};
	CCMenuItemSpriteExtra* m_closeBtn{nullptr};
	UploadPopupDelegate* m_delegate{nullptr};
	bool m_successful{false};
	bool m_cancellable{false};

protected:
	bool init(const char* loadingMessage, bool cancellable);

	virtual void keyBackClicked() override;
	void onClose(cocos2d::CCObject*);

public:
	void closePopup();
	void showSuccessMessage(const char* message);
	void showFailMessage(const char* message);

	void setDelegate(UploadPopupDelegate*);

	bool getSuccessful() const;

	static UploadActionPopup* create(const char* loadingMessage, bool cancellable = true) {
		auto pRet = new UploadActionPopup();
		if (pRet && pRet->init(loadingMessage, cancellable)) {
			pRet->autorelease();
			return pRet;
		} else {
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}
};

class UploadPopupDelegate {
public:
	virtual void onClosePopup(UploadActionPopup*) { }
};

#endif
