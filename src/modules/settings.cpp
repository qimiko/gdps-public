#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/GameManager.hpp>

#include "base/game_variables.hpp"

#include "classes/settings/settingspopup.hpp"

namespace {
	void add_options_btn(cocos2d::CCLayer* self) {
		auto options_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
		options_sprite->setScale(0.75f);

		auto options_btn = geode::cocos::CCMenuItemExt::createSpriteExtra(options_sprite, [](auto){
			SettingsPopup::create()->show();
		});
		options_btn->setSizeMult(1.1f);

		auto menu = static_cast<cocos2d::CCMenu*>(self->getChildByID("back-menu"));
		if (!menu) {
			return;
		}

		options_btn->setID("options-btn"_spr);
		menu->addChild(options_btn, -1);

		// it's my layout, i can do what i want
		static_cast<geode::RowLayout*>(menu->getLayout())
			->setGap(-10.0f)
			->setAxisReverse(true);
		menu->updateLayout();
	}
}

#ifdef GEODE_IS_WINDOWS

#include <Geode/modify/MoreVideoOptionsLayer.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/modify/CCEGLView.hpp>

void updateFramerate(bool updateVsync = false) {
	auto bypassEnabled = GameManager::sharedState()->getGameVariable(GameVariable::ENABLE_FPS_BYPASS);
	auto currentFps = GameManager::sharedState()->getIntGameVariable(GameVariable::FPS_BYPASS);
	if (bypassEnabled && currentFps > 0) {
		cocos2d::CCApplication::sharedApplication()->toggleVerticalSync(false);
		cocos2d::CCDirector::sharedDirector()->setAnimationInterval(1 / static_cast<double>(currentFps));
	} else if (updateVsync) {
		cocos2d::CCDirector::sharedDirector()->setAnimationInterval(1 / 60.0);
		if (GameManager::sharedState()->getGameVariable(GameVariable::VERTICAL_SYNC)) {
			cocos2d::CCApplication::sharedApplication()->toggleVerticalSync(true);
		}
	}
}

struct SettingsMoreVideoOptionsLayer : geode::Modify<SettingsMoreVideoOptionsLayer, MoreVideoOptionsLayer> {
	struct Fields {
		geode::TextInput* m_fpsInput;
		cocos2d::CCNode* m_applyBtn;
	};

	virtual bool init() override {
		if (!MoreVideoOptionsLayer::init()) {
			return false;
		}

		auto windowSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

		auto bypassEnabled = GameManager::sharedState()->getGameVariable(GameVariable::ENABLE_FPS_BYPASS);

		cocos2d::CCPoint basePosition{windowSize.width / 2 - 135.0f, windowSize.height / 2 + 10.0f};

		auto toggle = GameToolbox::createToggleButton(
			"Unlock FPS",
			static_cast<cocos2d::SEL_MenuHandler>(&SettingsMoreVideoOptionsLayer::onToggleFPS),
			bypassEnabled, this->m_buttonMenu,
			basePosition, this, this->m_mainLayer, 0.8f, 0.5f, 110.0f,
			{8.0f, 0.0f}, "bigFont.fnt", false
		);
		toggle->setSizeMult(1.0f);

		auto infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
		infoSprite->setScale(0.5f);
		auto infoBtn = CCMenuItemSpriteExtra::create(infoSprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&SettingsMoreVideoOptionsLayer::onFPSInfo));
		infoBtn->setSizeMult(2.0f);

		auto togglePos = toggle->getPosition();

		m_buttonMenu->addChild(infoBtn, 1);
		infoBtn->setPosition({togglePos.x - 18.0f, togglePos.y + 16.0f});

		cocos2d::CCPoint inputPosition{windowSize.width / 2 + 69.0f, windowSize.height / 2 + 10.0f};

		auto fpsInput = geode::TextInput::create(90.0f, "FPS");
		fpsInput->setLabel("FPS");
		fpsInput->setCommonFilter(geode::CommonFilter::Uint);
		fpsInput->setMaxCharCount(4);

		auto currentFps = GameManager::sharedState()->getIntGameVariable(GameVariable::FPS_BYPASS);
		if (currentFps > 0) {
			fpsInput->setString(std::to_string(currentFps));
		}

		m_mainLayer->addChild(fpsInput);
		fpsInput->setPosition(inputPosition);

