#include "hooks/ColorSelectPopup.hpp"

#include "classes/remaptriggerpopup.hpp"

void CustomColorSelectPopup::updateDurationAdvanced() {
	if (!this->m_durationLabel) {
		return;
	}

	auto percentage_string = fmt::format("FadeTime: {:.2f}s", this->m_duration);

	this->m_durationLabel->setString(percentage_string.c_str());
}

#ifndef GEODE_IS_WINDOWS
void CustomColorSelectPopup::updateDurLabel() {
	updateDurationAdvanced();
}
#endif

void CustomColorSelectPopup::sliderChanged(cocos2d::CCObject* slider) {
	ColorSelectPopup::sliderChanged(slider);

#ifdef GEODE_IS_WINDOWS
	updateDurationAdvanced();
#endif

	m_fields->m_durationDirty = true;
}

void CustomColorSelectPopup::onRemap(cocos2d::CCObject*) {
	auto popup = RemapTriggerPopup::create(m_targetObject, this);
	// this is dumb
	if (auto layer = cocos2d::CCDirector::sharedDirector()->getRunningScene()->getChildByType<LevelEditorLayer>(0); layer != nullptr) {
		popup->setEditorUI(layer->m_uiLayer);
	}

	popup->show();
}

bool CustomColorSelectPopup::init(GameObject* target, int color_id, int player_id, int blending) {
	if (!ColorSelectPopup::init(target, color_id, player_id, blending)) {
		return false;
	}

	auto ccd = cocos2d::CCDirector::sharedDirector();
	m_fields->m_advancedSelect = nullptr;

	auto adv_widget = AdvancedColorSelectWidget::create(this);
	this->getInternalLayer()->addChild(adv_widget);
	adv_widget->setPosition(ccd->getScreenLeft() + 40.0f, ccd->getScreenTop() - 170.0f);
	adv_widget->setID("rgb-input"_spr);

	auto hex_widget = AdvancedColorSelectWidget::create(this, true);
	this->getInternalLayer()->addChild(hex_widget);
	hex_widget->setPosition(ccd->getScreenRight() - 50.0f, ccd->getScreenTop() - 170.0f);
	hex_widget->setID("hex-input"_spr);

	m_fields->m_advancedSelect = adv_widget;
	m_fields->m_advancedSelect2 = hex_widget;

	if (target != nullptr) {
		auto remap_sprite = ButtonSprite::create("Remap", 40, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 30.0f);
		auto remap_button = CCMenuItemSpriteExtra::create(remap_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&CustomColorSelectPopup::onRemap));

		m_buttonMenu->addChild(remap_button);
		auto remap_position = m_buttonMenu->convertToNodeSpace({ccd->getScreenRight() - 50.0f, ccd->getScreenTop() - 110.0f});
		remap_button->setPosition(remap_position);
	}

#ifdef GEODE_IS_WINDOWS
	updateDurationAdvanced();
#endif

	return true;
}

void CustomColorSelectPopup::colorValueChanged(cocos2d::ccColor3B col) {
	ColorSelectPopup::colorValueChanged(col);

	auto fields = m_fields.self();

	if (fields->m_initFinished) {
		m_colorPicker->m_colourPicker->getSlider()->setVisible(true);
		fields->m_colorDirty = true;
	}

	if (auto color_select = fields->m_advancedSelect) {
		if (!color_select->getIsInUpdate()) {
			color_select->updateColorInputs(true);
		}
	}

	if (auto color_select = fields->m_advancedSelect2) {
		if (!color_select->getIsInUpdate()) {
			color_select->updateColorInputs(true);
		}
	}
}

void CustomColorSelectPopup::show() {
	ColorSelectPopup::show();
	m_fields->m_initFinished = true;
}

void CustomColorSelectPopup::onPlayerColor1(cocos2d::CCObject* callback) {
	ColorSelectPopup::onPlayerColor1(callback);
	m_fields->m_pColDirty = true;
}

void CustomColorSelectPopup::onPlayerColor2(cocos2d::CCObject* callback) {
	ColorSelectPopup::onPlayerColor2(callback);
	m_fields->m_pColDirty = true;
}

void CustomColorSelectPopup::onTintGround(cocos2d::CCObject* callback) {
	ColorSelectPopup::onTintGround(callback);
	m_fields->m_tintGroundDirty = true;
}

void CustomColorSelectPopup::onToggleTintMode(cocos2d::CCObject* callback) {
	ColorSelectPopup::onToggleTintMode(callback);
	m_fields->m_blendDirty = true;
}

void CustomColorSelectPopup::onTouchTriggered(cocos2d::CCObject* callback) {
	ColorSelectPopup::onTouchTriggered(callback);
	m_fields->m_touchTriggedDirty = true;
}
