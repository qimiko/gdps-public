#pragma once

#include "utils.hpp"
#include <string>
#include <vector>
#include <memory>
#include <string_view>
#include <functional>

#include "base/game_variables.hpp"
#include "classes/extensions/playlayerext.hpp"
#include "classes/managers/overlaymanager.hpp"

namespace Colors {
	constexpr auto BACKGROUND = rgb(0x1f2020);
	constexpr auto BACKGROUND_HOVER = rgba(0x00000070);
	constexpr auto TEXT = rgb(0xdfdfd7);
	// constexpr auto PRIMARY = rgb(0xee7762);
	// constexpr auto PRIMARY = rgb(0x5555ee);
	constexpr auto PRIMARY = rgb(0x92ceae);
	// window is slighty darker to help with readability
	constexpr auto WINDOW = rgb(0x91b7a3);
	constexpr auto INACTIVE = rgb(0x525351);
}

struct Theme {
	float width = 200.0f;
	float height = 26.0f;
};
Theme& get_theme();

void update_theme();

class MenuItem {
public:
	MenuItem(std::string label) : label(label) {}
	virtual ~MenuItem() = default;
	std::string label;

	virtual void render() {
		ImGui::Text("%s", label.c_str());
	}

	virtual void load() {}
};

class CheckboxMenuItem : public MenuItem {
public:
	CheckboxMenuItem(std::string label, bool value = false) : MenuItem(label), value(value) {}
	bool value;
	void render() override;
	virtual void pressed() {}
};

class GameVariableMenuItem : public CheckboxMenuItem {
	std::string gv;

public:
	GameVariableMenuItem(std::string label, const char* gv) : CheckboxMenuItem(label, false), gv(gv) { }

	virtual void load() {
		this->value = GameManager::sharedState()->getGameVariable(gv.c_str());
	}

	virtual void pressed() {
		GameManager::sharedState()->setGameVariable(gv.c_str(), this->value);
	}
};

class CBGameVariableMenuItem : public GameVariableMenuItem {
	std::function<void(CBGameVariableMenuItem&)> m_callback;

public:
	CBGameVariableMenuItem(std::string label, const char* gv, std::function<void(CBGameVariableMenuItem&)> callback) : GameVariableMenuItem(label, gv), m_callback(callback) {}

	virtual void pressed() {
		GameVariableMenuItem::pressed();

		m_callback(*this);
	}
};

void updateOverlayHacks();

class HackGameVariableMenuItem : public CBGameVariableMenuItem {
	std::function<void(CBGameVariableMenuItem&)> m_callback;

public:
	HackGameVariableMenuItem(std::string label, const char* gv) : CBGameVariableMenuItem(label, gv, [](CBGameVariableMenuItem&) {
		updateOverlayHacks();
	}) {}
};

class ButtonMenuItem : public MenuItem {
public:
	ButtonMenuItem(std::string label) : MenuItem(label) {}
	void render() override;

	virtual void callback() {};
};

class CBButtonMenuItem : public ButtonMenuItem {
	std::function<void(CBButtonMenuItem&)> m_callback;

public:
  CBButtonMenuItem(std::string label, std::function<void(CBButtonMenuItem&)> callback) : ButtonMenuItem(label), m_callback(callback) {}

	virtual void callback() {
		m_callback(*this);
	}
};

class InputMenuItem : public MenuItem {
protected:
	virtual void render_input();
public:
	InputMenuItem() : MenuItem({}) {}
	void render() override;
};

void updateFPSValue();

class FPSInputMenuItem : public InputMenuItem {
	int m_value = 60;
	void render_input() override;

	virtual void load() override {
		auto bypassValue = GameManager::sharedState()->getIntGameVariable(GameVariable::FPS_BYPASS);
		if (bypassValue != 0) {
			m_value = bypassValue;
		}
	}
};

void updateSpeedhack();

class SpeedInputMenuItem : public InputMenuItem {
	float m_value = 1.25f;
	void render_input() override;

	virtual void load() override {
		m_value = (GameManager::sharedState()->getIntGameVariable(GameVariable::SPEED_INTERVAL) + 1000.0f) / 1000.0f;
	}
};

class BindingMenuItem : public ButtonMenuItem {
public:

	BindingMenuItem() : ButtonMenuItem({}) {}

	virtual void callback() override;
	virtual void load() override {
		label = fmt::format("Open Menu: {}", OverlayManager::getKeybindName());
	}
};

class RowMenuItem : public MenuItem {
	std::unique_ptr<MenuItem> m_left;
	std::unique_ptr<MenuItem> m_right;
public:
	RowMenuItem(std::unique_ptr<MenuItem> left, std::unique_ptr<MenuItem> right) : MenuItem({}),
		m_left(std::move(left)),
		m_right(std::move(right)) {}
	void render() override;

	virtual void load() override {
		m_left->load();
		m_right->load();
	}
};

class Window {
	std::vector<std::unique_ptr<MenuItem>> m_items;
	ImVec2 m_pos;
	ImVec2 m_size;
	std::string m_name;
	bool m_dirty_position = false;
	bool m_open = true;
	// for the moving animation
	ImVec2 m_saved_pos;
	ImVec2 m_start_pos;
	ImVec2 m_target_pos;
	float m_animation_time = 0.0f;
	const float m_animation_duration = 0.3f;
	bool m_animating = false;

public:
	Window(std::string name) : m_name(name) {}
	Window& add(std::unique_ptr<MenuItem> item) {
		m_items.push_back(std::move(item));
		return *this;
	}
	void draw();
	ImVec2 pos() const { return m_pos; }
	ImVec2 target_pos() const { return m_target_pos; }
	ImVec2 size() const { return m_size; }
	void set_position(ImVec2 pos, bool force = false) {
		if (force) {
			m_start_pos = m_target_pos = m_pos = pos;
		} else {
			m_start_pos = m_pos;
			m_target_pos = pos;
			m_animation_time = 0.0f;
			m_animating = true;
		}
		m_dirty_position = true;
	}
	void save_position() {
		if (!m_animating)
			m_saved_pos = m_pos;
	}
	void undo_animation() {
		this->set_position(m_saved_pos);
	}

	void load() {
		for (const auto& item : m_items) {
			item->load();
		}
	}

	std::string_view name() const { return m_name; }
};

class Menu {
	std::vector<Window> m_windows;
	float m_animation_time = 0.0f;
	const float m_animation_duration = 0.3f;
	bool m_leaving = false;
	bool m_open = false;

	bool m_listening = false;
	BindingMenuItem* m_bind_button;
	cocos2d::enumKeyCodes m_open_key = cocos2d::enumKeyCodes::KEY_Tab;

	int m_update_windows = -1;

public:
	static Menu& get() {
		static Menu instance;
		return instance;
	}
	Menu();
	void draw_menu();

	void sort_windows(bool force = false);
	void toggle();
	void leave_effect(bool instant = false);
	void come_back_effect();

	bool is_listening() const {
		return m_listening;
	}

	void set_listening(bool l) {
		m_listening = l;
	}

	void update_bind(cocos2d::enumKeyCodes key) {
		GameManager::sharedState()->setIntGameVariable(GameVariable::OVERLAY_KEYBIND, static_cast<int>(key));
		m_bind_button->load();

		m_open_key = key;
		m_listening = false;
	}

	cocos2d::enumKeyCodes current_bind() const {
		return m_open_key;
	}
};
