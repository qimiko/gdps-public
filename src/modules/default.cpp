#include <chrono>

#include <Geode/Geode.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/SupportLayer.hpp>
#include <Geode/modify/MenuGameLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/utils/web.hpp>

#include "base/game_variables.hpp"
#include "base/platform_helper.hpp"
#include "base/config.hpp"

#include "classes/creditspopup.hpp"
#include "classes/uploadactionpopup.hpp"
#include "classes/mainlevelinfopopup.hpp"
#include "classes/managers/tokenmanager.hpp"
#include "classes/managers/secretmanager.hpp"

#ifdef GEODE_IS_WINDOWS
#include "classes/managers/overlaymanager.hpp"
#endif

using namespace geode::prelude;

namespace {
#ifdef GEODE_IS_ANDROID
	bool IS_GD_INSTALLED = false;
#else
	bool IS_GD_INSTALLED = true;
#endif
}

struct SnowMenuGameLayer : geode::Modify<SnowMenuGameLayer, MenuGameLayer> {
	bool init() {
		if (!MenuGameLayer::init()) {
			return false;
		}

		std::chrono::time_point now{std::chrono::system_clock::now()};
		std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(now)};

		if (ymd.month() == std::chrono::December) {
			auto snow = cocos2d::CCParticleSnow::create();
			this->addChild(snow, 10);
		}

		return true;
	}
};

struct CustomMenuLayer : geode::Modify<CustomMenuLayer, MenuLayer> {
	void onDiscord(cocos2d::CCObject*) {
		GameManager::sharedState()->m_clickedFacebook = true;
		cocos2d::CCApplication::sharedApplication()->openURL("https://absolllute.com/api/1.9?join");
	}

	void onTwitter(cocos2d::CCObject*) {
		GameManager::sharedState()->m_clickedTwitter = true;
		cocos2d::CCApplication::sharedApplication()->openURL("https://absolllute.com/api/1.9");
	}

	void onYouTube(cocos2d::CCObject*) {
		GameManager::sharedState()->m_clickedYouTube = true;
		cocos2d::CCApplication::sharedApplication()->openURL("https://absolllute.com/api/1.9?youtube");
	}

	void onToolsPage(cocos2d::CCObject*) {
		cocos2d::CCApplication::sharedApplication()->openURL("http://absolllute.com/tool");
	}

	void onSpecialThanks(cocos2d::CCObject*) {
		GameManager::sharedState()->reportAchievementWithID("geometry.ach.moreGames", 100, false);

		CreditsPopup::create()->show();
	}

#ifdef GEODE_IS_ANDROID
	void onCreator(cocos2d::CCObject*) {
		auto gm = GameManager::sharedState();

		// messy logic, "caches" check
		if (!IS_GD_INSTALLED && !gm->getGameVariable(GameVariable::BYPASS_PACKAGE_CHECK)) {
			gm->setShowedPirate(true);

			auto err_string = "<cy>Online features</c> have been <cr>disabled</c>.\n\
To enable them, you must <cl>purchase and install</c> the official full release of <cg>Geometry Dash</c> onto your device. \
This allows the GDPS to stay up and also supports the development of the main game.\n\
<co>Thanks</c>!";

			auto msg_box = FLAlertLayer::create(
				this, "Geometry Dash Required!", err_string, "OK :(", "Get it", 400.0);

			msg_box->setTag(2);

			auto app_delegate = AppDelegate::get();
			auto scene_pointer = app_delegate->getScenePointer();

			msg_box->setTargetScene(scene_pointer);
			msg_box->show();
			return;
		}

		if (!gm->getClickedEditor()) {
			gm->setClickedEditor(true);
		}

		// i'm too lazy to figure out how to correctly trampoline, gg
		this->willClose();

		auto director = cocos2d::CCDirector::sharedDirector();

		auto clScene = CreatorLayer::scene();
		auto fadeScene = cocos2d::CCTransitionFade::create(0.5, clScene);

		director->replaceScene(fadeScene);
	}
#endif

