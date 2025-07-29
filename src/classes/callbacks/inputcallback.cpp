#include <Geode/Geode.hpp>

#ifdef GEODE_IS_ANDROID

#include <android/keycodes.h>
#include <Geode/cocos/platform/android/jni/JniHelper.h>

#include "classes/managers/controllermanager.hpp"

extern "C" {

[[gnu::visibility("default")]] jboolean Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeActionJoystickMove(JNIEnv * /* env */, jclass /* clazz */, float leftX, float leftY, float rightX, float rightY) {
    return ControllerManager::getManager().updateJoystickValues({ leftX, leftY }, { rightX, rightY });
}

[[gnu::visibility("default")]] jboolean Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeActionMouseMove(JNIEnv * /* env */, jclass /* clazz */, float x, float y) {
    return ControllerManager::getManager().pushCursor(x * 2.0f, -y * 2.0f);
}

// this is tangentially related
[[gnu::visibility("default")]] void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeSurfaceChanged(JNIEnv * /* env */, jclass /* clazz */, int width, int height) {
    auto fWidth = static_cast<float>(width);
    auto fHeight = static_cast<float>(height);

    cocos2d::CCEGLView::sharedOpenGLView()->setFrameSize(fWidth, fHeight);
    cocos2d::CCDirector::sharedDirector()->updateScreenScale({ fWidth, fHeight });
    cocos2d::CCDirector::sharedDirector()->setViewport();
    cocos2d::CCDirector::sharedDirector()->setProjection(cocos2d::kCCDirectorProjection2D);
}

[[gnu::visibility("default")]] bool Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeKeyUp(
    JNIEnv* env, jobject, jint keycode, jint modifiers
) {
    if (keycode == AKEYCODE_BUTTON_A) {
        ControllerManager::getManager().handleClickUp();
        return true;
    }

    // back/menu keys
    if (keycode != AKEYCODE_BACK && keycode != AKEYCODE_MENU) {
        auto keyboard_dispatcher = cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher();
        auto translated_code = ControllerManager::translateAndroidKeyCodeToWindows(keycode);

        auto isShiftPressed = modifiers & 0x1;
        auto isCtrlPressed = modifiers & 0x1000;
        auto isAltPressed = modifiers & 0x2;

        keyboard_dispatcher->updateModifierKeys(
            isShiftPressed,
            isCtrlPressed,
            isAltPressed,
            false
        );

        return keyboard_dispatcher->dispatchKeyboardMSG(translated_code, false);
    }

    return true;
}

[[gnu::visibility("default")]] bool Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeKeyDown2(
	JNIEnv* env, jobject, jint keycode, jint modifiers
) {
    if (keycode != AKEYCODE_BACK && keycode != AKEYCODE_MENU) {
        auto keyboard_dispatcher = cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher();
        auto translated_code = ControllerManager::translateAndroidKeyCodeToWindows(keycode);

        auto isShiftPressed = modifiers & 0x1;
        auto isCtrlPressed = modifiers & 0x1000;
        auto isAltPressed = modifiers & 0x2;

        keyboard_dispatcher->updateModifierKeys(isShiftPressed, isCtrlPressed, isAltPressed, false);

        return keyboard_dispatcher->dispatchKeyboardMSG(translated_code, true);
    } else {
        auto keypad_dispatcher = cocos2d::CCDirector::sharedDirector()->getKeypadDispatcher();
        if (keycode == AKEYCODE_BACK) {
            return keypad_dispatcher->dispatchKeypadMSG(cocos2d::kTypeBackClicked);
        } else {
            return keypad_dispatcher->dispatchKeypadMSG(cocos2d::kTypeMenuClicked);
        }
    }
}

}
#endif
