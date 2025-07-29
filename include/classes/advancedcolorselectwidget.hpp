#pragma once

#ifndef CLASSES_ADVANCEDCOLORSELECTWIDGET_HPP
#define CLASSES_ADVANCEDCOLORSELECTWIDGET_HPP

#include <Geode/Geode.hpp>
#include <fmt/format.h>

class AdvancedColorSelectWidget : public cocos2d::CCLayer, TextInputDelegate {
public:
	static AdvancedColorSelectWidget* create(ColorSelectPopup* target, bool hex = false);

private:
	ColorSelectPopup* target_{nullptr};
	CCTextInputNode* color_red_{nullptr};
	CCTextInputNode* color_green_{nullptr};
	CCTextInputNode* color_blue_{nullptr};

	bool is_in_update_{false};
	bool hex_{false};

	void updateColors();

	CCTextInputNode* setupInput(const char* label, std::string initial_value, float pos_y, bool hex = true);

	CCTextInputNode* setupInput(const char* label, std::uint8_t initial_value, float pos_y) {
		return setupInput(label, fmt::format("{}", initial_value), pos_y, false);
	}

public:
	bool getIsInUpdate() const;

	void updateColorInputs(bool ignore_changes = false);

	void textChanged(CCTextInputNode*) override;

	bool init(ColorSelectPopup* target, bool hex);
};

#endif //CLASSES_ADVANCEDCOLORSELECTWIDGET_HPP
