#include <Geode/cocos/platform/android/jni/JniHelper.h>

#include <fstream>

#include "base/platform_helper.hpp"
#include "classes/callbacks/levelimportcallback.hpp"

bool PlatformHelper::is_gd_installed() {
	cocos2d::JniMethodInfo t;
	auto ret = false;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "isGeometryDashInstalled", "()Z")) {
		ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID);
	}

	return ret;
}

void PlatformHelper::keep_screen_awake() {
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "keepScreenAwake", "()V")) {
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}

void PlatformHelper::remove_screen_awake() {
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "removeScreenAwake", "()V")) {
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}

void PlatformHelper::open_texture_picker() {
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "showTexturePicker", "()V")) {
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}

void PlatformHelper::apply_classic_pack() {
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "applyClassicPack", "()V")) {
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}

void PlatformHelper::wipe_textures_directory() {
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "wipeTexturesDirectory", "()V")) {
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}

namespace {
void pick_level_export(const std::string& name) {
	if (name.empty()) {
		return;
	}

	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "onExportLevel", "(Ljava/lang/String;)V")) {
		auto jname = t.env->NewStringUTF(name.c_str());
		t.env->CallStaticVoidMethod(t.classID, t.methodID, jname);
	}
}

std::string get_level_export_path() {
	cocos2d::JniMethodInfo t;
	std::string ret("");

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "getLevelExportPath", "()Ljava/lang/String;")) {
		auto str = reinterpret_cast<jstring>(t.env->CallStaticObjectMethod(t.classID, t.methodID));
		t.env->DeleteLocalRef(t.classID);
		ret = cocos2d::JniHelper::jstring2string(str);
		t.env->DeleteLocalRef(str);
	}

	return ret;
}
}

void PlatformHelper::export_level(std::string name, std::string export_string) {
	if (name.empty()) {
		name = "Unnamed";
	}

	auto output_path = get_level_export_path();

	auto out = std::ofstream(output_path, std::ofstream::out);

	out << export_string;

	out.close();

	pick_level_export(name);
}

void PlatformHelper::import_level() {
	using namespace geode::utils;

	file::pickReadBytes({
		.defaultPath = std::nullopt,
		.filters = {
			{"GD Level Files", {"*.gmd"}}
		},
	}).listen(
		[=](geode::Result<std::span<const std::uint8_t>>* result) {
			if (!result->ok()) {
				return;
			}

			LevelImportCallback::importLevelBytes(result->unwrap());
		}
	);
}

void PlatformHelper::loaded_to_menu() {
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "loadedToMenu", "()V")) {
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}

std::string PlatformHelper::get_textures_directory() {
	cocos2d::JniMethodInfo t;
	std::string ret("");

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "getTexturesDirectory", "()Ljava/lang/String;")) {
		auto str = reinterpret_cast<jstring>(t.env->CallStaticObjectMethod(t.classID, t.methodID));
		t.env->DeleteLocalRef(t.classID);
		ret = cocos2d::JniHelper::jstring2string(str);
		t.env->DeleteLocalRef(str);
	}

	return ret;
}

bool PlatformHelper::is_controller_connected() {
	cocos2d::JniMethodInfo t;
	auto ret = false;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "isControllerConnected", "()Z")) {
		ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID);
	}

	return ret;
}

std::string PlatformHelper::get_secondary_assets_directory() {
	cocos2d::JniMethodInfo t;
	std::string ret("");

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "getSecondaryAssetsDirectory", "()Ljava/lang/String;")) {
		auto str = reinterpret_cast<jstring>(t.env->CallStaticObjectMethod(t.classID, t.methodID));
		t.env->DeleteLocalRef(t.classID);
		ret = cocos2d::JniHelper::jstring2string(str);
		t.env->DeleteLocalRef(str);
	}

	return ret;
}

std::string PlatformHelper::get_save_directory() {
	cocos2d::JniMethodInfo t;
	std::string ret("");

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "getSaveDirectory", "()Ljava/lang/String;")) {
		auto str = reinterpret_cast<jstring>(t.env->CallStaticObjectMethod(t.classID, t.methodID));
		t.env->DeleteLocalRef(t.classID);
		ret = cocos2d::JniHelper::jstring2string(str);
		t.env->DeleteLocalRef(str);
	}

	return ret;
}

bool PlatformHelper::is_launcher_build() {
	cocos2d::JniMethodInfo t;
	auto ret = false;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "isLauncherBuild", "()Z")) {
		ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID);
	}

	return ret;
}

bool PlatformHelper::is_screen_restricted() {
	cocos2d::JniMethodInfo t;
	auto ret = false;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "isScreenRestricted", "()Z")) {
		ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID);
	}

	return ret;
}

void PlatformHelper::toggle_is_screen_restricted() {
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "toggleIsScreenRestricted", "()V")) {
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}

void PlatformHelper::capture_cursor() {
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "captureCursor", "()V")) {
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}

void PlatformHelper::release_cursor() {
	cocos2d::JniMethodInfo t;

	if (cocos2d::JniHelper::getStaticMethodInfo(t, "com/kyurime/geometryjump/ModGlue", "releaseCursor", "()V")) {
		t.env->CallStaticVoidMethod(t.classID, t.methodID);
	}
}