	bool init() override {
		if (!MenuLayer::init()) {
			return false;
		}

		auto gm = GameManager::sharedState();

#ifdef GEODE_IS_ANDROID
		if (!IS_GD_INSTALLED) {
			if (!PlatformHelper::is_gd_installed() && !gm->getGameVariable(GameVariable::BYPASS_PACKAGE_CHECK)) {
				auto main_menu = this->getChildByID("main-menu");
				auto create_button = reinterpret_cast<CCMenuItemSpriteExtra*>(
					main_menu->getChildByID("editor-button")
				);

				create_button->setColor({ 0xA6, 0xA6, 0xA6 });
			} else {
				IS_GD_INSTALLED = true;
			}

			CreditsNode::s_disableOnline = !IS_GD_INSTALLED;
		}
#endif

		// perform texture/url replacements
		auto bottom_menu = this->getChildByID("bottom-menu");
		auto play_button = bottom_menu->getChildByID("play-games-button");
		if (play_button != nullptr) {
			play_button->removeFromParent();
			bottom_menu->updateLayout();
		}

		auto social_menu = this->getChildByID("social-media-menu");

		{
			auto robtop_button = reinterpret_cast<CCMenuItemSpriteExtra*>(social_menu->getChildByID("robtop-logo-button"));
			auto robtop_sprite = reinterpret_cast<cocos2d::CCSprite*>(robtop_button->getChildren()->firstObject());

			auto sprite_frame_cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
			auto tools_page_frame = sprite_frame_cache->spriteFrameByName("toolsPageBtnSmall.png"_spr);
			robtop_button->setTarget(this, static_cast<cocos2d::SEL_MenuHandler>(&CustomMenuLayer::onToolsPage));
			robtop_sprite->setDisplayFrame(tools_page_frame);

			robtop_button->setPositionX(robtop_button->getPositionX() + 3.0f);
		}

		{
			auto facebook_button = reinterpret_cast<CCMenuItemSpriteExtra*>(social_menu->getChildByID("facebook-button"));
			auto facebook_sprite = reinterpret_cast<cocos2d::CCSprite*>(facebook_button->getChildren()->firstObject());

			auto sprite_frame_cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
			auto discord_frame = sprite_frame_cache->spriteFrameByName("discordIcon.png"_spr);
			facebook_button->setTarget(this, static_cast<cocos2d::SEL_MenuHandler>(&CustomMenuLayer::onDiscord));
			facebook_sprite->setDisplayFrame(discord_frame);
		}

		{
			auto twitter_button = reinterpret_cast<CCMenuItemSpriteExtra*>(social_menu->getChildByID("twitter-button"));
			twitter_button->setTarget(this, static_cast<cocos2d::SEL_MenuHandler>(&CustomMenuLayer::onTwitter));

			auto youtube_button = reinterpret_cast<CCMenuItemSpriteExtra*>(social_menu->getChildByID("youtube-button"));
			youtube_button->setTarget(this, static_cast<cocos2d::SEL_MenuHandler>(&CustomMenuLayer::onYouTube));
		}

		auto more_games_menu = this->getChildByID("more-games-menu");

		{
			auto more_games_button = reinterpret_cast<CCMenuItemSpriteExtra*>(more_games_menu->getChildByID("more-games-button"));
			auto more_games_sprite = reinterpret_cast<cocos2d::CCSprite*>(more_games_button->getChildren()->firstObject());

			auto sprite_frame_cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
			auto special_frame = sprite_frame_cache->spriteFrameByName("specialThanksBtn.png"_spr);
			more_games_button->setTarget(this, static_cast<cocos2d::SEL_MenuHandler>(&CustomMenuLayer::onSpecialThanks));
			more_games_sprite->setDisplayFrame(special_frame);
		}

		auto demonlistSprite = cocos2d::CCSprite::createWithSpriteFrameName("demonListBtnSmall.png"_spr);
		demonlistSprite->setScale(0.8f);
		auto demonlistBtn = geode::cocos::CCMenuItemExt::createSpriteExtra(
			demonlistSprite, [](auto) {
				cocos2d::CCApplication::sharedApplication()->openURL("https://pointercrate.xyze.dev/");
			}
		);
		social_menu->addChild(demonlistBtn);
		demonlistBtn->setID("demonlist-btn"_spr);
		demonlistBtn->setPosition(3.0f, -28.0f);

		social_menu->setPositionY(social_menu->getPositionY() + 20.0f);

		PlatformHelper::loaded_to_menu();
		return true;
	}
};