		auto applySprite = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_04.png");
		applySprite->setScale(0.5f);
		auto applyBtn = CCMenuItemSpriteExtra::create(applySprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&SettingsMoreVideoOptionsLayer::onApplyFPS));

		m_buttonMenu->addChild(applyBtn);
		applyBtn->setPosition({47.0f, -17.0f});

		m_fields->m_fpsInput = fpsInput;
		m_fields->m_applyBtn = applyBtn;
		fpsInput->setVisible(bypassEnabled);
		applyBtn->setVisible(bypassEnabled);

		return true;
	}

	void onToggleFPS(cocos2d::CCObject*) {
		GameManager::sharedState()->toggleGameVariable(GameVariable::ENABLE_FPS_BYPASS);

		auto bypassEnabled = GameManager::sharedState()->getGameVariable(GameVariable::ENABLE_FPS_BYPASS);
		m_fields->m_fpsInput->setVisible(bypassEnabled);
		m_fields->m_applyBtn->setVisible(bypassEnabled);

		updateFramerate(true);
	}

	void onToggle(cocos2d::CCObject* sender) {
		MoreVideoOptionsLayer::onToggle(sender);

		updateFramerate(true);
	}

	void onApplyFPS(cocos2d::CCObject*) {
		auto input = m_fields->m_fpsInput->getString();
		auto fpsValue = std::atoi(input.c_str());
		auto currentFps = std::max(fpsValue, 30);

		GameManager::sharedState()->setIntGameVariable(GameVariable::FPS_BYPASS, currentFps);

		m_fields->m_fpsInput->setString(std::to_string(currentFps));

		cocos2d::CCApplication::sharedApplication()->toggleVerticalSync(false);
		cocos2d::CCDirector::sharedDirector()->setAnimationInterval(1 / static_cast<double>(currentFps));
	}

	void onFPSInfo(cocos2d::CCObject*) {
		FLAlertLayer::create(nullptr, "Info", "Lets you set a custom target frames per second. Automatically disables VSync when enabled.", "OK", nullptr)->show();
	}
};

struct SettingsAppDelegate : geode::Modify<SettingsAppDelegate, AppDelegate> {
	virtual bool applicationDidFinishLaunching() override {
		if (!AppDelegate::applicationDidFinishLaunching()) {
			return false;
		}

		updateFramerate();

		return true;
	}

	virtual void applicationWillEnterForeground() override {
		AppDelegate::applicationWillEnterForeground();

		updateFramerate();
	}
};

struct SettingsCCEGLView : geode::Modify<SettingsCCEGLView, cocos2d::CCEGLView> {
	void toggleFullScreen(bool fullscreen) {
		cocos2d::CCEGLView::toggleFullScreen(fullscreen);
		updateFramerate();
	}
};

#endif

struct SettingsLevelInfoLayer : geode::Modify<SettingsLevelInfoLayer, LevelInfoLayer> {
	bool init(GJGameLevel* level) {
		if (LevelInfoLayer::init(level)) {
			add_options_btn(this);

// this functionality is not present on windows and is fully unused, so not worth it
#ifndef GEODE_IS_WINDOWS
			auto gm = GameManager::sharedState();
			bool secret_enabled = gm->getGameVariable(GameVariable::REPLAY_CONTROLS);

			if (secret_enabled) {
				auto replay_sprite = cocos2d::CCSprite::createWithSpriteFrameName(
					"GJ_playEditorBtn_001.png");
				replay_sprite->setScale(0.75f);

				auto replay_btn = CCMenuItemSpriteExtra::create(
					replay_sprite, nullptr, this,
					static_cast<cocos2d::SEL_MenuHandler>(&LevelInfoLayer::onPlayReplay));

				auto menu = cocos2d::CCMenu::createWithItem(replay_btn);
				this->addChild(menu);

				auto director = cocos2d::CCDirector::sharedDirector();

				auto pos_x = director->getScreenLeft() + 110.0f;
				auto pos_y = director->getScreenTop() - 23.0f;

				menu->setPosition(pos_x, pos_y);

				replay_btn->setSizeMult(1.1f);
			}
#endif
			return true;
		}

		return false;
	}
};

struct SettingsEditLevelLayer : geode::Modify<SettingsEditLevelLayer, EditLevelLayer> {
	bool init(GJGameLevel* level) {
		if (!EditLevelLayer::init(level)) {
			return false;
		}

		add_options_btn(this);

		return true;
	}
};

// inlined on Windows, but we're using the native video settings anyways so don't bother
#ifndef GEODE_IS_WINDOWS
struct SettingsMenuLayer : geode::Modify<SettingsMenuLayer, MenuLayer> {
	void openOptions(bool openVideoSettings) {
		if (openVideoSettings) {
			auto ol = OptionsLayer::create();
			this->addChild(ol, 99);
			ol->showLayer(true);

			auto sp = SettingsPopup::create(5);
			this->addChild(sp, 100);
			return;
		} else {
			MenuLayer::openOptions(openVideoSettings);
		}
	}
};
#endif

struct SettingsOptionsLayer : geode::Modify<SettingsOptionsLayer, OptionsLayer> {
	void onOptions(cocos2d::CCObject* /* target */) {
		SettingsPopup::create()->show();
	}
};

struct SettingsGameManager : geode::Modify<SettingsGameManager, GameManager> {
	bool getGameVariable(const char* variable) {
		auto tag = std::string("gv_").append(variable);
		return this->m_valueKeeper->valueForKey(tag)->boolValue();
	}

	int getIntGameVariable(const char* variable) {
		auto tag = std::string("gv_").append(variable);
		return this->m_valueKeeper->valueForKey(tag)->intValue();
	}

	void firstLoad() {
		GameManager::firstLoad();
		this->setGameVariable(GameVariable::KEEP_AWAKE, true);
	}
};
