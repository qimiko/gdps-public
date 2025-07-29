#include "classes/settings/pages/interfacesettingspage.hpp"

void InterfaceSettingsPage::createPage() {
	const float width = this->_dimensions.width;
	const float height = this->_dimensions.height;

	auto title = cocos2d::CCLabelBMFont::create("Interface", "bigFont.fnt");
	this->addChild(title);

	title->setPosition(width / 2, height - 10.0f);
	title->setScale(0.75f);

	auto gm = GameManager::sharedState();

	this->addToggle(
		"Flip 2-Player Controls",
		gm->getGameVariable(GameVariable::FLIP_TWO_PLAYER_CONTROLS),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<10>),
		"Flip which side controls which player during 2-player dual mode.");

	this->addToggle(
		"Always Limit Controls",
		gm->getGameVariable(GameVariable::LIMIT_CONTROLS),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<11>),
		"Player 1 controls are limited to one side even when dual mode is inactive.");

	this->addToggle(
		"Flip pause button",
		gm->getGameVariable(GameVariable::FLIP_PAUSE),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<15>));

	this->addToggle(
		"Restart Button",
		gm->getGameVariable(GameVariable::SHOW_RESTART),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4096>),
		"Enables a persistent restart button in the pause menu.");

	this->addToggle(
		"Accurate Percentage",
		gm->getGameVariable(GameVariable::ACCURATE_PERCENTAGE),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4089>),
		"Shows an extra two digits of precision on the progress bar in a level. Does not apply anywhere else.");

#ifdef GEODE_IS_WINDOWS
	this->addToggle(
		"Show Cursor In-Game",
		gm->getGameVariable(GameVariable::SHOW_CURSOR),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<24>),
		"Controller cursor is visible during gameplay."
	);

	this->addToggle(
		"Hide Practice Buttons",
		gm->getGameVariable(GameVariable::HIDE_PRACTICE_UI),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4081>),
		"Hides the checkpoint buttons shown in practice mode."
	);

	this->addToggle(
		"Disable thumbstick",
		gm->getGameVariable(GameVariable::DISABLE_THUMBSTICK),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<28>),
		"Disable mouse movement using the controller thumb stick."
	);
#endif
}