#include <Geode/Geode.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>

#include "classes/speedhack/speedhackmanagercard.hpp"

#include "base/game_variables.hpp"

#include "hooks/EditorUI.hpp"

struct CustomEditorPauseLayer : geode::Modify<CustomEditorPauseLayer, EditorPauseLayer> {
	void onShowHitboxes(cocos2d::CCObject*) {
		GameManager::sharedState()->toggleGameVariable(GameVariable::SHOW_EDITOR_HITBOXES);
	}

	void onShowInfoLabel(cocos2d::CCObject*) {
		GameManager::sharedState()->toggleGameVariable(GameVariable::SHOW_INFO_LABEL);

		static_cast<CustomEditorUI*>(m_levelEditorLayer->m_uiLayer)->scheduleInfoLabelUpdate();
	}

	void customSetup() {
		auto director = cocos2d::CCDirector::sharedDirector();

		EditorPauseLayer::customSetup();

		auto main_menu = reinterpret_cast<cocos2d::CCMenu*>(this->getChildren()->objectAtIndex(1));

		auto show_hitboxes = GameManager::sharedState()->getGameVariable(GameVariable::SHOW_EDITOR_HITBOXES);
		GameToolbox::createToggleButton(
			"Show Hitboxes",
			static_cast<cocos2d::SEL_MenuHandler>(&CustomEditorPauseLayer::onShowHitboxes),
			show_hitboxes, main_menu,
			{ 30.0f, director->getScreenBottom() + 120.0f },
			this, this,
			0.7f, 0.4f, 80.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false);

		GameToolbox::createToggleButton(
			"Show Info Label",
			static_cast<cocos2d::SEL_MenuHandler>(&CustomEditorPauseLayer::onShowInfoLabel),
			GameManager::sharedState()->getGameVariable(GameVariable::SHOW_INFO_LABEL),
			main_menu,
			{30.0f, director->getScreenBottom() + 150.0f},
			this, this, 0.7f, 0.4f, 80.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false
		);

		auto screen_right = director->getScreenRight();
		auto screen_top = director->getScreenTop();

		auto card = SpeedhackManagerCard::create();
		this->addChild(card);
		card->setPosition({screen_right - 50.0f, screen_top - 50.0f});
	}

	std::string formatTimeToString(float seconds) {
		auto levelLength = "Tiny";
		auto intSeconds = static_cast<int>(seconds);

		if (intSeconds >= 120) {
			levelLength = "Extra-Long";
		} else if (intSeconds > 60) {
			levelLength = "Long";
		} else if (intSeconds > 30) {
			levelLength = "Medium";
		} else if (intSeconds > 10) {
			levelLength = "Short";
		}

		auto mins = std::div(intSeconds, 60);
		if (mins.quot == 0) {
			return fmt::format("{:.1f}s ({})", seconds, levelLength);
		}

		auto hours = std::div(mins.quot, 60);
		if (hours.quot == 0) {
			return fmt::format("{}m {}s ({})", mins.quot, mins.rem, levelLength);
		}

		return fmt::format("{}h {}m {}s ({})", hours.quot, hours.rem, mins.rem, levelLength);
	}

	bool init(LevelEditorLayer* editor) {
		if (!EditorPauseLayer::init(editor)) {
			return false;
		}

		auto levelTime = editor->m_gridLayer->timeForXPos(editor->m_levelDistance);

		auto director = cocos2d::CCDirector::sharedDirector();
		auto screen_left = director->getScreenLeft();
		auto screen_top = director->getScreenTop();

		auto timeString = formatTimeToString(levelTime);
		auto timeLabel = cocos2d::CCLabelBMFont::create(timeString.c_str(), "goldFont.fnt");
		timeLabel->setAnchorPoint({0.0f, 1.0f});
		this->addChild(timeLabel);

		timeLabel->setPosition({screen_left + 10.0f, screen_top - 20.0f});
		timeLabel->setScale(0.5f);

		return true;
	}
};
