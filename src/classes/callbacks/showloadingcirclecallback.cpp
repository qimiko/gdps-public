#include "classes/callbacks/showloadingcirclecallback.hpp"

void ShowLoadingCircleCallback::onCircle(cocos2d::CCNode*) {
    if (loadingCircle_) {
        onRemoveCircle(nullptr);
    }

    auto currentScene = cocos2d::CCDirector::sharedDirector()->getRunningScene();
    loadingCircle_ = LoadingCircle::create();
    loadingCircle_->setTargetScene(currentScene);
    loadingCircle_->setDarkOverlay(true);

    loadingCircle_->show();

    // some hacks to get it to work properly for its one use case
    loadingCircle_->setZOrder(currentScene->getHighestChildZ() + 2);
    loadingCircle_->registerWithTouchDispatcher();
}

void ShowLoadingCircleCallback::onRemoveCircle(cocos2d::CCNode*) {
    if (loadingCircle_) {
        // undo our hack
        cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(
                loadingCircle_);
        loadingCircle_->removeFromParentAndCleanup(true);
        loadingCircle_ = nullptr;
    }
}

LoadingCircle* ShowLoadingCircleCallback::loadingCircle_ = nullptr;

void ShowLoadingCircleCallback::showLoadingCircle() {
	// ensure code is running on main thread
	auto callback = ShowLoadingCircleCallback::create();

	auto cc_func = cocos2d::CCCallFuncN::create(callback, static_cast<cocos2d::SEL_CallFuncN>(&ShowLoadingCircleCallback::onCircle));

	// this avoids a very yummy crash
	auto cc_sequence = cocos2d::CCSequence::createWithTwoActions(
		cocos2d::CCDelayTime::create(0.0f),
		cc_func
	);

	auto action_manager = cocos2d::CCDirector::sharedDirector()->getActionManager();
	action_manager->addAction(cc_sequence, callback, false);
}

void ShowLoadingCircleCallback::hideLoadingCircle() {
	auto callback = ShowLoadingCircleCallback::create();

	// put this in the callback, or else sometimes this could run before the show
	auto cc_func = cocos2d::CCCallFuncN::create(callback, static_cast<cocos2d::SEL_CallFuncN>(&ShowLoadingCircleCallback::onRemoveCircle));

	auto cc_sequence = cocos2d::CCSequence::createWithTwoActions(
		cocos2d::CCDelayTime::create(0.0f),
		cc_func
	);

	auto action_manager = cocos2d::CCDirector::sharedDirector()->getActionManager();
	action_manager->addAction(cc_sequence, callback, false);
}

#ifdef GEODE_IS_ANDROID
#include <Geode/cocos/platform/android/jni/JniHelper.h>

extern "C" {
[[gnu::visibility("default")]] void Java_com_kyurime_geometryjump_ModGlue_showLoadingCircle(JNIEnv * /* env */, jclass /* clazz */) {
    ShowLoadingCircleCallback::showLoadingCircle();
}

[[gnu::visibility("default")]] void Java_com_kyurime_geometryjump_ModGlue_removeLoadingCircle(JNIEnv * /* env */, jclass /* clazz */) {
	ShowLoadingCircleCallback::hideLoadingCircle();
}
}
#endif
