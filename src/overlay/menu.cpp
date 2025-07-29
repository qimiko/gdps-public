#include "imgui.h"
#include "menu.hpp"

#include <fmt/format.h>

#include "classes/managers/overlaymanager.hpp"
#include "base/game_variables.hpp"

Theme& get_theme() {
	static Theme theme;
	return theme;
}

void update_theme() {
	auto& theme = get_theme();

	constexpr auto INITIAL_WIDTH = 200.0f;
	constexpr auto INITIAL_HEIGHT = 26.0f;

	auto scale = OverlayManager::get().scale();

	theme.width = INITIAL_WIDTH * scale;
	theme.height = INITIAL_HEIGHT * scale;
}

void updateOverlayHacks() {
	auto pl = GameManager::sharedState()->m_playLayer;
	if (!pl) {
		return;
	}

	if (auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(pl->getUserObject("hacks"_spr))) {
		extension_object->updateCheats();
	}
}

Menu::Menu() {
	auto scale = std::max(GameManager::sharedState()->getIntGameVariable(GameVariable::OVERLAY_SCALE) / 100.0f, 1.0f);
	auto bind_button = std::make_unique<BindingMenuItem>();
	m_bind_button = bind_button.get();

	auto current_key = static_cast<cocos2d::enumKeyCodes>(GameManager::sharedState()->getIntGameVariable(GameVariable::OVERLAY_KEYBIND));
	if (current_key != cocos2d::enumKeyCodes::KEY_None && current_key != cocos2d::enumKeyCodes::KEY_Unknown) {
		m_open_key = current_key;
	}

	m_windows.emplace_back("Gameplay")
		.add(std::make_unique<HackGameVariableMenuItem>("Safe NoClip", GameVariable::IGNORE_DAMAGE))
		.add(std::make_unique<GameVariableMenuItem>("Practice Music", GameVariable::PRACTICE_MUSIC))
		.add(std::make_unique<GameVariableMenuItem>("Hitboxes on Death", GameVariable::SHOW_HITBOXES_ON_DEATH))
		.add(std::make_unique<GameVariableMenuItem>("Fast Practice Reset", GameVariable::FAST_PRACTICE_RESET))
		.add(std::make_unique<CBGameVariableMenuItem>("Show Percentage", GameVariable::SHOW_PERCENTAGE, [](CBGameVariableMenuItem&) {
			auto playLayer = GameManager::sharedState()->m_playLayer;
			if (!playLayer) {
				return;
			}

			if (auto percent_label = geode::cast::typeinfo_cast<cocos2d::CCLabelBMFont*>(playLayer->getChildByID("percentage-label"_spr))) {
				percent_label->setVisible(GameManager::sharedState()->getGameVariable(GameVariable::SHOW_PERCENTAGE));
			}
		}))
		.add(std::make_unique<GameVariableMenuItem>("Editor Hitboxes", GameVariable::SHOW_EDITOR_HITBOXES));

#ifdef GEODE_IS_WINDOWS
	m_windows.emplace_back("FPS Bypass")
		.add(std::make_unique<RowMenuItem>(
			std::make_unique<CBGameVariableMenuItem>("Enabled", GameVariable::ENABLE_FPS_BYPASS, [](CBGameVariableMenuItem&) {
				updateFPSValue();
			}),
			std::make_unique<FPSInputMenuItem>()
		));
#endif

	m_windows.emplace_back("Speedhack")
		.add(std::make_unique<RowMenuItem>(
			std::make_unique<CBGameVariableMenuItem>("Enabled", GameVariable::OVERLAY_SPEEDHACK_ENABLED, [](CBGameVariableMenuItem&) {
				updateSpeedhack();
			}),
			std::make_unique<SpeedInputMenuItem>()
		));

	m_windows.emplace_back("Options")
		.add(std::make_unique<CBButtonMenuItem>(fmt::format("Set Scale: {:.2f}x", scale), [](CBButtonMenuItem& self) {
			auto currentScale = OverlayManager::get().scale();

			currentScale += 0.25f;

			if (currentScale > 2.0f) {
				currentScale = 0.75f;
			}

			OverlayManager::get().setScale(currentScale);
			update_theme();

			Menu::get().m_update_windows = 2;

			self.label = fmt::format("Set Scale: {:.2f}x", currentScale);

			GameManager::sharedState()->setIntGameVariable(GameVariable::OVERLAY_SCALE, static_cast<int>(currentScale * 100.0f));
		}))
		.add(std::make_unique<CBButtonMenuItem>("Sort Windows", [](CBButtonMenuItem&) {
			Menu::get().sort_windows();
		}))
		.add(std::move(bind_button));
}

