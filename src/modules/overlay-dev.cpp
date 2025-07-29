#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include "base/cmake_config.hpp"
#include "base/game_variables.hpp"

#include "classes/loadingfailedhijacklayer.hpp"
#include "classes/managers/overlaymanager.hpp"
#include "classes/settings/settingspopup.hpp"

// i got this value by using my own device's dpi lol
constexpr float DPI_NORMAL_FACTOR = 432.0f;

bool LOGGER_TRANSLUCENT = false;
float LOGGER_ALPHA = 0.9f;
bool LOGGER_AUTOSCROLL = true;
bool LOGGER_SHOW_DEBUG = true;

bool SCENE_TRANSLUCENT = false;
float SCENE_ALPHA = 0.9f;

bool SHOW_DEBUG_OPTIONS = false;
bool SHOW_LOGGER = false;
bool SHOW_LOG_OVERLAY = true;
bool SHOW_DEMO = false;
bool SHOW_VARS = false;
bool SHOW_EXPLORER = false;

float DT_COUNTER = 0.0f;
uint32_t FRAME_COUNTER = 0u;
float CURRENT_FPS = 0.0f;

std::string get_name_for_node(cocos2d::CCNode* node) {
#ifdef GEODE_IS_WINDOWS
	return typeid(*node).name() + 6;
#else
	// stolen from mat (thanks mat)
	// and also stolen from https://stackoverflow.com/a/4541470

	auto name = typeid(*node).name();
	auto status = -1;

	std::unique_ptr<char, void (*)(void*)> res {
		abi::__cxa_demangle(name, nullptr, nullptr, &status),
		std::free
	};

	return (status == 0) ? res.get() : name;
#endif
}

void draw_tree_for_node(cocos2d::CCNode* node) {
	auto ccd = cocos2d::CCDirector::sharedDirector();
	auto scaleFactor = OverlayManager::get().scaleFactor();

	std::ostringstream uid_ss;
	uid_ss << node->m_uID;

	if (ImGui::TreeNode(uid_ss.str().c_str(), "%s", get_name_for_node(node).c_str())) {
		ImGui::PushItemWidth(std::min(ImGui::GetWindowWidth() * 0.65f, scaleFactor * 400.0f));

		{
			auto px = node->getPositionX();
			auto py = node->getPositionY();
			float pos_vals[] = {px, py};

			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			if (ImGui::DragFloat2("Pos.", pos_vals, 0.1f, -FLT_MAX, FLT_MAX, "%.1f")) {
				node->setPosition(cocos2d::CCPoint(pos_vals[0], pos_vals[1]));
			}
		}

		{
			auto scale = node->getScale();
			auto sx = node->getScaleX();
			auto sy = node->getScaleY();
			float scale_vals[] = {scale, sx, sy};

			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			if (ImGui::DragFloat3("Scale", scale_vals, 0.01f, -FLT_MAX, FLT_MAX, "%.2f")) {
				if (scale_vals[1] == scale_vals[2]) {
					node->setScale(scale_vals[0]);
				} else {
					node->setScaleX(scale_vals[1]);
					node->setScaleY(scale_vals[2]);
				}
			}
		}

		{
			auto rotation = node->getRotation();
			auto rx = node->getRotationX();
			auto ry = node->getRotationY();
			float rot_vals[] = {rotation, rx, ry};

			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			if (ImGui::DragFloat3("Rot.", rot_vals, 0.1f, -FLT_MAX, FLT_MAX, "%.1f")) {
				if (rot_vals[1] == rot_vals[2]) {
					node->setRotation(rot_vals[0]);
				} else {
					node->setRotationX(rot_vals[1]);
					node->setRotationY(rot_vals[2]);
				}
			}
		}

		{
			auto is_visible = node->isVisible();
			if (ImGui::Checkbox("Visible", &is_visible)) {
				node->setVisible(is_visible);
			}
		}

		if (geode::cast::typeinfo_cast<cocos2d::CCRGBAProtocol*>(node) != nullptr) {
			auto c_node = geode::cast::typeinfo_cast<cocos2d::CCRGBAProtocol*>(node);

			auto color = c_node->getColor();
			auto opacity = c_node->getOpacity();

			float color_vals[] = {
				static_cast<float>(color.r) / 255.0f,
				static_cast<float>(color.g) / 255.0f,
				static_cast<float>(color.b) / 255.0f,
				static_cast<float>(opacity) / 255.0f
			};

			// NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			if (ImGui::ColorEdit4("Color", color_vals, ImGuiColorEditFlags_Uint8)) {
				c_node->setColor(cocos2d::ccColor3B {
					static_cast<uint8_t>(color_vals[0] * 255),
					static_cast<uint8_t>(color_vals[1] * 255),
					static_cast<uint8_t>(color_vals[2] * 255)
				});
				c_node->setOpacity(color_vals[3] * 255);
			}
		}

		if (node->getChildrenCount() >= 1) {
			if (ImGui::TreeNode("node_children", "Children (%i)", node->getChildrenCount())) {
				auto children = node->getChildren();
				for (auto i = 0u; i < children->count(); i++) {
					auto child = reinterpret_cast<cocos2d::CCNode*>(children->objectAtIndex(i));

					draw_tree_for_node(child);
				}

				ImGui::TreePop();
			}
		}

		ImGui::PopItemWidth();
		ImGui::TreePop();
	}
}

