#include "classes/creditspopup.hpp"

void CreditsPopup::keyBackClicked() {
	this->onClose(nullptr);
};

void CreditsPopup::onClose(cocos2d::CCObject*) {
	this->setKeyboardEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void CreditsPopup::onGeode(cocos2d::CCObject*) {
	cocos2d::CCApplication::sharedApplication()->openURL("https://geode-sdk.org/");
}

void CreditsPopup::onRobTop(cocos2d::CCObject*) {
	cocos2d::CCApplication::sharedApplication()->openURL("https://robtopgames.com/");
}

void CreditsPopup::createPoweredBy(cocos2d::CCPoint offset) {
	auto poweredBy = cocos2d::CCSprite::createWithSpriteFrameName("poweredBy_001.png");
	auto geodeLogo = cocos2d::CCSprite::createWithSpriteFrameName("geodeLogo.png"_spr);
	geodeLogo->setScale(0.8f);

	auto geodeBtn = CCMenuItemSpriteExtra::create(
		geodeLogo, nullptr, this,
		static_cast<cocos2d::SEL_MenuHandler>(&CreditsPopup::onGeode)
	);

	m_mainLayer->addChild(poweredBy);
	m_buttonMenu->addChild(geodeBtn);

	poweredBy->setPosition({offset.x, offset.y + 27.0f});
	geodeBtn->setPosition(offset);
}

void CreditsPopup::createOriginalGame(cocos2d::CCPoint offset) {
	auto originalGame = cocos2d::CCSprite::createWithSpriteFrameName("originalGame.png"_spr);
	auto robtop = cocos2d::CCSprite::createWithSpriteFrameName("robtoplogo_small.png");
	robtop->setScale(0.9f);

	auto robtopBtn = CCMenuItemSpriteExtra::create(
		robtop, nullptr, this,
		static_cast<cocos2d::SEL_MenuHandler>(&CreditsPopup::onRobTop)
	);

	m_mainLayer->addChild(originalGame);
	m_buttonMenu->addChild(robtopBtn);

	robtopBtn->setPosition(offset);
	originalGame->setPosition({offset.x, offset.y + 27.0f});
}

bool CreditsPopup::init() {
	if (!this->initWithColor({ 0, 0, 0, 75 })) {
		return false;
	}

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
	auto bg = cocos2d::extension::CCScale9Sprite::create("square05.png"_spr, { 0.0f, 0.0f, 80.0f, 80.0f });
	bg->setContentSize(cocos2d::CCSize(width, height));
	bg->setPosition(window_size.width / 2, window_size.height / 2);

	this->m_mainLayer->addChild(bg, -2);

	this->m_buttonMenu = cocos2d::CCMenu::create();
	this->m_mainLayer->addChild(this->m_buttonMenu);
	m_buttonMenu->ignoreAnchorPointForPosition(false);

	auto close_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
	auto close_btn = CCMenuItemSpriteExtra::create(
					close_sprite,
					nullptr,
					this,
					static_cast<cocos2d::SEL_MenuHandler>(&CreditsPopup::onClose));

	this->m_buttonMenu->addChild(close_btn);
	close_btn->setPosition(
		(window_size.width - width) / 2 + 5,
		(window_size.height + height) / 2 - 5
	);

	auto title = cocos2d::CCLabelBMFont::create("Credits", "goldFont.fnt");
	this->m_mainLayer->addChild(title);

	title->setPosition({window_size.width / 2, (window_size.height + height) / 2 - 20.0f});

	this->createPoweredBy({(window_size.width + width) / 2 - 60.0f, (window_size.height - height) / 2 + 30.0f});
	this->createOriginalGame({(window_size.width - width) / 2 + 60.0f, (window_size.height - height) / 2 + 30.0f});

	auto mainLevelsNode = CreditsNode::create("Main Levels");
	mainLevelsNode->addUser("qjamie", 6153, 47361866);
	mainLevelsNode->addUser("Morpheus", 3107, 47360133);
	mainLevelsNode->addUser("Squared", 9817, 47363831);
	mainLevelsNode->addUser("royen", 18525, 47368948);
	mainLevelsNode->addUser("Aroo", 17414, 47368115);
	mainLevelsNode->addUser("KIKO", 4511, 47360943);
	mainLevelsNode->addUser("zPeton", 19184, 47369412);
	mainLevelsNode->addUser("DreamEater", 26671, 47375212);
	mainLevelsNode->addUser("hsam", 23230, 47372533);
	mainLevelsNode->addUser("Rulls", 27136, 47375572);
	mainLevelsNode->addUser("CelestialCat", 100491, 47407226);
	mainLevelsNode->addUser("Awedsy", 22134, 47371677);

	m_mainLayer->addChild(mainLevelsNode);
	mainLevelsNode->setPosition((window_size.width - width) / 2 + 80.0f, (window_size.height + height) / 2 - 45.0f);

	auto assetsNode = CreditsNode::create("Assets");
	assetsNode->addUser("RobTop", -1, -1);
	assetsNode->addUser("Aroo", 17414, 47368115);
	assetsNode->addUser("RadleyMCT", -1, -1);
	assetsNode->addUser("mat", -1, -1);

	m_mainLayer->addChild(assetsNode);
	assetsNode->setPosition((window_size.width + width) / 2 - 80.0f, (window_size.height + height) / 2 - 45.0f);

	return true;
}