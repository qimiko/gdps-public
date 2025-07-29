#include "classes/advancedcolorselectwidget.hpp"

AdvancedColorSelectWidget* AdvancedColorSelectWidget::create(ColorSelectPopup* target, bool hex) {
	auto p_ret = new (std::nothrow) AdvancedColorSelectWidget();
	if (p_ret && p_ret->init(target, hex)) {
		p_ret->autorelease();
		return p_ret;
	}
	else {
		delete p_ret;
		p_ret = nullptr;
		return nullptr;
	}
}

CCTextInputNode* AdvancedColorSelectWidget::setupInput(const char* label, std::string initial_value, float pos_y, bool hex) {
	auto color_input = CCTextInputNode::create(100.0f, 50.0f, label, "Thonburi", 24, "bigFont.fnt");
	this->addChild(color_input, 4);
	color_input->m_delegate = this;

	if (hex) {
		color_input->m_maxLabelLength = 7;
		color_input->setAllowedChars("0123456789abcdefABCDEF");
	} else {
		color_input->m_maxLabelLength = 4;
		color_input->setAllowedChars("0123456789");
	}

	color_input->setMaxLabelWidth(50.0f);
	color_input->setMaxLabelScale(0.6f);
	color_input->setLabelPlaceholderScale(0.6f);
	color_input->setLabelPlaceholderColor({120, 170, 240 });
	color_input->setPosition(0.0f, pos_y);

	auto color_label = cocos2d::CCLabelBMFont::create(label, "bigFont.fnt");
	this->addChild(color_label);

	if (hex) {
		color_label->setPosition(0.0f, pos_y + 25.0f);
	} else {
		color_label->setPosition(35.0f, pos_y);
	}

	color_label->setScale(0.50f);

	color_input->setString(initial_value.c_str());

	auto label_bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));

	if (hex) {
		label_bg->setContentSize({ 60.0f, 30.0f });
	} else {
		label_bg->setContentSize({ 50.0f, 30.0f });
	}
	label_bg->setOpacity(100);
	label_bg->setPosition(0.0f, pos_y);
	this->addChild(label_bg);

	return color_input;
}

bool AdvancedColorSelectWidget::init(ColorSelectPopup* target, bool hex) {
	this->target_ = target;
	this->hex_ = hex;
	auto color = target_->getColorValue();

	// this should be useless as the inputs will be null
	is_in_update_ = true;

	if (hex) {
		this->color_red_ = setupInput("Hex", geode::cocos::cc3bToHexString(color), 0.0f);
	} else {
		this->color_red_ = setupInput("R", color.r, 70.0f);
		this->color_green_ = setupInput("G", color.g, 35.0f);
		this->color_blue_ = setupInput("B", color.g, 0.0f);
	}

	is_in_update_ = false;

	updateColorInputs();

	return true;
}

void AdvancedColorSelectWidget::updateColors() {
	if (hex_) {
		auto value = color_red_->getString();
		auto colString = geode::cocos::cc3bFromHexString(value);
		if (colString) {
			target_->selectColor(colString.unwrap());
		}

		color_red_->setString(geode::utils::string::toUpper(value));

		return;
	}

	auto red_string = color_red_->m_textField->getString();
	auto red = std::atoi(red_string);
	if (red > 255) {
		color_red_->setString("255");
	}

	auto green_string = color_green_->m_textField->getString();
	auto green = std::atoi(green_string);
	if (green > 255) {
		color_green_->setString("255");
	}

	auto blue_string = color_blue_->m_textField->getString();
	auto blue = std::atoi(blue_string);
	if (blue > 255) {
		color_blue_->setString("255");
	}

	// clamp values
	red = std::clamp(red, 0, 255);
	green = std::clamp(green, 0, 255);
	blue = std::clamp(blue, 0, 255);

	target_->selectColor({
		static_cast<uint8_t>(red),
		static_cast<uint8_t>(green),
		static_cast<uint8_t>(blue)
	});
}

void AdvancedColorSelectWidget::updateColorInputs(bool ignore_changes) {
	auto color = target_->getColorValue();

	if (ignore_changes) {
		is_in_update_ = true;
	}

	if (hex_) {
		color_red_->setString(geode::cocos::cc3bToHexString(color));
	} else {
		color_red_->setString(fmt::format("{}", color.r));
		color_green_->setString(fmt::format("{}", color.g));
		color_blue_->setString(fmt::format("{}", color.b));
	}

	if (ignore_changes) {
		is_in_update_ = false;
	}
}

void AdvancedColorSelectWidget::textChanged(CCTextInputNode*) {
	if (hex_ && !color_red_) {
		return;
	}

	if (!hex_ && (!color_red_ || !color_blue_ || !color_green_)) {
		return;
	}

	if (!is_in_update_) {
		is_in_update_ = true;
		updateColors();
		is_in_update_ = false;
	}
}

bool AdvancedColorSelectWidget::getIsInUpdate() const {
	return is_in_update_;
}
