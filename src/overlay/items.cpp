#include "menu.hpp"

#include <iostream>
#include <numbers>
#include <fmt/format.h>
#include <misc/cpp/imgui_stdlib.h>

#include "classes/managers/overlaymanager.hpp"
#include "classes/speedhack/speedhackmanagercard.hpp"

void Window::draw() {
	ImGui::SetNextWindowSize(ImVec2(get_theme().width, -1));
	if (m_dirty_position) {
		if (m_animating) {
			const auto easing = [](float x) {
				return 1.f - std::pow(1.f - x, 3.f);
			};
			float t = easing(m_animation_time / m_animation_duration);
			auto pos = m_start_pos * (1.f - t) + m_target_pos * t;
			ImGui::SetNextWindowPos(pos);
			m_animation_time += ImGui::GetIO().DeltaTime;
			if (m_animation_time >= m_animation_duration) {
				m_animation_time = 0.f;
				m_dirty_position = false;
				m_animating = false;
			}
		} else {
			ImGui::SetNextWindowPos(m_pos);
			m_dirty_position = false;
		}
	} else {
		m_target_pos = m_pos;
	}

	// PushID doesnt seem to work with windows
	if (ImGui::Begin(fmt::format("{}##{}", m_name, static_cast<void*>(this)).c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar)) {
		for (auto& item : m_items) {
			item->render();
		}
	}
	m_open = !ImGui::IsWindowCollapsed();

	m_pos = ImGui::GetWindowPos();
	m_size = ImGui::GetWindowSize();

	ImGui::End();
}

void CheckboxMenuItem::render() {
	auto draw = ImGui::GetWindowDrawList();

	const auto height = get_theme().height;

	if (ImGui::InvisibleButton(this->label.c_str(), ImVec2(-1, height))) {
		this->value = !this->value;
		this->pressed();
	}

	const bool hovering = ImGui::IsItemHovered();
	const auto item_min = ImGui::GetItemRectMin();
	const auto item_max = ImGui::GetItemRectMax();
	const auto item_mid = (item_min + item_max) / 2.f;
	const float width = item_max.x - item_min.x;

	const auto text_size = ImGui::CalcTextSize(label.c_str());

	if (this->value) {
		// TODO: hardcoded colors D:
		draw->AddRectFilled(item_min, item_max, rgba(0x264e3820));
		draw->AddRectFilledMultiColor(ImVec2(item_min.x + width * 0.56f, item_min.y), item_max,
			rgba(0), rgba(0x264e3850),
			rgba(0x264e3850), rgba(0)
		);
	}
	if (hovering) {
		draw->AddRectFilled(item_min, item_max, Colors::BACKGROUND_HOVER);
	}
	draw->AddText(item_min + ImVec2(5, height / 2.f - text_size.y / 2.f), this->value ? Colors::PRIMARY : Colors::TEXT, this->label.c_str());
	draw->AddRectFilled(ImVec2(item_max.x - (6 * OverlayManager::get().scale()), item_min.y + 3), item_max - ImVec2(3, 3), this->value ? Colors::PRIMARY : Colors::INACTIVE);

	if (false) {
		// checkmark
		const auto check_height = height - 6;
		const auto check_origin = ImVec2(item_max.x - check_height - 9, (item_min.y + item_max.y) / 2.f - check_height / 2.f);
		ImVec2 points[] = {
			ImVec2(0.10f, 0.51f) * check_height + check_origin,
			ImVec2(0.40f, 0.80f) * check_height + check_origin,
			ImVec2(0.90f, 0.14f) * check_height + check_origin,
		};
		if (this->value)
			draw->AddPolyline(points, 3, this->value ? Colors::PRIMARY : Colors::INACTIVE, 0, 3.0f);
	}
}