struct CustomLoadingLayer : geode::Modify<CustomLoadingLayer, LoadingLayer> {
	const char* getLoadingString() {
		std::time_t t = std::time(nullptr);
#ifdef GEODE_IS_WINDOWS
		// i got tired of the warning
		struct tm tmv{};
		localtime_s(&tmv, &t);

		auto tmp = &tmv;
#else
		auto tmp = localtime(&t);
#endif

		if (tmp != nullptr) {
			// august 13th
			if (tmp->tm_mday == 13 && tmp->tm_mon == 7) {
				return "Happy birthday Geometry Dash!";
			}

			// june 24
			if (tmp->tm_mday == 24 && tmp->tm_mon == 5) {
				return "Happy birthday 1.9 GDPS!";
			}

			// april 1
			if (tmp->tm_mday == 1 && tmp->tm_mon == 3) {
				return "Now with more moving objects";
			}

			if (tmp->tm_wday == 1) {
				// thank you stev
				return "Backup your save data often!";
			}
		}

		srand(t);

		constexpr auto messages = std::to_array({
			"Did you make a session?",
			"Be sure to support the original game!",
			"See the tools page for additional features!",
			"No preview mode here...",
			"Tip: Blending colors goes beneath other colors",
			"Sponsored by beans.com", "Its not over till its over...",
			"Check the options menu for special settings!",
			"Tip: Ensure your 3D lines are consistent",
			"Have you seen my volcano?"
		});

		auto chosen_index = rand() % messages.size();
		return messages[chosen_index];
	}

#ifdef GEODE_IS_WINDOWS
	bool init(bool p1) {
		if (!LoadingLayer::init(p1)) {
			return false;
		}

		auto director = cocos2d::CCDirector::sharedDirector();

		auto overlayText = fmt::format("Press {} to open the overlay!", OverlayManager::getKeybindName());
		auto overlayLabel = cocos2d::CCLabelBMFont::create(overlayText.c_str(), "chatFont.fnt");
		this->addChild(overlayLabel, 2);

		constexpr float baseScale = 0.5f;

		overlayLabel->setAnchorPoint({0.0f, 1.0f});
		overlayLabel->setPosition({ director->getScreenLeft() + 10.0f, director->getScreenTop() - 10.0f });
		overlayLabel->setScale(baseScale);
		overlayLabel->setID("overlay-keybind-label"_spr);

		auto overlayBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0.0f, 0.0f, 80.0f, 80.0f});
		overlayBg->setOpacity(100);
		overlayBg->setColor({0, 0, 0});

		// scale it to prevent weird overlap (this is probably a bad way of doing it, but whatever)

		constexpr float scaleFactor = 4.0f;
		constexpr float padding = 15.0f;
		constexpr float bgScale = baseScale / scaleFactor;

		auto& overlayPosition = overlayLabel->getPosition();
		auto& overlaySize = overlayLabel->getContentSize();
		auto scaledOverlaySize = overlayLabel->getScaledContentSize();

		overlayBg->setContentSize({
			(overlaySize.width + padding) * scaleFactor,
			(overlaySize.height + padding) * scaleFactor
		});

		this->addChild(overlayBg, 1);
		overlayBg->setPosition({
			overlayPosition.x + (scaledOverlaySize.width / 2),
			overlayPosition.y - (scaledOverlaySize.height / 2)
		});
		overlayBg->setScale(bgScale);
		overlayBg->setID("overlay-keybind-bg"_spr);

		return true;
	}
#endif
};

struct CustomLevelPage : geode::Modify<CustomLevelPage, LevelPage> {
	void onInfo(cocos2d::CCObject* target) {
		auto gamelevel = this->m_level;
		if (gamelevel != nullptr) {
			// game uses page -1 for last page
			if (gamelevel->m_levelID == -1) {
				std::array<const char*, 1> messages {
					{
						"<cr>Watch your step</c>, the <co>lasaga</c> approaches...",
					}
				};

				auto chosen_index = rand() % messages.size();

				auto alert = FLAlertLayer::create(nullptr, "You found me..!",
					messages.at(chosen_index), "Thanks", nullptr, 300.0f);
				alert->show();

				return;
			}

			MainLevelInfoPopup::create(gamelevel)->show();
			return;
		}

		LevelPage::onInfo(target);
	}

	bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
		if (m_level->m_levelID == -1) {
			auto gsm = GameStatsManager::sharedState();

			if (m_secretCoin != nullptr) {
				auto gnomeCollected = gsm->hasUniqueItem("gnome02");
				auto gnomeUnlocked = GameManager::sharedState()->getGameVariable(GameVariable::SECRET_COMPLETED) || gnomeCollected;
				auto secretUnlocked = gsm->hasUniqueItem("secret04");

				if (secretUnlocked && gnomeUnlocked && !gnomeCollected) {
					auto touchLoc = this->convertToNodeSpace(touch->getLocation());
					auto touchDistance = cocos2d::ccpDistance(touchLoc, m_secretCoin->getPosition());

					if (touchDistance <= 30.0f) {
						this->setTouchEnabled(false);

						this->playCoinEffect();

						GameSoundManager::sharedManager()->playEffect("highscoreGet02.ogg", 1.0f, 0.0f, 1.0f);

						// shh
						gsm->storeUniqueItem("gnome02");
						gsm->incrementStat("8");
					}

					return true;
				}
			}
		}

		return LevelPage::ccTouchBegan(touch, event);
	}

	struct Fields {
		cocos2d::CCSprite* m_gnomeSprite{nullptr};
	};

	void updateDynamicPage(GJGameLevel* level) {
		LevelPage::updateDynamicPage(level);

		if (m_fields->m_gnomeSprite) {
			m_fields->m_gnomeSprite->setVisible(m_level->m_levelID == -1);
			m_fields->m_gnomeSprite->setZOrder(m_level->m_levelID);

			auto comingSoon = this->getChildByType<cocos2d::CCLabelBMFont>(-1);
			if (comingSoon != nullptr) {
				comingSoon->setVisible(false);
			}
		}
	}
};

struct CustomLevelSelectLayer : geode::Modify<CustomLevelSelectLayer, LevelSelectLayer> {
	struct Fields {
		bool m_createdNodes{false};
	};

	void updatePageWithObject(cocos2d::CCObject* levelPage, cocos2d::CCObject* level) {
		auto page = static_cast<CustomLevelPage*>(levelPage);
		auto levelData = static_cast<GJGameLevel*>(level);

		if (levelData->m_levelID == -1) {
			auto gsm = GameStatsManager::sharedState();
			auto gnomeCollected = gsm->hasUniqueItem("gnome02");
			auto gnomeUnlocked = GameManager::sharedState()->getGameVariable(GameVariable::SECRET_COMPLETED) || gnomeCollected;
			auto secretUnlocked = gsm->hasUniqueItem("secret04");

			// don't show this until we have master detective
			if (!secretUnlocked || !gnomeUnlocked) {
				LevelSelectLayer::updatePageWithObject(levelPage, level);
				return;
			}

			if (m_fields->m_createdNodes) {
				LevelSelectLayer::updatePageWithObject(levelPage, level);
				return;
			}

			m_fields->m_createdNodes = true;

			auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

			auto gnomeSprite = cocos2d::CCSprite::create("gnome.png"_spr);
			page->m_fields->m_gnomeSprite = gnomeSprite;

			page->addChild(gnomeSprite);
			gnomeSprite->setScale(4.0f);

			auto groundPos = m_groundSprite->getPositionY();

			gnomeSprite->setPosition({winSize.width / 2, groundPos - 30.0f});
			gnomeSprite->setAnchorPoint({0.5f, 0.0f});
			gnomeSprite->setID("gnome"_spr);

			if (!gnomeCollected && page->m_secretCoin == nullptr) {
				page->addSecretCoin();
			}
		}

		LevelSelectLayer::updatePageWithObject(levelPage, level);
	}
};

struct CustomSupportLayer : geode::Modify<CustomSupportLayer, SupportLayer> {
	struct Fields : SessionDelegate {
		UploadActionPopup* m_reqPopup{nullptr};
		EventListener<web::WebTask> m_listener{};

		SupportLayer* m_self{nullptr};

		virtual void onSessionCreated() override {
			TokenManager::get().setSessionDelegate(nullptr);

			m_reqPopup->showSuccessMessage("Session token cleared!");
		}

		virtual void onSessionFailed(const char* reason) override {
			TokenManager::get().setSessionDelegate(nullptr);

			m_reqPopup->showFailMessage(reason);
		}
	};

#ifdef GEODE_IS_ANDROID
	void onBypassBtn(cocos2d::CCObject* /* target */) {
		auto gm = GameManager::sharedState();
		gm->toggleGameVariable(GameVariable::BYPASS_PACKAGE_CHECK);
	}
#endif

	void onResetAuthKey(cocos2d::CCObject* /* target */) {
		if (GameToolbox::doWeHaveInternet()) {
			auto& tokenManager = TokenManager::get();
			tokenManager.setSessionDelegate(m_fields.self());

			m_fields->m_reqPopup = UploadActionPopup::create("Resetting token...", false);
			m_fields->m_reqPopup->show();

			TokenManager::get().deleteAuthKey();
		}
	}

