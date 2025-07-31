#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/ColorSelectPopup.hpp>

#include "classes/advancedcolorselectwidget.hpp"

struct CustomColorSelectPopup : geode::Modify<CustomColorSelectPopup, ColorSelectPopup> {
	struct Fields {
		AdvancedColorSelectWidget* m_advancedSelect;
		AdvancedColorSelectWidget* m_advancedSelect2;

		bool m_durationDirty{false};
		bool m_colorDirty{false};
		bool m_blendDirty{false};
		bool m_pColDirty{false};
		bool m_tintGroundDirty{false};
		bool m_touchTriggedDirty{false};
		bool m_initFinished{false};

		EditorUI* m_editorUi{nullptr};
	};

	void updateDurationAdvanced();

#ifndef GEODE_IS_WINDOWS
	void updateDurLabel();
#endif

	void sliderChanged(cocos2d::CCObject* slider);

	void onRemap(cocos2d::CCObject*);

	bool init(GameObject* target, int color_id, int player_id, int blending);

	void colorValueChanged(cocos2d::ccColor3B col) override;

	void show() override;

	void onPlayerColor1(cocos2d::CCObject* callback);
	void onPlayerColor2(cocos2d::CCObject* callback);
	void onTintGround(cocos2d::CCObject* callback);
	void onToggleTintMode(cocos2d::CCObject* callback);
	void onTouchTriggered(cocos2d::CCObject* callback);
};
