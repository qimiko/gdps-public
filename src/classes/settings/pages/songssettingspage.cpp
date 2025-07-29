#include "classes/settings/pages/songssettingspage.hpp"

void SongsSettingsPage::createPage() {
	const float width = this->_dimensions.width;
	const float height = this->_dimensions.height;

	auto title = cocos2d::CCLabelBMFont::create("Songs", "bigFont.fnt");
	this->addChild(title);

	title->setPosition(width / 2, height - 10.0f);
	title->setScale(0.75f);

	auto gm = GameManager::sharedState();

	this->addToggle(
		"Load songs to memory",
		gm->getGameVariable(GameVariable::LOAD_SONGS_MEMORY),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<19>),
		"Songs are loaded into memory before playing. Increases load time but can improve performance.");

	this->addToggle(
		"No song limit",
		gm->getGameVariable(GameVariable::NO_SONG_LIMIT),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<18>),
		"Custom songs are no longer auto-removed to save space.");

	this->addToggle(
		"Higher audio quality\n(req. restart)",
		gm->getGameVariable(GameVariable::HIGHER_AUDIO_QUALITY),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<22>),
		"Switches samplerate from 24000 to 44100");

	this->addToggle(
		"Practice music",
		gm->getGameVariable(GameVariable::PRACTICE_MUSIC),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<4098>),
		"Uses the song of the current level in practice mode.");

	this->addSpacer();

#ifdef GEODE_IS_WINDOWS

	this->addToggle(
		"Change custom songs\nlocation",
		gm->getGameVariable(GameVariable::CHANGE_SONG_LOCATION),
		static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onToggleVariable<33>),
		"Songs are saved in a different directory. (Fix if custom songs aren't working)"
	);
#endif

	this->addToggle(
		"Disable menu music",
		gm->getGameVariable(GameVariable::DISABLE_MENU_MUSIC),
		static_cast<cocos2d::SEL_MenuHandler>(&SongsSettingsPage::onToggleMenuMusic));

	auto song_browser_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_savedSongsBtn_001.png");
	auto song_browser_button = CCMenuItemSpriteExtra::create(song_browser_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&SongsSettingsPage::onSongBrowser));

	this->_internal_menu->addChild(song_browser_button);
	this->_menu_objects.push_back(song_browser_button);

	song_browser_button->setPosition(-((_window_dimensions.width / 2) - 20.0f), -(_window_dimensions.height / 2) + 20.0f);

	auto offset_input = CCTextInputNode::create(100.0f, 50.0f, "Offset", "Thonburi", 24, "bigFont.fnt");
	this->addChild(offset_input, 4);
	offset_input->m_delegate = this;
	offset_input->m_maxLabelLength = 4;
	offset_input->setAllowedChars("0123456789-");
	offset_input->setMaxLabelWidth(100.0f);
	offset_input->setMaxLabelScale(0.6f);
	offset_input->setLabelPlaceholderScale(0.6f);
	offset_input->setLabelPlaceholderColor({120, 170, 240 });
	offset_input->setPosition(width - 60.0f, 9.0f);
	this->offset_input_ = offset_input;

	auto offset_label = cocos2d::CCLabelBMFont::create("Music Offset (ms)", "bigFont.fnt");
	this->addChild(offset_label);
	offset_label->setPosition(width - 60.0f, 32.0f);
	offset_label->setScale(0.35f);

	auto fmod_engine = FMODAudioEngine::sharedEngine();
	if (auto time_offset = fmod_engine->m_musicOffset; time_offset != 0) {
		offset_input->setString(fmt::format("{}", time_offset));
	}

	auto label_bg = cocos2d::extension::CCScale9Sprite::create(
			"square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
	label_bg->setContentSize({ 100.0f, 30.0f });
	label_bg->setOpacity(100);
	label_bg->setPosition(width - 60.0f, 9.0f);
	this->addChild(label_bg);
}

void SongsSettingsPage::onClose() {
	if (offset_input_ == nullptr) {
		return;
	}

	// avoiding string based crashes part 2
	auto offset_string = offset_input_->m_textField->getString();
	auto offset = std::atoi(offset_string);

	FMODAudioEngine::sharedEngine()->m_musicOffset = offset;
}

void SongsSettingsPage::onToggleVisibility(bool visible) {
	ToggleSettingsPage::onToggleVisibility(visible);

	// disable textbox if hidden
	offset_input_->setTouchEnabled(visible);
}

void SongsSettingsPage::onSongBrowser(cocos2d::CCObject* /* target */) {
	auto song_browser = GJSongBrowser::create(nullptr);

	auto director = cocos2d::CCDirector::sharedDirector();
	director->getRunningScene()->addChild(song_browser, 106);

	song_browser->showLayer(false);
}

void SongsSettingsPage::onToggleMenuMusic(cocos2d::CCObject*) {
	auto gm = GameManager::sharedState();
	gm->toggleGameVariable(GameVariable::DISABLE_MENU_MUSIC);

	if (gm->getGameVariable(GameVariable::DISABLE_MENU_MUSIC)) {
		GameSoundManager::sharedManager()->stopBackgroundMusic();
	} else {
		GameSoundManager::sharedManager()->m_activeBGMusic = "";
		gm->fadeInMusic("menuLoop.mp3");
	}
}
