#include "classes/managers/controllermanager.hpp"

bool ControllerManager::isFloatNotCloseToZero(float value) {
	// ignore incredibly small values of the controller
	// this constant seems to be "good enough"
	return std::abs(value) > 0.05f;
}

void ControllerManager::update(float dt) {
	// update right joystick scroll
	if (isFloatNotCloseToZero(right_.axisX) || isFloatNotCloseToZero(right_.axisY)) {
		// they seem to be reversed for some reason? idk.
		constexpr auto SCROLL_FACTOR = 5.0f;

		cocos2d::CCDirector::sharedDirector()->getMouseDispatcher()
			->dispatchScrollMSG(right_.axisY * SCROLL_FACTOR, -right_.axisX * SCROLL_FACTOR);
	}

	if (isFloatNotCloseToZero(left_.axisX) || isFloatNotCloseToZero(left_.axisY)) {
		// they seem to be reversed for some reason? idk.
		constexpr auto PUSH_FACTOR = 5.0f;

		controllerOverlay_->pushCursor(left_.axisX * PUSH_FACTOR, -left_.axisY * PUSH_FACTOR);
	}

	controllerOverlay_->update(dt);
}

bool ControllerManager::updateJoystickValues(const Joystick left, const Joystick right) {
	this->left_ = left;
	this->right_ = right;

	return true;
}

bool ControllerManager::pushCursor(float x, float y) {
	controllerOverlay_->pushCursor(x, y);

	return true;
}

void ControllerManager::handleReset() {
	controllerOverlay_->resetCursor();
}

void ControllerManager::handleClickDown() {
	controllerOverlay_->pushDown();
}

void ControllerManager::handleClickUp() {
	controllerOverlay_->pushUp();
}

void ControllerManager::hideCursor() {
	geode::log::info("ControllerManager::hideCursor");
//    controllerOverlay_->setCursorHidden(true);
//    PlatformHelper::capture_cursor();
}

void ControllerManager::showCursor() {
	geode::log::info("ControllerManager::showCursor");
//    controllerOverlay_->setCursorHidden(false);
//    PlatformHelper::release_cursor();
}

ControllerManager& ControllerManager::getManager() {
	static ControllerManager _managerInstance;
	return _managerInstance;
}

void ControllerManager::init() {
	// register with scheduler
	auto scheduler = cocos2d::CCDirector::sharedDirector()->getScheduler();
	scheduler->scheduleUpdateForTarget(this, 1, false);

	controllerOverlay_ = ControllerOverlay::create();
	cocos2d::CCDirector::sharedDirector()->setNotificationNode(controllerOverlay_);
}

ControllerManager::ControllerManager() {
	init();
};

ControllerManager::~ControllerManager() {
	auto scheduler = cocos2d::CCDirector::sharedDirector()->getScheduler();
	scheduler->unscheduleUpdateForTarget(this);
}

#ifdef GEODE_IS_ANDROID
#include <android/keycodes.h>

bool ControllerManager::isKeycodeControllerConfusable(int keyCode) {
	return keyCode == AKEYCODE_DPAD_LEFT
		|| keyCode == AKEYCODE_DPAD_RIGHT
		|| keyCode == AKEYCODE_DPAD_UP
		|| keyCode == AKEYCODE_DPAD_DOWN;
}

cocos2d::enumKeyCodes ControllerManager::translateAndroidKeyCodeToWindows(int keyCode) {
	// special controller override
	// only the main menu makes use of this difference
	if (isKeycodeControllerConfusable(keyCode) && PlatformToolbox::isControllerConnected()) {
		switch (keyCode) {
			case AKEYCODE_DPAD_DOWN:
				return cocos2d::CONTROLLER_Down;
			case AKEYCODE_DPAD_UP:
				return cocos2d::CONTROLLER_Up;
			case AKEYCODE_DPAD_LEFT:
				return cocos2d::CONTROLLER_Left;
			case AKEYCODE_DPAD_RIGHT:
				return cocos2d::CONTROLLER_Right;
		}
	}

	return static_cast<cocos2d::enumKeyCodes>(keyCode);
}
#else
bool ControllerManager::isKeycodeControllerConfusable(int keyCode) {
	return false;
}

cocos2d::enumKeyCodes ControllerManager::translateAndroidKeyCodeToWindows(int keyCode) {
	return static_cast<cocos2d::enumKeyCodes>(keyCode);
}
#endif