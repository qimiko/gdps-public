#include "classes/callbacks/reloadgamecallback.hpp"

void ReloadGameCallback::onReload(cocos2d::CCNode*) {
    GameManager::sharedState()->reloadAll(false, false, true);
}

void ReloadGameCallback::reloadGame() {
	auto callback = ReloadGameCallback::create();

	auto cc_func = cocos2d::CCCallFuncN::create(
		callback, static_cast<cocos2d::SEL_CallFuncN>(&ReloadGameCallback::onReload)
	);

	auto cc_sequence = cocos2d::CCSequence::createWithTwoActions(
		cocos2d::CCDelayTime::create(0.0f),
		cc_func
	);

	auto action_manager = cocos2d::CCDirector::sharedDirector()->getActionManager();
	action_manager->addAction(cc_sequence, callback, false);
}

#ifdef GEODE_IS_ANDROID
#include <Geode/cocos/platform/android/jni/JniHelper.h>

extern "C"
JNIEXPORT void JNICALL Java_com_kyurime_geometryjump_ModGlue_onTextureDirectoryChosen(JNIEnv *, jclass) {
    ReloadGameCallback::reloadGame();
}
#endif