void ButtonMenuItem::render() {
	auto draw = ImGui::GetWindowDrawList();

	if (ImGui::InvisibleButton(this->label.c_str(), ImVec2(-1, get_theme().height))) {
		this->callback();
	}

	const bool hovering = ImGui::IsItemHovered();
	const auto item_min = ImGui::GetItemRectMin();
	const auto item_max = ImGui::GetItemRectMax();

	const auto text_size = ImGui::CalcTextSize(label.c_str());

	if (hovering) {
		draw->AddRectFilled(item_min, item_max, Colors::BACKGROUND_HOVER);
	}
	draw->AddText((item_min + item_max) / 2.0f - text_size / 2.f, Colors::TEXT, this->label.c_str());
}

void InputMenuItem::render() {
	auto draw = ImGui::GetWindowDrawList();

	ImGui::SetNextItemWidth(-1);

	ImGui::PushID(this);
	ImGui::PushStyleColor(ImGuiCol_FrameBg, Colors::BACKGROUND_HOVER);

	this->render_input();

	ImGui::PopStyleColor();
	ImGui::PopID();
}

void InputMenuItem::render_input() {
	static std::string buffer;
	ImGui::InputText("", &buffer);
}

void updateFPSValue() {
#ifdef GEODE_IS_WINDOWS
	auto bypassEnabled = GameManager::sharedState()->getIntGameVariable(GameVariable::ENABLE_FPS_BYPASS);
	auto currentFps = GameManager::sharedState()->getIntGameVariable(GameVariable::FPS_BYPASS);

	if (currentFps == 0) {
		GameManager::sharedState()->setIntGameVariable(GameVariable::FPS_BYPASS, 60);
		currentFps = 60;
	}

	if (bypassEnabled) {
		cocos2d::CCApplication::sharedApplication()->toggleVerticalSync(false);
		cocos2d::CCDirector::sharedDirector()->setAnimationInterval(1 / static_cast<double>(currentFps));
	} else {
		cocos2d::CCDirector::sharedDirector()->setAnimationInterval(1 / 60.0);
		if (GameManager::sharedState()->getGameVariable(GameVariable::VERTICAL_SYNC)) {
			cocos2d::CCApplication::sharedApplication()->toggleVerticalSync(true);
		}
	}
#endif
}

void FPSInputMenuItem::render_input() {
	ImGui::InputScalar("##", ImGuiDataType_U32, &m_value, nullptr, nullptr, "%d FPS", ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_ParseEmptyRefVal);
	if (ImGui::IsItemDeactivated()) {
		if (m_value == 0)
			m_value = 60;

		if (m_value <= 15) {
			m_value = 15;
		}

		GameManager::sharedState()->setIntGameVariable(GameVariable::FPS_BYPASS, m_value);

		updateFPSValue();
	}
}

void updateSpeedhack() {
	auto currentInterval = (GameManager::sharedState()->getIntGameVariable(GameVariable::SPEED_INTERVAL) + 1000.0f) / 1000.0f;
	SpeedhackManagerCard::setSpeedhackValue(currentInterval);

	updateOverlayHacks();
}

void SpeedInputMenuItem::render_input() {
	ImGui::InputScalar("##", ImGuiDataType_Float, &m_value, nullptr, nullptr, "%.2fx", ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_ParseEmptyRefVal);
	if (ImGui::IsItemDeactivated()) {
		if (m_value <= 0)
			m_value = 1.0f;

		auto storedValue = static_cast<int>((m_value * 1000.0f) - 1000.0f);
		GameManager::sharedState()->setIntGameVariable(GameVariable::SPEED_INTERVAL, storedValue);

		updateSpeedhack();
	}
}

void BindingMenuItem::callback() {
	auto& menu = Menu::get();
	if (menu.is_listening()) {
		menu.set_listening(false);
		this->load();

		return;
	}

	menu.set_listening(true);
	this->label = "Listening...";
}

void RowMenuItem::render() {
	ImGui::PushID(this);
	auto avail = ImGui::GetContentRegionAvail();
	if (ImGui::BeginTable("", 2, ImGuiTableFlags_SizingFixedFit)) {
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, avail.x / 2.f);
		ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, avail.x / 2.f);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		m_left->render();
		ImGui::TableSetColumnIndex(1);
		m_right->render();
		ImGui::EndTable();
	}
	ImGui::PopID();
}
