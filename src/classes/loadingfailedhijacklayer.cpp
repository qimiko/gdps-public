#include "classes/loadingfailedhijacklayer.hpp"

bool LoadingFailedHijackLayer::gHasDestroyedState = false;
int LoadingFailedHijackLayer::gReasonCode = -1;

bool LoadingFailedHijackLayer::hasDestroyedState() {
	return gHasDestroyedState;
}

void LoadingFailedHijackLayer::FLAlert_Clicked(FLAlertLayer*, bool) {};

void LoadingFailedHijackLayer::onReset(cocos2d::CCObject*) {
	restoreGameState();

	cocos2d::CCDirector::sharedDirector()->replaceScene(LoadingLayer::scene(false));
}

LoadingFailedHijackLayer* LoadingFailedHijackLayer::create(int code) {
	auto p_ret = new (std::nothrow) LoadingFailedHijackLayer();
	if (p_ret && p_ret->init(code)) {
		p_ret->autorelease();
		return p_ret;
	}
	else {
		delete p_ret;
		p_ret = nullptr;
		return nullptr;
	}
}

bool LoadingFailedHijackLayer::init(int code) {
	if (CCLayer::init()) {
		auto win_size = cocos2d::CCDirector::sharedDirector()->getWinSize();

		auto bg_filename = GameManager::sharedState()->getBGTexture(1);
		auto bg_sprite = cocos2d::CCSprite::create(bg_filename);

		this->addChild(bg_sprite);
		bg_sprite->setPosition(win_size / 2);

		auto bgScale = AppDelegate::get()->bgScale();
		bg_sprite->setScale(bgScale);
		bg_sprite->setColor({ 0, 102, 255 });

		auto message_bg = cocos2d::extension::CCScale9Sprite::create("square02_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
		this->addChild(message_bg);
		message_bg->setOpacity(75);
		message_bg->setContentSize(cocos2d::CCSize(350.0f, 100.0f));
		message_bg->setPosition(win_size / 2);

		auto message = TextArea::create(
			"An <co>unknown problem</c> has been found with your <cl>save data</c>. "
			"The game will <cy>not continue</c> unless you <cr>reset your progress</c>.",
			300.0f,
			0,
			{ 0.5f, 0.5f },
			"chatFont.fnt",
			20.0f
		);
		this->addChild(message);
		message->setPosition( win_size / 2);

		auto actions_menu = cocos2d::CCMenu::create();
		this->addChild(actions_menu);
		actions_menu->setPosition( { win_size.width / 2, (win_size.height / 2) - 75.0f  });

		auto reset_sprite = ButtonSprite::create("Reset");
		auto reset_button = CCMenuItemSpriteExtra::create(
			reset_sprite,
			nullptr,
			this,
			static_cast<cocos2d::SEL_MenuHandler>(&LoadingFailedHijackLayer::onReset)
		);
		actions_menu->addChild(reset_button);

		auto info_message = fmt::format("Helpful number: {}", code);

		auto info_text = cocos2d::CCLabelBMFont::create(info_message.c_str(), "chatFont.fnt");
		this->addChild(info_text);
		info_text->setAnchorPoint({ 1.0f, 0.5f });
		info_text->setPosition({ win_size.width - 25.0f, 25.0f });

		return true;
	}

	return false;
}

void LoadingFailedHijackLayer::destroyGameState() {
	// patch out scene replacement/save
	if (gHasDestroyedState) {
		return;
	}

	// this patch destroys loadAsset for testing
	// HookHandler::get_handler().install_patch(0x1AF2C6, { 0xF0, 0xBD });

	geode::log::warn("destroying game state!");

	gHasDestroyedState = true;
}

void LoadingFailedHijackLayer::restoreGameState() {
	// unpatch scene replacement/save
	// we will probably need to reload the loading scene from here

	if (!gHasDestroyedState) {
		return;
	}

	gHasDestroyedState = false;
}

void LoadingFailedHijackLayer::setCode(int code) {
	gReasonCode = code;
}

cocos2d::CCScene* LoadingFailedHijackLayer::scene() {
	auto scene = cocos2d::CCScene::create();
	scene->addChild(LoadingFailedHijackLayer::create());

	return scene;
}

LoadingFailedHijackLayer* LoadingFailedHijackLayer::create() {
	return LoadingFailedHijackLayer::create(gReasonCode);
}
