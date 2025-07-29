#include <Geode/modify/AppDelegate.hpp>

#include <imgui-cocos.hpp>

#include "classes/managers/overlaymanager.hpp"

struct OverlayAppDelegate : geode::Modify<OverlayAppDelegate, AppDelegate> {
	virtual bool applicationDidFinishLaunching() {
		if (!AppDelegate::applicationDidFinishLaunching()) {
			return false;
		}

		ImGuiCocos::get().setup([] {
			OverlayManager::get().setup();
		}).draw([] {
			OverlayManager::get().render();
		});

		return true;
	}
};
