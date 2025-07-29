#include "classes/gameplaysettingspopup.hpp"

#include "base/game_variables.hpp"
#include "classes/speedhack/speedhackmanagercard.hpp"

void GameplaySettingsPopup::keyBackClicked() {
	this->onClose(nullptr);
};

void GameplaySettingsPopup::onClose(cocos2d::CCObject*) {
	if (
		auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(m_playLayer->getUserObject("hacks"_spr));
		extension_object != nullptr
	) {
		extension_object->updateCheats();
	}

	this->setKeyboardEnabled(false);
	this->removeFromParentAndCleanup(true);
}

cocos2d::CCPoint GameplaySettingsPopup::getNextTogglePoint() {
	return {
		m_centerPoint.x + 35.0f,
		m_centerPoint.y + 215.0f - (this->m_toggleIdx * 45.0f)
	};
}

void GameplaySettingsPopup::onShowPercentage(cocos2d::CCObject*) {
	auto gm = GameManager::sharedState();
	gm->toggleGameVariable(GameVariable::SHOW_PERCENTAGE);

	if (auto percent_label = reinterpret_cast<cocos2d::CCLabelBMFont*>(m_playLayer->getChildByID("percentage-label"_spr)); percent_label != nullptr) {
		percent_label->setVisible(gm->getGameVariable(GameVariable::SHOW_PERCENTAGE));
	}
}

void GameplaySettingsPopup::onPracticeMusic(cocos2d::CCObject*) {
	GameManager::sharedState()->toggleGameVariable(GameVariable::PRACTICE_MUSIC);
}

void GameplaySettingsPopup::onDeathHitboxes(cocos2d::CCObject*) {
	GameManager::sharedState()->toggleGameVariable(GameVariable::SHOW_HITBOXES_ON_DEATH);
}

void GameplaySettingsPopup::onIgnoreDamage(cocos2d::CCObject*) {
	GameManager::sharedState()->toggleGameVariable(GameVariable::IGNORE_DAMAGE);
	this->updateCheatIndicator();
}

void GameplaySettingsPopup::onSpeedhackValueChanged(float interval) {
	this->updateCheatIndicator();
};

void GameplaySettingsPopup::updateCheatIndicator() {
	if (auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(m_playLayer->getUserObject("hacks"_spr)); extension_object != nullptr) {
		auto cheats_enabled = extension_object->determineCheatStatus();
		auto is_cheating = extension_object->getIsIllegitimate();

		if (cheats_enabled) {
			m_cheatIndicator->setColor({0xff, 0x00, 0x00});
			return;
		} else if (is_cheating) {
			// this means cheat status will be reset on next attempt
			m_cheatIndicator->setColor({0xff, 0xaa, 0x00});
			return;
		}
	}

	// all checks passed :)
	m_cheatIndicator->setColor({0x00, 0xff, 0x00});
}

void GameplaySettingsPopup::addToggle(const char* title, bool default_on, cocos2d::SEL_MenuHandler callback, const char* description) {
	auto pos = this->getNextTogglePoint();

	auto toggle = GameToolbox::createToggleButton(
		title, callback, default_on, this->m_buttonMenu, pos,
		this, this->m_mainLayer, 0.8f, 0.5f,
		205.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false
	);
	toggle->setSizeMult(1.0f);

	this->m_toggleIdx++;

	if (description) {
		auto info_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
		info_sprite->setScale(0.5f);
		auto info_btn = CCMenuItemSpriteExtra::create(info_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&GameplaySettingsPopup::onInfoText));
		info_btn->setSizeMult(2.0f);

		auto toggle_pos = toggle->getPosition();

		this->m_buttonMenu->addChild(info_btn);

		info_btn->setPosition(toggle_pos.x - 18.0f, toggle_pos.y + 16.0f);

		auto desc_str = std::string(description);

		auto desc_identifier = static_cast<uint32_t>(std::hash<std::string>{}(desc_str));
		this->m_infoText.insert({ desc_identifier, desc_str });

		info_btn->setTag(desc_identifier);
		info_btn->setSizeMult(2.0f);
	}
}