void imgui_draw_cocos_window() {
	constexpr auto window_height = 600.0f;
	constexpr auto window_width = 800.0f;
	constexpr auto window_xoffset = 120.0f;
	constexpr auto window_yoffset = 60.0f;

	auto eglview = cocos2d::CCEGLView::sharedOpenGLView();
	auto pixel_size = eglview->getFrameSize();

	auto scaleFactor = OverlayManager::get().scaleFactor();

	ImGui::SetNextWindowSize(ImVec2(scaleFactor * window_width, scaleFactor * window_height), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(window_xoffset, (pixel_size.height - window_height) - window_yoffset), ImGuiCond_FirstUseEver);

	float backup_alpha = ImGui::GetStyle().Alpha;
	if (SCENE_TRANSLUCENT) {
		ImGui::GetStyle().Alpha = SCENE_ALPHA;
	}

	if (ImGui::Begin("Scene", &SHOW_EXPLORER)) {
		if (ImGui::BeginPopup("Options")) {
			ImGui::Checkbox("Translucent", &SCENE_TRANSLUCENT);
			if (SCENE_TRANSLUCENT) {
				ImGui::SliderFloat("Alpha", &SCENE_ALPHA, 0.25f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
			}

			ImGui::EndPopup();
		}

		if (ImGui::Button("Options")) {
			ImGui::OpenPopup("Options");
		}

		auto scene = cocos2d::CCDirector::sharedDirector()->getRunningScene();

		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		if (scene == nullptr) {
			ImGui::Text("No running scene!");
		} else {
			ImGui::SetNextItemOpen(true);
			draw_tree_for_node(scene);
		}

		ImGui::EndChild();
	}

	if (ImGui::GetStyle().Alpha != backup_alpha) {
		ImGui::GetStyle().Alpha = backup_alpha;
	}

	ImGui::End();
}

ImVec4 get_log_color(geode::Severity level, bool useDark = false) {
	switch (level) {
	case geode::Severity::Debug:
		return useDark ? ImVec4(0.8f, 0.8f, 0.8f, 1.0f) : ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	case geode::Severity::Info:
		return useDark ? ImVec4(1.0f, 1.0f, 1.0f, 1.0f) : ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	case geode::Severity::Warning:
		// 255, 221, 87 or 193, 131, 1, 255
		return useDark ? ImVec4(1.0f, 0.867f, 0.341f, 1.0f) : ImVec4(0.757f, 0.514f, 0.004f, 1.0f);
	case geode::Severity::Error:
		// 255, 52, 33 or 228, 86, 73, 255
		return useDark ? ImVec4(1.0f, 0.204f, 0.129f, 1.0f) : ImVec4(0.894f, 0.337f, 0.286f, 1.0f);
	default:
		return {0.0f, 0.0f, 0.0f, 1.0f};
	}
}

void imgui_draw_logs(bool useDark = false) {
#ifdef GEODE_IS_ANDROID
	auto& ring_buffer = geode::log::lines();
	for (const auto& line : ring_buffer) {
		if (!LOGGER_SHOW_DEBUG && (line.getSeverity() == geode::Severity::Debug)) {
			continue;
		}

		ImGui::PushStyleColor(ImGuiCol_Text, get_log_color(line.getSeverity(), useDark));

		ImGui::TextWrapped("%s", line.toString().c_str());

		ImGui::PopStyleColor();
	}
#endif
}

void imgui_draw_log_window() {
#ifdef GEODE_IS_ANDROID
	constexpr auto window_height = 600.0f;
	constexpr auto window_width = 800.0f;
	constexpr auto window_xoffset = 60.0f;
	constexpr auto window_yoffset = 60.0f;

	auto eglview = cocos2d::CCEGLView::sharedOpenGLView();
	auto pixel_size = eglview->getFrameSize();

	auto scaleFactor = OverlayManager::get().scaleFactor();

	ImGui::SetNextWindowSize(ImVec2(scaleFactor * window_width, scaleFactor * window_height), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(ImVec2(window_xoffset, (pixel_size.height - window_height) - window_yoffset), ImGuiCond_FirstUseEver);

	// push/pop style crashed for... whatever reason
	float backup_alpha = ImGui::GetStyle().Alpha;
	if (LOGGER_TRANSLUCENT) {
		ImGui::GetStyle().Alpha = LOGGER_ALPHA;
	}

	if (ImGui::Begin("Logs", &SHOW_LOGGER)) {
		if (ImGui::BeginPopup("Options")) {
			ImGui::Checkbox("Translucent", &LOGGER_TRANSLUCENT);
			if (LOGGER_TRANSLUCENT) {
				ImGui::SliderFloat("Alpha", &LOGGER_ALPHA, 0.25f, 1.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
			}

			ImGui::Checkbox("Auto-scroll", &LOGGER_AUTOSCROLL);
			ImGui::Checkbox("Show Debug", &LOGGER_SHOW_DEBUG);

			if (ImGui::Button("Test Logs")) {
				geode::log::debug("test debug {}", 0);
				geode::log::info("test info {}", 1);
				geode::log::warn("test warn {}", 2);
				geode::log::error("test error {}", 3);
			}

			ImGui::EndPopup();
		}

		if (ImGui::Button("Options")) {
			ImGui::OpenPopup("Options");
		}

		ImGui::Separator();

		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, 0);

		imgui_draw_logs();

		if (LOGGER_AUTOSCROLL && (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
			ImGui::SetScrollHereY(1.0f);
		}

		ImGui::EndChild();
	}

	if (ImGui::GetStyle().Alpha != backup_alpha) {
		ImGui::GetStyle().Alpha = backup_alpha;
	}

	ImGui::End();
#endif
}

void imgui_draw_vars_window() {
	if (ImGui::Begin("Variables", &SHOW_VARS, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
		auto gm = GameManager::sharedState();

		auto nc_val = gm->getGameVariable(GameVariable::IGNORE_DAMAGE);
		if (ImGui::Checkbox("Ignore Damage", &nc_val)) {
			gm->toggleGameVariable(GameVariable::IGNORE_DAMAGE);
		}

		auto pm_val = gm->getGameVariable(GameVariable::PRACTICE_MUSIC);
		if (ImGui::Checkbox("Practice Music", &pm_val)) {
			gm->toggleGameVariable(GameVariable::PRACTICE_MUSIC);
		}

		auto sc_val = gm->getGameVariable(GameVariable::SPEED_CONTROLS);
		if (ImGui::Checkbox("Speedhack Controls", &sc_val)) {
			gm->toggleGameVariable(GameVariable::SPEED_CONTROLS);
		}

		auto ru_val = gm->getGameVariable(GameVariable::REPLAY_CONTROLS);
		if (ImGui::Checkbox("Replay Utils", &ru_val)) {
			gm->toggleGameVariable(GameVariable::REPLAY_CONTROLS);
		}

#ifdef GDMOD_ENABLE_LOGGING
		auto vfb_val = gm->getGameVariable(GameVariable::BYPASS_VERIFY);
		if (ImGui::Checkbox("Bypass Verify", &vfb_val)) {
			gm->toggleGameVariable(GameVariable::BYPASS_VERIFY);
		}

		auto unlock_val = gm->getGameVariable(GameVariable::ALL_ICON_BYPASS);
		if (ImGui::Checkbox("Unlock Everything", &unlock_val)) {
			gm->toggleGameVariable(GameVariable::ALL_ICON_BYPASS);
		}
#endif
	}

	ImGui::End();
}

void imgui_draw_overlay_options() {
	auto title_string = fmt::format("1.9 GDPS u{}", CMakeConfiguration::VERSION);

	ImGui::Begin(title_string.c_str(), &SHOW_DEBUG_OPTIONS, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

	// seems to be necessary from https://github.com/ocornut/imgui/issues/331
	auto show_delete_popup = false;
	auto show_restore_popup = false;
	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Windows")) {
#ifdef GEODE_IS_ANDROID
			ImGui::MenuItem("Logs", nullptr, &SHOW_LOGGER);
#endif
			ImGui::MenuItem("Variables", nullptr, &SHOW_VARS);
			ImGui::MenuItem("Scene", nullptr, &SHOW_EXPLORER);
			ImGui::Separator();
			ImGui::MenuItem("Demo", nullptr, &SHOW_DEMO);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Shortcuts")) {
			if (ImGui::MenuItem("Open MoreOptionsLayer")) {
				MoreOptionsLayer::create()->show();
			}

			if (ImGui::MenuItem("Open SettingsPopup")) {
				SettingsPopup::create()->show();
			}

			if (ImGui::MenuItem("Open LoadingFailedHijackLayer")) {
				auto loadingFailedLayer = LoadingFailedHijackLayer::scene();
				cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->setTargetPrio(-500);
				cocos2d::CCDirector::sharedDirector()->runWithScene(loadingFailedLayer);
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Clear Save Data")) {
				show_delete_popup = true;
			}


			if (ImGui::MenuItem("Test Cloud Restore")) {
				show_restore_popup = true;
			}

			if (ImGui::MenuItem("Unlock Secret")) {
				auto gsm = GameStatsManager::sharedState();
				if (gsm->hasUniqueItem("gnome02")) {
					// clear
					GameManager::sharedState()->setGameVariable(GameVariable::SECRET_COMPLETED, false);
					gsm->m_valueDict->removeObjectForKey("unique_gnome02");
					gsm->incrementStat("8", -1);
				} else {
					GameManager::sharedState()->setGameVariable(GameVariable::SECRET_COMPLETED, true);
				}
			}

			if (ImGui::MenuItem("Crash Game")) {
				geode::log::warn("manual crash called");

				reinterpret_cast<cocos2d::CCDirector*>(0x0)->init();
			}

			if (ImGui::MenuItem("Save Game")) {
				auto file = cocos2d::CCFileUtils::sharedFileUtils();

				geode::log::info("calling for game save");

				AppDelegate::get()->trySaveGame();
			}

			if (ImGui::MenuItem("Reload All")) {
				GameManager::sharedState()->reloadAll(false, false, false);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	if (show_delete_popup) {
		ImGui::OpenPopup("Clear Save Data?");
	}

	if (ImGui::BeginPopupModal(
			"Clear Save Data?"
		)) {
		ImGui::Text("This will immediately reset all game progress.\nThis operation cannot be undone!");

		ImGui::Separator();

		if (ImGui::Button("Continue")) {
			geode::log::warn("clearing all save data");

			GameManager::sharedState()->firstLoad();
			LocalLevelManager::sharedState()->firstLoad();
			GJAccountManager::sharedState()->unlinkFromAccount();

			cocos2d::CCDirector::sharedDirector()->replaceScene(
				MenuLayer::scene(false)
			);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


	if (show_restore_popup) {
		ImGui::OpenPopup("Test Save Restore?");
	}

	if (ImGui::BeginPopupModal(
			"Test Save Restore?"
		)) {
		ImGui::Text("This will immediately reset all game progress.\nThis operation cannot be undone!");

		ImGui::Separator();

		if (ImGui::Button("Continue")) {
			auto gm = GameManager::sharedState();
			gm->m_quickSave = true;
			std::string saveString = gm->getCompressedSaveString();
			gm->m_quickSave = false;

			saveString += ";";

			auto llm = LocalLevelManager::sharedState();
			llm->updateLevelOrder();
			auto llmString = llm->getCompressedSaveString();
			saveString += llmString;

			// clear save data to see how restore goes
			GameManager::sharedState()->firstLoad();
			LocalLevelManager::sharedState()->firstLoad();

			cocos2d::CCDirector::sharedDirector()->replaceScene(
				MenuLayer::scene(false)
			);

			GJAccountManager::sharedState()->onSyncAccountCompleted(saveString, "sync_account");

			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	ImGui::Text("FPS: %.1f", CURRENT_FPS);
	ImGui::Text("%s-%s", CMakeConfiguration::BRANCH, CMakeConfiguration::HASH);
	ImGui::Text("Development Build");

	ImGui::End();
}

void imgui_draw_log_overlay() {
#ifdef GEODE_IS_ANDROID
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_NoNav
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoInputs
		| ImGuiWindowFlags_NoBackground;

	auto eglview = cocos2d::CCEGLView::sharedOpenGLView();
	auto pixel_size = eglview->getFrameSize();

	auto scaleFactor = OverlayManager::get().scaleFactor();

	auto window_pos = ImVec2(10.0f, 10.0f);
	auto window_size = ImVec2(pixel_size.width, scaleFactor * 600.0f);

	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

	if (ImGui::Begin("Log Overlay", nullptr, window_flags)) {
		imgui_draw_logs(true);

		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
			ImGui::SetScrollHereY(1.0f);
		}
	}
	ImGui::End();
#endif
}

void imgui_stylecolorslightgreen() {
	auto font = OverlayManager::get().getFont(Overlay::Font::Monospace);
	ImGui::PushFont(font);

	// fix scale for mobile but in a lazy way
#ifdef GEODE_IS_ANDROID
	auto sf = 3.5f;
#else
	auto sf = 1.0f;
#endif

	// copied from https://github.com/ocornut/imgui/issues/707#issuecomment-439117182
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f * sf);
	ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 3.0f * sf);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 6.0f * sf);
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 2.0f * sf);
	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 3.0f * sf);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f * sf);

	ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 12.0f * sf);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f * sf);
	ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f * sf);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10.0f * sf, 6.0f * sf));
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 22.0f * sf);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f * sf, 4.0f * sf));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f * sf, 0.5f * sf));

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);

	// style->AntiAliasedLines = true;
	// style->AntiAliasedFill = true;

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.00f, 0.00f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.60f, 0.60f, 0.60f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.00f, 0.00f, 0.00f, 0.25f));
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.00f, 0.00f, 0.00f, 0.04f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.85f, 0.85f, 0.85f, 0.55f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.94f, 0.94f, 0.94f, 0.55f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.71f, 0.78f, 0.69f, 0.98f));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 1.0f, 1.0f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0.00f, 0.00f, 0.00f, 0.00f));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(0.00f, 0.00f, 0.00f, 0.10f));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, ImVec4(0.00f, 0.00f, 0.00f, 0.15f));
	ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, ImVec4(0.00f, 0.00f, 0.00f, 0.20f));
	ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.184f, 0.407f, 0.193f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.184f, 0.407f, 0.193f, 0.78f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.184f, 0.407f, 0.193f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.71f, 0.78f, 0.69f, 0.40f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.725f, 0.805f, 0.702f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.793f, 0.900f, 0.836f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.71f, 0.78f, 0.69f, 0.31f));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.71f, 0.78f, 0.69f, 0.80f));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.71f, 0.78f, 0.69f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0.39f, 0.39f, 0.39f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(0.14f, 0.44f, 0.80f, 0.78f));
	ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(0.14f, 0.44f, 0.80f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_ResizeGrip, ImVec4(1.00f, 1.00f, 1.00f, 0.00f));
	ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, ImVec4(0.725f, 0.805f, 0.702f, 0.70f));
	ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, ImVec4(0.793f, 0.900f, 0.836f, 0.80f));
	ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.39f, 0.39f, 0.39f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_PlotLinesHovered, ImVec4(1.00f, 0.43f, 0.35f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.90f, 0.70f, 0.00f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, ImVec4(1.00f, 0.60f, 0.00f, 1.00f));
	ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4(0.26f, 0.59f, 0.98f, 0.35f));
	ImGui::PushStyleColor(ImGuiCol_DragDropTarget, ImVec4(0.26f, 0.59f, 0.98f, 0.95f));
	ImGui::PushStyleColor(ImGuiCol_NavHighlight, ImVec4(0.71f, 0.78f, 0.69f, 0.80f));
	ImGui::PushStyleColor(ImGuiCol_NavWindowingHighlight, ImVec4(0.70f, 0.70f, 0.70f, 0.70f));
}

