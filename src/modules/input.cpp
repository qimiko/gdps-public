#include <Geode/Geode.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/RetryLevelLayer.hpp>
#include <Geode/modify/PlatformToolbox.hpp>

#include "base/game_variables.hpp"
#include "base/platform_helper.hpp"

#include "classes/managers/controllermanager.hpp"

struct InputCCKeyboardDispatcher : geode::Modify<InputCCKeyboardDispatcher, cocos2d::CCKeyboardDispatcher> {
	bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool isKeyDown) {
		if (isKeyDown && key == cocos2d::enumKeyCodes::CONTROLLER_A) {
			ControllerManager::getManager().handleClickDown();
			return true;
		}

		if (isKeyDown && key == cocos2d::enumKeyCodes::CONTROLLER_LTHUMBSTICK_DOWN) {
			ControllerManager::getManager().handleReset();
			return true;
		}

		return cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown);
	}
};

struct InputPlatformToolbox : geode::Modify<InputPlatformToolbox, PlatformToolbox> {
	bool isControllerConnected() {
		return PlatformHelper::is_controller_connected();
	}
};

struct InputPlayLayer : geode::Modify<InputPlayLayer, PlayLayer> {
	void resume(PlayLayer* self) {
		PlayLayer::resume();

		if (AppDelegate::get()->getPaused()) {
			return;
		}

		if (!GameManager::sharedState()->getGameVariable(GameVariable::SHOW_CURSOR)) {
			if (PlatformToolbox::isControllerConnected()) {
				ControllerManager::getManager().hideCursor();
			}
		}
	}

	void resumeAndRestart() {
		PlayLayer::resumeAndRestart();

		if (AppDelegate::get()->getPaused()) {
			return;
		}

		if (!GameManager::sharedState()->getGameVariable(GameVariable::SHOW_CURSOR)) {
			if (PlatformToolbox::isControllerConnected()) {
				ControllerManager::getManager().hideCursor();
			}
		}
	}

	void showRetryLayer() {
		PlayLayer::showRetryLayer();

		if (PlatformToolbox::isControllerConnected()) {
			ControllerManager::getManager().showCursor();
		}
	}

	void showEndLayer() {
		PlayLayer::showEndLayer();

		if (PlatformToolbox::isControllerConnected()) {
			ControllerManager::getManager().showCursor();
		}
	}

	void onQuit() {
		PlayLayer::onQuit();

		if (PlatformToolbox::isControllerConnected()) {
			ControllerManager::getManager().showCursor();
		}
	}

	void pauseGame(bool p1) {
		PlayLayer::pauseGame(p1);

		// locked to !PlayLayer::getShowingEndLayer() && !PlayLayer::getEndTriggered()

		if (!AppDelegate::get()->getPaused()) {
			return;
		}

		if (PlatformToolbox::isControllerConnected()) {
			ControllerManager::getManager().showCursor();
		}
	}

/*
	void ~InputPlayLayer() {
		PlayLayer::~PlayLayer();

		if (PlatformToolbox::isControllerConnected()) {
			ControllerManager::getManager().showCursor();
		}
	}
*/
};

struct InputEndLevelLayer : geode::Modify<InputEndLevelLayer, EndLevelLayer> {
	void onReplay(cocos2d::CCObject* target) {
		EndLevelLayer::onReplay(target);

		if (!GameManager::sharedState()->getGameVariable(GameVariable::SHOW_CURSOR)) {
			if (PlatformToolbox::isControllerConnected()) {
				ControllerManager::getManager().hideCursor();
			}
		}
	}
};

struct InputRetryLevelLayer : geode::Modify<InputRetryLevelLayer, RetryLevelLayer> {
	void RetryLevelLayer_onReplay(RetryLevelLayer* self, cocos2d::CCObject* target) {
		RetryLevelLayer::onReplay(target);

		if (!GameManager::sharedState()->getGameVariable(GameVariable::SHOW_CURSOR)) {
			if (PlatformToolbox::isControllerConnected()) {
				ControllerManager::getManager().hideCursor();
			}
		}
	}
};