void Menu::draw_menu() {
	if (m_leaving) {
		m_animation_time += ImGui::GetIO().DeltaTime;
		if (m_animation_time >= m_animation_duration) {
			return;
		}
	}

	// auto& style = ImGui::GetStyle();

	// style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	// style.WindowPadding = ImVec2(0, 0);
	// style.WindowBorderSize = 0;

	// style.ItemSpacing = ImVec2(0, 0);

	// style.CellPadding = ImVec2(0, 0);

	// auto colors = style.Colors;
	// colors[ImGuiCol_TitleBgActive] = ImColor(Colors::WINDOW);
	// colors[ImGuiCol_TitleBg] = ImColor(Colors::WINDOW);
	// colors[ImGuiCol_TitleBgCollapsed] = ImColor(Colors::WINDOW);
	// colors[ImGuiCol_WindowBg] = ImColor(Colors::BACKGROUND);

	// // for the little window collapse button
	// colors[ImGuiCol_ButtonHovered] = ImColor(Colors::BACKGROUND_HOVER);
	// colors[ImGuiCol_ButtonActive] = ImColor(Colors::BACKGROUND_HOVER);

	auto just_opened = ImGui::GetFrameCount() < 4;
	if (!m_open && just_opened && ImGui::GetFrameCount() < 3) {
		auto scale = std::max(GameManager::sharedState()->getIntGameVariable(GameVariable::OVERLAY_SCALE) / 100.0f, 1.0f);
		OverlayManager::get().setScale(scale);
		update_theme();
	}

	if (!m_open && just_opened && ImGui::GetFrameCount() > 2) {
		this->sort_windows(true);
		this->leave_effect(true);
	}

	if (just_opened) {
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);
	}

	auto font = OverlayManager::get().getFont(Overlay::Font::SansSerif);
	ImGui::PushFont(font);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.5f, 0.5f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));

	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, Colors::WINDOW);
	ImGui::PushStyleColor(ImGuiCol_TitleBg, Colors::WINDOW);
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, Colors::WINDOW);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, Colors::BACKGROUND);

	// for the little window collapse button
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Colors::BACKGROUND_HOVER);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, Colors::BACKGROUND_HOVER);

	for (auto& window : m_windows) {
		window.draw();
	}

	ImGui::PopStyleColor(6);
	ImGui::PopStyleVar(5);

	ImGui::PopFont();

	if (just_opened) {
		ImGui::PopStyleVar();
	}

	auto comp = m_update_windows <=> 0;
	if (comp == std::strong_ordering::greater) {
		m_update_windows--;
	} else if (comp == std::strong_ordering::equal) {
		this->sort_windows();
		m_update_windows--;
	}
}

void Menu::sort_windows(bool force) {
	const float pad = 5;
	float x = pad;
	for (size_t i = 0; i < m_windows.size(); ++i) {
		auto& window = m_windows[i];
		auto size = window.size() + ImVec2(pad * 2, pad * 2);
		if (x + size.x > ImGui::GetIO().DisplaySize.x) {
			x = pad;
		}
		float y = pad;
		for (size_t j = 0; j < i; ++j) {
			auto& other_window = m_windows[j];
			const auto other_x = other_window.target_pos().x + other_window.size().x / 2.f;
			// look for other window that is in the same column
			if (other_x >= x && other_x <= x + window.size().x) {
				y = std::max(y, other_window.target_pos().y + other_window.size().y + pad);
			}
		}
		window.set_position({ x + pad, y + pad }, force);
		x += size.x;
	}
}

void Menu::leave_effect(bool instant) {
	for (auto& window : m_windows) {
		auto dir = (rand() / static_cast<float>(RAND_MAX)) * 2 * 3.14159;
		float r = 100.f;
		float x_off = std::cos(dir) * r;
		float y_off = std::sin(dir) * r;
		const auto inside_screen = [&]() {
			const auto rect_intersection = [](const ImVec2& a_min, const ImVec2& a_max, const ImVec2& b_min, const ImVec2& b_max) {
				return a_min.x < b_max.x && a_max.x > b_min.x && a_min.y < b_max.y && a_max.y > b_min.y;
			};
			const auto pos = window.pos() + ImVec2(x_off, y_off);
			const auto size = window.size();
			return rect_intersection(pos, pos + size, ImVec2(0, 0), ImGui::GetIO().DisplaySize);
		};
		// this is an abomination
		while (inside_screen()) {
			r += 100.f;
			x_off = std::cos(dir) * r;
			y_off = std::sin(dir) * r;
		}
		window.save_position();
		window.set_position(window.pos() + ImVec2(x_off, y_off), instant);
	}
	m_leaving = true;
	m_animation_time = instant ? 999.0f : 0.f;
}

void Menu::come_back_effect() {
	m_leaving = false;
	for (auto& window : m_windows) {
		window.undo_animation();
	}
}

void Menu::toggle() {
	m_open = !m_open;
	if (!m_open) {
		this->leave_effect();
	} else {
		for (auto& window : m_windows) {
			window.load();
		}

		this->come_back_effect();
	}
}