void imgui_pop_styles() {
	ImGui::PopStyleVar(14);
	ImGui::PopStyleColor(41);
	ImGui::PopFont();
}

void dev_imgui_tick() {
	ImGuiIO& io = ImGui::GetIO();

	imgui_stylecolorslightgreen();

	/*
		// todo: use cocos methods for this (if needed)
	static bool WantTextInputLast = false;
	if (io.WantTextInput && !WantTextInputLast)
		ShowSoftKeyboardInput();
	WantTextInputLast = io.WantTextInput;
*/

	if (SHOW_DEBUG_OPTIONS) {
		imgui_draw_overlay_options();
	}

	if (SHOW_LOGGER) {
		imgui_draw_log_window();
	}

	if (SHOW_LOG_OVERLAY) {
		imgui_draw_log_overlay();
	}

	if (SHOW_DEMO) {
		ImGui::ShowDemoWindow(&SHOW_DEMO);
	}

	if (SHOW_VARS) {
		imgui_draw_vars_window();
	}

	if (SHOW_EXPLORER) {
		imgui_draw_cocos_window();
	}

	auto window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground;
	auto height = ImGui::GetFrameHeight();

	ImGui::PushStyleColor(ImGuiCol_MenuBarBg, ImVec4(0.5f, 0.5f, 0.5f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	if (ImGui::BeginViewportSideBar("##MainStatusBar", nullptr, ImGuiDir_Down, height, window_flags)) {
		if (ImGui::BeginMenuBar()) {
			ImGui::Text("DEV %s-%s", CMakeConfiguration::BRANCH, CMakeConfiguration::HASH);
			ImGui::Text("FPS: %.1f", CURRENT_FPS);

			ImGui::MenuItem("More", nullptr, &SHOW_DEBUG_OPTIONS);

#ifdef GEODE_IS_ANDROID
			ImGui::MenuItem("Logs", nullptr, &SHOW_LOG_OVERLAY);

			if (ImGui::MenuItem("F11")) {
				cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher()->dispatchKeyboardMSG(cocos2d::KEY_F11, true);
				cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher()->dispatchKeyboardMSG(cocos2d::KEY_F11, false);
			}

			if (ImGui::MenuItem("Tab")) {
				cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher()->dispatchKeyboardMSG(cocos2d::KEY_Tab, true);
				cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher()->dispatchKeyboardMSG(cocos2d::KEY_Tab, false);
			}
#endif

			ImGui::EndMenuBar();
		}
	}
	ImGui::End();

	ImGui::PopStyleColor(2);

	imgui_pop_styles();
}

struct OverlayCCScheduler : geode::Modify<OverlayCCScheduler, cocos2d::CCScheduler> {
	void update(float dt) {
		FRAME_COUNTER++;
		DT_COUNTER += dt;

		if (DT_COUNTER >= 0.5f) {
			CURRENT_FPS = FRAME_COUNTER / DT_COUNTER;

			FRAME_COUNTER = 0u;
			DT_COUNTER = 0.0f;
		}

		return cocos2d::CCScheduler::update(dt);
	}
};

$execute {
	OverlayManager::get().registerOverlay(&dev_imgui_tick);
}