	void onRequestSuccess() {
		m_fields->m_reqPopup->showSuccessMessage("Success! Access granted.");
		GameManager::sharedState()->setGameVariable(GameVariable::IS_MODERATOR, true);
	}

	void onRequestFailure() {
		m_fields->m_reqPopup->showFailMessage("Failed. Nothing found.");
		GameManager::sharedState()->setGameVariable(GameVariable::IS_MODERATOR, false);
	}

	void onRequest(cocos2d::CCObject*) {
		m_fields->m_reqPopup = UploadActionPopup::create("Checking...", false);
		m_fields->m_reqPopup->show();

		m_fields->m_listener.bind([this](web::WebTask::Event* e) {
			if (web::WebResponse* res = e->getValue()) {
				auto resultResult = res->string();
				if (!resultResult) {
					this->onRequestFailure();
					return;
				}

				auto resultResultResult = resultResult.unwrap();

				auto resultCode = std::atoi(resultResultResult.c_str());

				if (resultCode >= 1) {
					// access granted!
					this->onRequestSuccess();
				} else {
					this->onRequestFailure();
				}
			} else if (e->isCancelled()) {
				this->onRequestFailure();
			}
		});

		auto req = web::WebRequest();
		auto am = GJAccountManager::sharedState();

		// gjp is required, but not necessary
		auto bodyStr = fmt::format("accountID={}&gjp=uwu&secret=Wmfd2893gb7", am->m_accountID);

		req.bodyString(bodyStr);
		req.userAgent(Config::USER_AGENT);

		m_fields->m_listener.setFilter(req.post(GDMOD_ENDPOINT_BASE_URL "/requestUserAccess.php"));
	}

	void customSetup() override {
		SupportLayer::customSetup();

		m_fields->m_self = this;

		auto internal_layer = this->m_mainLayer;

		auto director = cocos2d::CCDirector::sharedDirector();
		auto win_size = director->getWinSize();

		auto gm = GameManager::sharedState();
		auto current_status = gm->getGameVariable(GameVariable::BYPASS_PACKAGE_CHECK);

		auto point = cocos2d::CCPoint(
			director->getScreenLeft() + (win_size.width / 2.0f) - 150.0f,
			(2 * director->getScreenTop()) - 77.0f);

		auto legal_menu = reinterpret_cast<cocos2d::CCMenu*>(
			internal_layer->getChildren()->objectAtIndex(5));

		// disable restore button
		reinterpret_cast<cocos2d::CCNode*>(
			legal_menu->getChildren()->objectAtIndex(3)
		)->setVisible(false);

#ifdef GEODE_IS_ANDROID
		// bypass button
		this->createToggleButton("",
			static_cast<cocos2d::SEL_MenuHandler>(&CustomSupportLayer::onBypassBtn),
			!current_status, legal_menu, point
		);
#endif

		auto reset_authkey_sprite = ButtonSprite::create(
			"Reset", 220, 0, 0.4f, false, "bigFont.fnt", "GJ_button_03.png", 25.0f);

		auto reset_authkey_button = CCMenuItemSpriteExtra::create(
			reset_authkey_sprite, nullptr, this,
			static_cast<cocos2d::SEL_MenuHandler>(&CustomSupportLayer::onResetAuthKey)
		);

		legal_menu->addChild(reset_authkey_button);

		reset_authkey_button->setPosition({95.0f, -150.0f});

		auto reqSprite = ButtonSprite::create(
			"Req", 220, 0, 0.4f, false, "bigFont.fnt", "GJ_button_04.png", 25.0f
		);

		auto reqButton = CCMenuItemSpriteExtra::create(
			reqSprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&CustomSupportLayer::onRequest)
		);

		legal_menu->addChild(reqButton);

		reqButton->setPosition({150.0f, -150.0f});

		return;
	}
};

#ifdef GEODE_IS_ANDROID
$execute {
	// patches early text field truncation
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1A8B5A),
		{0x1B, 0x46}
	);
}
#endif

#ifdef GEODE_IS_WINDOWS

#include <Geode/modify/CCDirector.hpp>

struct InputCCDirector : geode::Modify<InputCCDirector, cocos2d::CCDirector> {
	void drawScene() {
		this->m_pobOpenGLView->pollEvents();
		CCDirector::drawScene();
	}
};

#endif
