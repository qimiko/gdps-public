#include "classes/uploadactionpopup.hpp"

void UploadActionPopup::keyBackClicked() {
	this->onClose(nullptr);
};

void UploadActionPopup::closePopup() {
	this->setKeyboardEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void UploadActionPopup::onClose(cocos2d::CCObject*) {
	if (!this->m_cancellable) {
		return;
	}

	if (this->m_delegate != nullptr) {
		this->m_delegate->onClosePopup(this);
		return;
	}

	this->closePopup();
}

bool UploadActionPopup::init(const char* loadingMessage, bool cancellable) {
	if (!this->initWithColor({0, 0, 0, 75})) {
		return false;
	}

	constexpr auto width = 200.0f;
	constexpr auto height = 100.0f;

	auto ccd = cocos2d::CCDirector::sharedDirector();
	ccd->getTouchDispatcher()->incrementForcePrio();
	this->setTouchEnabled(true);
	this->setKeypadEnabled(true);

	this->m_controlConnected = -1;
	this->m_scrollAction = -1;

	this->m_mainLayer = cocos2d::CCLayer::create();
	this->addChild(this->m_mainLayer);

	auto window_size = ccd->getWinSize();
	auto bg = cocos2d::extension::CCScale9Sprite::create("square02.png"_spr, {0.0f, 0.0f, 80.0f, 80.0f});
	bg->setContentSize(cocos2d::CCSize(width, height));
	bg->setPosition(window_size.width / 2, window_size.height / 2);

	this->m_mainLayer->addChild(bg, -2);

	this->m_buttonMenu = cocos2d::CCMenu::create();
	this->m_mainLayer->addChild(this->m_buttonMenu, 1);
	m_buttonMenu->ignoreAnchorPointForPosition(false);

	auto messageArea = TextArea::create(loadingMessage, 400.0f, 0, {0.5f, 1.0f}, "chatFont.fnt", 1.0f);
	m_messageArea = messageArea;

	m_mainLayer->addChild(messageArea);

	messageArea->setPosition({window_size.width / 2, (window_size.height / 2) - 20.0f});
	messageArea->setScale(0.8f);

	auto loadingCircle = LoadingCircle::create();
	m_loadingCircle = loadingCircle;

	loadingCircle->setScale(0.7f);
	loadingCircle->m_parentLayer = m_mainLayer;
	loadingCircle->m_fade = false;

	loadingCircle->show();

	loadingCircle->setPosition({0.0f, 14.0f});

	auto closeSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
	closeSprite->setScale(0.8f);

	auto closeBtn = CCMenuItemSpriteExtra::create(closeSprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&UploadActionPopup::onClose));
	m_closeBtn = closeBtn;

	closeBtn->setSizeMult(1.6f);
	this->m_buttonMenu->addChild(closeBtn);

	closeBtn->setPosition({(window_size.width - width) / 2 + 5.0f, (window_size.height + height) / 2 - 5.0f});

	closeBtn->setVisible(cancellable);
	m_cancellable = cancellable;

	return true;
}

void UploadActionPopup::showSuccessMessage(const char* message) {
	m_cancellable = true;

	m_closeBtn->setVisible(true);
	m_messageArea->setString(message);
	m_loadingCircle->fadeAndRemove();

	auto successSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
	successSprite->setScale(1.5f);

	auto windowSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
	m_mainLayer->addChild(successSprite);
	successSprite->setPosition({windowSize.width / 2, (windowSize.height / 2) + 14.0f});

	m_successful = true;
}

void UploadActionPopup::showFailMessage(const char* message) {
	m_cancellable = true;

	m_closeBtn->setVisible(true);
	m_messageArea->setString(message);
	m_loadingCircle->fadeAndRemove();

	auto failSprite = cocos2d::CCSprite::createWithSpriteFrameName("exMark_001.png");

	auto windowSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
	m_mainLayer->addChild(failSprite);
	failSprite->setPosition({windowSize.width / 2, (windowSize.height / 2) + 14.0f});

	m_successful = false;
}

void UploadActionPopup::setDelegate(UploadPopupDelegate* delegate) {
	this->m_delegate = delegate;
}

bool UploadActionPopup::getSuccessful() const {
	return this->m_successful;
}
