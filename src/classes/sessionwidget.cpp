#include "classes/sessionwidget.hpp"
#include "classes/managers/tokenmanager.hpp"

void SessionWidget::onSessionCreated() {
	this->sessionStatusChanged();
}

void SessionWidget::onSessionFailed(const char* reason) {
	this->sessionStatusChanged(reason);
}

void SessionWidget::sessionStatusChanged(const char* message) {
	// determine if session is created/in progress/not created
	// if message is set, assume error
	auto& tokenManager = TokenManager::get();

	auto statusLabel = "No session found.";

	this->statusMenu_->setVisible(true);
	this->infoButton_->setVisible(false);

	auto win_size = cocos2d::CCDirector::sharedDirector()->getWinSize();
	auto pos_x = win_size.width / 2.0f;

	this->setPositionX(pos_x);

	// ew, else if chain
	if (message) {
		statusLabel = "Error creating session.";
		this->errorMessage_ = message;
		this->infoButton_->setVisible(true);

		// offset it (hacky)
		this->setPositionX(pos_x - 37.0f);
	} else if (!PlatformToolbox::isNetworkAvailable()) {
		statusLabel = "No network available!";
	} else if (tokenManager.sendingRequest()) {
		// in process of creating a token
		statusLabel = "Creating session...";
		this->setPositionX(pos_x);
		this->statusMenu_->setVisible(false);
	} else if (tokenManager.createdSession()) {
		if (this->isFirstChange_) {
			this->setVisible(false);
		} else {
			// fade out to show people it happened
			auto fadeSequence = cocos2d::CCSequence::createWithTwoActions(
				cocos2d::CCDelayTime::create(1.0f),
				cocos2d::CCFadeTo::create(1.0f, 0)
			);
			// fade out the label, as we can't fade out the layer itself
			this->statusLabel_->runAction(fadeSequence);
		}

		statusLabel = "Session created!";
		this->setPositionX(pos_x);
		this->statusMenu_->setVisible(false);
	}

	this->statusLabel_->setString(statusLabel, true);
	this->statusMenu_->setPositionX(
		this->statusLabel_->boundingBox().getMaxX() + 20.0f
	);
}

void SessionWidget::onInfo(cocos2d::CCObject*) {
	if (this->errorMessage_.empty()) {
		return;
	}

	FLAlertLayer::create(
		nullptr,
		"Session Creation Error",
		this->errorMessage_.c_str(),
		"OK",
		nullptr,
		350.0f
	)->show();
}

void SessionWidget::onCreateSession(cocos2d::CCObject*) {
	auto am = GJAccountManager::sharedState();
	if (am->m_accountID == 0) {
		return;
	}

	if (!PlatformToolbox::isNetworkAvailable()) {
		this->sessionStatusChanged();
		return;
	}

	TokenManager::get().createSession();

	this->sessionStatusChanged();
}

bool SessionWidget::init() {
	// register delegate
	TokenManager::get().setSessionDelegate(this);

	this->setAnchorPoint({ 0.5f, 0.5f });

	this->background_ = cocos2d::extension::CCScale9Sprite::create(
		"square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
	this->background_->setContentSize({ 250.0f, 40.0f });
	this->background_->setOpacity(100);
	this->background_->setPosition(0.0f, 0.0f);
	this->addChild(this->background_);
	this->background_->setVisible(false);

	this->statusLabel_ = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");
	this->addChild(this->statusLabel_);
	this->statusLabel_->setPosition(0.0f, 0.0f);
	this->statusLabel_->setScale(0.6f);

	this->statusMenu_ = cocos2d::CCMenu::create();
	this->addChild(statusMenu_);
	statusMenu_->setPosition(10.0f, 0.0f);

	auto updateSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
	updateSprite->setScale(0.6f);
	auto updateButton = CCMenuItemSpriteExtra::create(
		updateSprite,
		nullptr,
		this,
		static_cast<cocos2d::SEL_MenuHandler>(&SessionWidget::onCreateSession)
	);
	statusMenu_->addChild(updateButton);

	auto infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png");
	infoSprite->setScale(0.6f);
	this->infoButton_ = CCMenuItemSpriteExtra::create(
		infoSprite,
		nullptr,
		this,
		static_cast<cocos2d::SEL_MenuHandler>(&SessionWidget::onInfo)
	);
	statusMenu_->addChild(this->infoButton_);

	this->infoButton_->setPositionX(40.0f);
	this->infoButton_->setVisible(false);

	this->isFirstChange_ = true;
	this->sessionStatusChanged();
	this->isFirstChange_ = false;

	return true;
}

SessionWidget::~SessionWidget() {
	// unregister delegate
	TokenManager::get().setSessionDelegate(nullptr);
}