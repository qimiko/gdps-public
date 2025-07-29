#include "classes/settings/pages/editorsettingspage.hpp"

void EditorSettingsPage::createPage() {
	const float width = this->_dimensions.width;
	const float height = this->_dimensions.height;

	auto title = cocos2d::CCLabelBMFont::create("Editor", "bigFont.fnt");
	this->addChild(title);

	title->setPosition(width / 2, height - 10.0f);
	title->setScale(0.75f);

	auto gm = GameManager::sharedState();

	this->addToggle(
		"Increased Max Undo/Redo",
		gm->getGameVariable(GameVariable::INCREASED_UNDO),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<13>),
		"Increases undo limit in the level editor from 200 to 1000.");

	this->addToggle(
		"Small editor step",
		gm->getGameVariable(GameVariable::SMALL_EDITOR_STEP),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<35>),
		"Smallest editor move command uses 1 instead of 2");

/*
	this->addToggle(
		"Persistent Clipboard",
		gm->getGameVariable(GameVariable::PERSISTENT_CLIPBOARD),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4092>),
		"Makes object clipboard transfer between editor sessions.");
*/

	this->addToggle(
		"Show Unused Objects",
		gm->getGameVariable(GameVariable::SHOW_UNUSED_OBJS),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4091>),
		"Adds various unused objects to the editor menus.");

/*
	this->addToggle(
		"Show Advanced Color Picker",
		gm->getGameVariable(GameVariable::ADVANCED_COLOR_SELECT),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4086>),
		"Enables an advanced color picker with RGB color input. Also increases precision in the duration selector."
	);
*/
}