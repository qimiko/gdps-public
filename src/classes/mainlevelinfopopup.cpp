#include "classes/mainlevelinfopopup.hpp"

void MainLevelInfoPopup::keyBackClicked() {
	this->onClose(nullptr);
};

void MainLevelInfoPopup::onClose(cocos2d::CCObject*) {
	this->setKeyboardEnabled(false);
	this->removeFromParentAndCleanup(true);
}

MainLevelInfoPopup* MainLevelInfoPopup::create(GJGameLevel* level) {
	auto pRet = new MainLevelInfoPopup();
	if (pRet && pRet->init(level)) {
		pRet->autorelease();
		return pRet;
	} else {
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

bool MainLevelInfoPopup::canCopyLevel() const {
	auto coinCount = 0;

	auto gsm = GameStatsManager::sharedState();
	for (int i = 1; i <= 3; i++) {
		if (gsm->hasUniqueItem(m_level->getCoinKey(i))) {
			coinCount++;
		}
	}

	return coinCount == 3;
}

void MainLevelInfoPopup::onCopy(cocos2d::CCObject*) {
	if (!canCopyLevel()) {
		FLAlertLayer::create(nullptr, "Clone Level", "You cannot <cl>copy</c> this <cg>level</c> until you complete it with <co>all coins</c>!", "OK", nullptr)->show();
		return;
	}

	auto cloneAlert = FLAlertLayer::create(this, "Clone Level", "Create a <cl>copy</c> of this <cg>level</c>?", "No", "Yes");
	cloneAlert->setTag(1);
	cloneAlert->show();
}

void MainLevelInfoPopup::cloneLevel() {
	// lol.

	auto key = GJSearchObject::create(SearchType::MyLevels)->getKey();
	GameLevelManager::sharedState()->setLastSearchKey(key);

	if (m_level->m_levelString.empty()) {
		auto levelString = LocalLevelManager::sharedState()->getMainLevelString(m_level->m_levelID);
		m_level->m_levelString = levelString;
	}

	EditLevelLayer::create(m_level)->onClone();
}

void MainLevelInfoPopup::FLAlert_Clicked(FLAlertLayer* popup, bool action) {
	if (!action) {
		return;
	}

	if (popup->getTag() == 1) {
		cloneLevel();
	}
}

void MainLevelInfoPopup::addCredits(CreditsLine* creditsNode) {
	if (m_level->m_levelID < 19) {
		creditsNode->addUser("RobTop", -1, -1);
		return;
	}

	switch (m_level->m_levelID) {
		case 19:
			// thumper
			creditsNode->addUser("qjamie", 6153, 47361866);
			break;
		case 20:
			// cf2
			creditsNode->addUser("qjamie", 6153, 47361866);
			creditsNode->addUser("Morpheus", 3107, 47360133);
			break;
		case 21:
			// aura
			creditsNode->addUser("qjamie", 6153, 47361866);
			creditsNode->addUser("Aroo", 17414, 47368115);
			break;
		case 22:
			// jack russel
			creditsNode->addUser("Squared", 9817, 47363831);
			break;
		case 23:
			// street
			creditsNode->addUser("KIKO", 4511, 47360943);
			creditsNode->addUser("zPeton", 19184, 47369412);
			creditsNode->addUser("Aroo", 17414, 47368115);
			creditsNode->addUser("royen", 18525, 47368948);
			creditsNode->addUser("qjamie", 6153, 47361866);
			creditsNode->addUser("DreamEater", 26671, 47375212);
			creditsNode->addUser("hsam", 23230, 47372533);
			creditsNode->addUser("Rulls", 27136, 47375572);
			creditsNode->addUser("CelestialCat", 100491, 47407226);
			creditsNode->addUser("Awedsy", 22134, 47371677);
			break;
	}
}

bool MainLevelInfoPopup::init(GJGameLevel* level) {
	if (!this->initWithColor({ 0, 0, 0, 75 })) {
		return false;
	}

	this->m_level = level;

	auto halfWidth = this->m_level->m_levelID < 19;
	auto width = halfWidth ? 300.0f : 350.0f;
	auto height = halfWidth ? 200.0f : 230.0f;

	auto ccd = cocos2d::CCDirector::sharedDirector();
	ccd->getTouchDispatcher()->incrementForcePrio();
	this->setTouchEnabled(true);
	this->setKeypadEnabled(true);

	this->m_controlConnected = -1;
	this->m_scrollAction = -1;

	this->m_mainLayer = cocos2d::CCLayer::create();
	this->addChild(this->m_mainLayer);

	auto window_size = ccd->getWinSize();
	auto bg = cocos2d::extension::CCScale9Sprite::create("square01_001.png", { 0.0f, 0.0f, 94.0f, 94.0f });
	bg->setContentSize(cocos2d::CCSize(width, height));
	bg->setPosition(window_size.width / 2, window_size.height / 2);

	this->m_mainLayer->addChild(bg, -2);

	this->m_buttonMenu = cocos2d::CCMenu::create();
	this->m_mainLayer->addChild(this->m_buttonMenu);
	m_buttonMenu->ignoreAnchorPointForPosition(false);

	auto close_sprite = ButtonSprite::create("OK");
	auto close_btn = CCMenuItemSpriteExtra::create(
		close_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&MainLevelInfoPopup::onClose)
	);

	this->m_buttonMenu->addChild(close_btn);
	close_btn->setPosition(
		window_size.width / 2,
		(window_size.height - height) / 2 + 30.0f
	);

	auto levelTitle = halfWidth ? "Level Stats" : level->m_levelName.c_str();
	auto title = cocos2d::CCLabelBMFont::create(levelTitle, "goldFont.fnt");
	this->m_mainLayer->addChild(title);

	auto titleOffset = halfWidth ? 30.0f : 25.0f;

	title->setPosition({window_size.width / 2, (window_size.height + height) / 2 - titleOffset});

	if (!halfWidth) {
		title->limitLabelWidth(width - 40.0f, 0.9f, 0.5f);
	} else {
		title->setScale(0.9f);
	}

	auto statsBase = window_size.width / 2;
	auto statsSize = 250.0f;

	if (!halfWidth) {
		auto statsBg = cocos2d::extension::CCScale9Sprite::create("square02_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
		this->m_mainLayer->addChild(statsBg);
		statsBg->setOpacity(75);
		statsBg->setContentSize(cocos2d::CCSize(statsSize + 15.0f, 100.0f));
		statsBg->setPosition({
			statsBase,
			window_size.height / 2
		});
	}

	auto stats = fmt::format("<cg>Total Attempts</c>: {}\n<cl>Total Jumps</c>: {}\n<cp>Normal</c>: {}%\n<co>Practice</c>: {}%", level->m_attempts, level->m_jumps, level->m_normalPercent, level->m_practicePercent);
	if (halfWidth) {
		stats = fmt::format("<cy>{}</c>\n", level->m_levelName) + stats;
	}

	auto statsArea = TextArea::create(stats.c_str(), statsSize, 0, {0.5f, 0.5f}, "chatFont.fnt", 20.0f);
	this->m_mainLayer->addChild(statsArea);
	statsArea->setPosition({
		statsBase,
		window_size.height / 2
	});

	auto copySprite = ButtonSprite::create("Copy Level", 60, 0, 0.6f, true, "goldFont.fnt", "GJ_button_04.png", 20.0f);
	auto copyBtn = CCMenuItemSpriteExtra::create(
		copySprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&MainLevelInfoPopup::onCopy)
	);

	this->m_buttonMenu->addChild(copyBtn);
	copyBtn->setPosition(
		(window_size.width - width) / 2 + 60.0f,
		(window_size.height - height) / 2 + 30.0f
	);

	if (!halfWidth) {
		auto creditsNode = CreditsLine::create();
		creditsNode->setPosition({window_size.width / 2, (window_size.height + height) / 2 - 50.0f});
		creditsNode->setScale(0.6f);

		this->addCredits(creditsNode);
		creditsNode->build();

		m_mainLayer->addChild(creditsNode);
	}

	if (!canCopyLevel()) {
		copySprite->setColor({0xa6, 0xa6, 0xa6});
	}

	return true;
}