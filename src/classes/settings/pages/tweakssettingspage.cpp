#include "classes/settings/pages/tweakssettingspage.hpp"

void TweaksSettingsPage::createPage()  {
	const float width = this->_dimensions.width;
	const float height = this->_dimensions.height;

	auto title = cocos2d::CCLabelBMFont::create("Tweaks", "bigFont.fnt");
	this->addChild(title);

	title->setPosition(width / 2, height - 10.0f);
	title->setScale(0.75f);

	auto gm = GameManager::sharedState();

	this->addToggle(
		"Smooth Fix",
		gm->getGameVariable(GameVariable::SMOOTH_FIX),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<23>),
		"Makes some optimizations that can reduce lag. Disable if game speed becomes inconsistent.");

	this->addToggle(
		"Disable explosion shake",
		gm->getGameVariable(GameVariable::DISABLE_EXPLOSION_SHAKE),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<14>));

	this->addToggle(
		"Unlock All Classic",
		gm->getGameVariable(GameVariable::UNLOCK_ALL),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4097>),
		"Unlocks all built in icons and colors. Will not unlock anything added after U9.");

	this->addToggle(
		"Disable Session Creation",
		gm->getGameVariable(GameVariable::DISABLE_SESSION),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4082>),
		"Disables the automatic creation of a session, an authentication feature which is required for several online features.");

#ifdef GEODE_IS_ANDROID
	this->addToggle(
		"Keep Screen Awake",
		gm->getGameVariable(GameVariable::KEEP_AWAKE),
		static_cast<cocos2d::SEL_MenuHandler>(&TweaksSettingsPage::onToggleKeepAwake),
		"Prevents the device from sleeping during song playback and gameplay. May reduce battery life.");
#endif

	this->addToggle(
			"Free Copy",
			gm->getGameVariable(GameVariable::COPY_HACK),
			static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4087>),
			nullptr);

#ifdef GEODE_IS_WINDOWS
	this->addToggle(
		"Disable Discord Presence",
		gm->getGameVariable(GameVariable::DISABLE_RICH_PRESENCE),
		static_cast<cocos2d::SEL_MenuHandler>(&TweaksSettingsPage::onToggleRichPresence),
		"Disables showing your current ingame status through Discord.");
#endif
}

#ifdef GEODE_IS_WINDOWS
#include "classes/managers/discordmanager.hpp"

void TweaksSettingsPage::onToggleRichPresence(cocos2d::CCObject* target) {
	ToggleSettingsPage::onToggleVariable<4079>(target);

	auto presenceDisabled = GameManager::sharedState()->getGameVariable(GameVariable::DISABLE_RICH_PRESENCE);

	auto& manager = DiscordManager::get();

	// don't reset status if it's the same
	if (!presenceDisabled == manager.status()) {
		return;
	}

	if (presenceDisabled) {
		manager.close();
	} else {
		manager.init();
	}
}
#endif

void TweaksSettingsPage::onToggleKeepAwake(cocos2d::CCObject* target) {
	ToggleSettingsPage::onToggleVariable<4088>(target);

	if (!GameManager::sharedState()->getGameVariable(GameVariable::KEEP_AWAKE)) {
		// disable screen awake when variable turned off
		PlatformHelper::remove_screen_awake();
	}
}