void GameplaySettingsPopup::onInfoText(cocos2d::CCObject* target) {
	auto n_target = geode::cast::typeinfo_cast<cocos2d::CCNode*>(target);
	if (n_target == nullptr) {
		return;
	}

	auto identifier = n_target->getTag();
	auto desc = m_infoText[identifier];

	FLAlertLayer::create(nullptr, "Info", desc.c_str(), "OK", nullptr, 300.0f)->show();
}

bool GameplaySettingsPopup::init() {
	if (!this->initWithColor({ 0, 0, 0, 75 })) {
		return false;
	}

	m_playLayer = GameManager::sharedState()->m_playLayer;

	constexpr auto width = 400.0f;
	constexpr auto height = 280.0f;

	auto ccd = cocos2d::CCDirector::sharedDirector();
	ccd->getTouchDispatcher()->incrementForcePrio();
	this->setTouchEnabled(true);
	this->setKeypadEnabled(true);

	this->m_controlConnected = -1;
	this->m_scrollAction = -1;

	this->m_mainLayer = cocos2d::CCLayer::create();
	this->addChild(this->m_mainLayer);

	auto window_size = ccd->getWinSize();
	auto bg = cocos2d::extension::CCScale9Sprite::create("square04.png"_spr, { 0.0f, 0.0f, 80.0f, 80.0f });
	bg->setContentSize(cocos2d::CCSize(width, height));
	bg->setPosition(window_size.width / 2, window_size.height / 2);

	this->m_mainLayer->addChild(bg, -2);

	this->m_buttonMenu = cocos2d::CCMenu::create();
	this->m_mainLayer->addChild(this->m_buttonMenu);
	m_buttonMenu->ignoreAnchorPointForPosition(false);

	auto close_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
	auto close_btn = CCMenuItemSpriteExtra::create(
		close_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&GameplaySettingsPopup::onClose)
	);

	this->m_centerPoint.x = (window_size.width - width) / 2;
	this->m_centerPoint.y = (window_size.height - height) / 2;

	this->m_buttonMenu->addChild(close_btn);
	close_btn->setPosition(
		(window_size.width - width) / 2 + 5,
		(window_size.height + height) / 2 - 5
	);

	auto gm = GameManager::sharedState();

	m_cheatIndicator = cocos2d::CCLabelBMFont::create(".", "bigFont.fnt");
	m_mainLayer->addChild(m_cheatIndicator);

	m_cheatIndicator->setPosition(
		(window_size.width + width) / 2 - 15.0f,
		(window_size.height - height) / 2 + 25.0f
	);

	auto card = SpeedhackManagerCard::create();
	m_mainLayer->addChild(card);
	card->setPosition(
		(window_size.width + width) / 2 - 60.0f,
		(window_size.height + height) / 2 - 60.0f
	);
	card->setDelegate(this);
	card->fixPriority();

	this->addToggle(
		"Show Percentage",
		gm->getGameVariable(GameVariable::SHOW_PERCENTAGE),
		static_cast<cocos2d::SEL_MenuHandler>(&GameplaySettingsPopup::onShowPercentage),
		nullptr
	);

	this->addToggle(
		"Practice Music",
		gm->getGameVariable(GameVariable::PRACTICE_MUSIC),
		static_cast<cocos2d::SEL_MenuHandler>(&GameplaySettingsPopup::onPracticeMusic),
		"Uses the song of the current level in practice mode."
	);

	this->addToggle(
		"Safe Noclip",
		gm->getGameVariable(GameVariable::IGNORE_DAMAGE),
		static_cast<cocos2d::SEL_MenuHandler>(&GameplaySettingsPopup::onIgnoreDamage),
		"Prevents the player from dying. Disables level completions."
	);

	this->addToggle(
		"Show Hitboxes on Death",
		gm->getGameVariable(GameVariable::SHOW_HITBOXES_ON_DEATH),
		static_cast<cocos2d::SEL_MenuHandler>(&GameplaySettingsPopup::onDeathHitboxes),
		"Shows exactly what a player died to during gameplay."
	);

/*
	this->addToggle(
		"Show Hitboxes in Practice",
		gm->getGameVariable(GameVariable::SHOW_PRACTICE_HITBOXES),
		nullptr,
		"Shows player hitboxes during practice mode."
	);
*/

	this->updateCheatIndicator();

	return true;
}
