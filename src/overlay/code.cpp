#include "imgui.h"
#include "menu.hpp"

void mat_draw() {
    Menu::get().draw_menu();

    static bool windows_dirty = true;

	if (ImGui::GetFrameCount() > 60 && windows_dirty) {
        Menu::get().sort_windows(false);
        windows_dirty = false;
	}

/*
    if (true) {
        if (ImGui::Begin("Config editor")) {
            ImGui::Text("Hello, world!");
            if (ImGui::SliderFloat("Width", &get_theme().width, 0.0f, 300.0f)) {
                windows_dirty = true;
            }
            if (ImGui::SliderFloat("Height", &get_theme().height, 0.0f, 100.0f)) {
                windows_dirty = true;
            }
            ImGui::SliderFloat("Font scale", &ImGui::GetIO().FontGlobalScale, 0.1f, 2.0f);
            static float initial_width = get_theme().width;
            static float initial_height = get_theme().height;
            static float total_scale = 1.0;
            if (ImGui::SliderFloat("Total scale", &total_scale, 0.1f, 3.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
                get_theme().width = initial_width * total_scale;
                get_theme().height = initial_height * total_scale;
                ImGui::GetIO().FontGlobalScale = total_scale;
                windows_dirty = true;
            }
            if (ImGui::Button("sort")) {
                windows_dirty = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("go away")) {
                Menu::get().leave_effect();
            }
            ImGui::SameLine();
            if (ImGui::Button("come back")) {
                Menu::get().come_back_effect();
            }
        }
        ImGui::End();

        ImGui::ShowStyleEditor();
    }
*/
}

#include "classes/managers/overlaymanager.hpp"

#include <Geode/modify/CCKeyboardDispatcher.hpp>

struct OverlayCCKeyboardDispatcher : geode::Modify<OverlayCCKeyboardDispatcher, cocos2d::CCKeyboardDispatcher> {
    bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool isDown) {
		auto r = cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG(key, isDown);

        if (!isDown || key == cocos2d::enumKeyCodes::KEY_Unknown || this->m_bAltPressed || this->m_bControlPressed || this->m_bCommandPressed || this->m_bShiftPressed) {
            return r;
        }

        auto& menu = Menu::get();
        if (menu.is_listening()) {
            menu.update_bind(key);
        } else if (key == menu.current_bind()) {
            menu.toggle();
        }

        return r;
	}
};

$execute {
	OverlayManager::get().registerOverlay(&mat_draw);
}
