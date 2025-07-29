#include "classes/controlleroverlay.hpp"

void ControllerOverlay::regenerateFade(bool isShort) {
	if (fadeAction_) {
		overlaySprite_->stopAction(fadeAction_);
		fadeAction_ = nullptr;
	}

	overlaySprite_->setOpacity(255);

	fadeAction_ = cocos2d::CCSequence::createWithTwoActions(
		cocos2d::CCDelayTime::create(isShort ? 0.0f : 0.75f),
		cocos2d::CCFadeOut::create(0.5f)
	);
	overlaySprite_->runAction(fadeAction_);
}

float ControllerOverlay::clampValueToMax(float value, float max) {
	return std::min(std::max(value, 0.0f), max);
}

float ControllerOverlay::getFixedPositionY() {
	return cocos2d::CCDirector::sharedDirector()->getScreenTop() - overlaySprite_->getPositionY();
}

void ControllerOverlay::updateTouchInfo() {
	currentTouch_->setTouchInfo(
		controllerID_,
		overlaySprite_->getPositionX(),
		getFixedPositionY());
}

void ControllerOverlay::updateTouch() {
	if (!currentTouch_) {
		return;
	}

	updateTouchInfo();

	auto touchSet = cocos2d::CCSet();
	touchSet.addObject(currentTouch_);

	cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()
		->touchesMoved(&touchSet, nullptr);
}

void ControllerOverlay::update(float dt) {
	if (fadeAction_ != nullptr) {
		// we're handling our own update so we handle our own step
		fadeAction_->step(dt);
	}
}

void ControllerOverlay::resetCursor(bool withFade) {
	auto wSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

	overlaySprite_->setPosition(wSize / 2);

	if (withFade) {
		regenerateFade();
	}
	updateTouch();
}

bool ControllerOverlay::init()  {
	overlaySprite_ = cocos2d::CCSprite::create("smallDot.png");
	this->addChild(overlaySprite_);

	overlaySprite_->setScale(1.5f);
	overlaySprite_->setOpacity(0);

	resetCursor(false);

	return true;
}

void ControllerOverlay::pushCursor(float x, float y) {
	auto wSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
	auto currentPosition = overlaySprite_->getPosition();

	overlaySprite_->setPosition({
		clampValueToMax(currentPosition.x + x, wSize.width),
		clampValueToMax(currentPosition.y + y, wSize.height)
	});

	regenerateFade();
	updateTouch();
}

void ControllerOverlay::pushDown() {
	if (fadeAction_) {
		overlaySprite_->stopAction(fadeAction_);
		fadeAction_ = nullptr;
	}

	if (!cursorHidden_) {
		overlaySprite_->setOpacity(255);
	}

	// hardcoded id, Android uses pointer id for touches
	if (currentTouch_) {
		return;
	}

	currentTouch_ = new cocos2d::CCTouch();
	updateTouchInfo();

	auto touchSet = cocos2d::CCSet();
	touchSet.addObject(currentTouch_);
	cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()
			->touchesBegan(&touchSet, nullptr);

	currentTouch_->retain();
}

void ControllerOverlay::pushUp() {
	if (!currentTouch_) {
		overlaySprite_->setOpacity(0);
		return;
	}

	if (!cursorHidden_) {
		regenerateFade(true);
	}

	updateTouchInfo();

	auto touchSet = cocos2d::CCSet();
	touchSet.addObject(currentTouch_);

	cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()
		->touchesEnded(&touchSet, nullptr);

	currentTouch_->release();
	currentTouch_ = nullptr;
}

void ControllerOverlay::setCursorHidden(bool hidden) {
	cursorHidden_ = hidden;
}

const cocos2d::CCPoint& ControllerOverlay::getPosition() {
	return overlaySprite_->getPosition();
}
