#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/CustomSongLayer.hpp>
#include <Geode/modify/CustomSongWidget.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/InfoLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/ShareLevelLayer.hpp>

#include <random>

#include "base/config.hpp"
#include "base/game_variables.hpp"
#include "classes/commands/rateleveldialog.hpp"

namespace {
	constexpr unsigned int SUPER_VALUE = 184594917;
	constexpr unsigned int HIGH_OBJECT_COUNT = 100'000;
}

struct SuperLevelCell : geode::Modify<SuperLevelCell, LevelCell> {
	void loadCustomLevelCell() {
		LevelCell::loadCustomLevelCell();

		auto level = this->m_level;
		auto featured = level->m_featured;

		if (featured == SUPER_VALUE) {
			auto menu = this->m_mainLayer;

			auto featured_coin = reinterpret_cast<cocos2d::CCSprite*>(
				menu->getChildren()->objectAtIndex(2));

			auto sprite_frame_cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
			auto special_frame = sprite_frame_cache->spriteFrameByName("superCoin.png"_spr);

			featured_coin->setDisplayFrame(special_frame);
		}

		if (level->m_objectCount > HIGH_OBJECT_COUNT) {
			auto nameLabelIdx = level->m_stars > 0 ? 2 : 1;
			auto nameLabel = this->m_mainLayer->getChildByType<cocos2d::CCLabelBMFont>(nameLabelIdx);

			auto badgeX = nameLabel->getPositionX() + (nameLabel->getContentSize().width * nameLabel->getScaleX()) + 12.0f;
			if (level->m_originalLevel != 0) {
				badgeX += 16.0f;
			}

			auto highObjectBadge = cocos2d::CCSprite::createWithSpriteFrameName("highObjectIcon.png"_spr);
			this->m_mainLayer->addChild(highObjectBadge);
			highObjectBadge->setPosition({badgeX, 52.0f});
		}

		return;
	}
};

struct SuperLevelInfoLayer : geode::Modify<SuperLevelInfoLayer, LevelInfoLayer> {
	virtual void FLAlert_Clicked(FLAlertLayer* alert, bool btn) override {
		if (alert->getTag() != 10) {
			return LevelInfoLayer::FLAlert_Clicked(alert, btn);
		}

		this->onCommands(nullptr);
	}

	void updateLabelValues() {
		LevelInfoLayer::updateLabelValues();

		auto level = this->m_level;

		auto featured = level->m_featured;

		if (featured == SUPER_VALUE) {
			auto featured_coin = this->m_featuredCoin;

			auto sprite_frame_cache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
			auto special_frame = sprite_frame_cache->spriteFrameByName("superCoin.png"_spr);

			featured_coin->setDisplayFrame(special_frame);
		}

		return;
	}

	bool init(GJGameLevel* level) {
		if (!LevelInfoLayer::init(level)) {
			return false;
		}

		if (level->m_objectCount > HIGH_OBJECT_COUNT) {
			auto nameLabel = this->getChildByType<cocos2d::CCLabelBMFont>(1);

			auto badgeX = nameLabel->getPositionX() + (0.5f * nameLabel->getContentSize().width * nameLabel->getScaleX()) + 12.0f;
			if (level->m_originalLevel != 0) {
				badgeX += 18.0f;
			}

			auto highObjectBadge = cocos2d::CCSprite::createWithSpriteFrameName("highObjectIcon.png"_spr);
			this->addChild(highObjectBadge);
			highObjectBadge->setPosition({badgeX, nameLabel->getPositionY() - 1.0f});
		}

		auto leftMenu = this->getChildByID("left-side-menu");
		if (leftMenu && GJAccountManager::sharedState()->m_accountID != 0) {
			auto commandsSprite = cocos2d::CCSprite::createWithSpriteFrameName("sendBtn.png"_spr);
			auto commandsBtn = CCMenuItemSpriteExtra::create(
				commandsSprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&SuperLevelInfoLayer::onCommands)
			);

			leftMenu->addChild(commandsBtn);
			leftMenu->updateLayout();
		}

		return true;
	}

	void onCommands(cocos2d::CCObject*) {
		if (GameStatsManager::sharedState()->getStat("6") < 10) {
			auto rateDialog = FLAlertLayer::create(nullptr,
				"Send Level",
				"You must have at least <cy>10 stars</c> to send levels. This ensures all users of the <cg>send system</c> understand the <cl>rate guidelines</c>.",
				"OK", nullptr, 350.0f
			);

			return;
		}

		if (!GameManager::sharedState()->getGameVariable(GameVariable::SHOWN_RATE_DIALOG)) {
			auto rateDialog = FLAlertLayer::create(this,
				"Send Guidelines",
				"All sends are <co>publicly visible</c> in the 1.9 GDPS <cl>Discord server</c>.\n"
				"Please follow the ingame <cy>commenting rules</c> when sending a level.\n"
				"Abuse of the <cg>send system</c> may result in a <cr>permanent send ban</c>.",
				"OK", nullptr, 350.0f
			);
			rateDialog->setTag(10);
			rateDialog->show();

			GameManager::sharedState()->setGameVariable(GameVariable::SHOWN_RATE_DIALOG, true);
			return;
		}

		auto shouldSend = !GameManager::sharedState()->getGameVariable(GameVariable::IS_MODERATOR);
		RateLevelDialog::create(m_level, shouldSend)->show();
	}

	void onLevelInfo(cocos2d::CCObject* target) {
		if (m_level->m_objectCount == 0) {
			LevelInfoLayer::onLevelInfo(target);
			return;
		}

		auto msg = fmt::format(
			"<cy>{}</c>\n"
			"<cg>Total Attempts</c>: {}\n"
			"<cl>Total Jumps</c>: {}\n"
			"<cp>Normal</c>: {}%\n"
			"<co>Practice</c>: {}%\n"
			"<cy>Objects</c>: {}",
			m_level->m_levelName,
			m_level->m_attempts,
			m_level->m_jumps,
			m_level->m_normalPercent,
			m_level->m_practicePercent,
			m_level->m_objectCount
		);
		FLAlertLayer::create(nullptr, "Level Stats", msg.c_str(), "OK", nullptr, 300.0f)->show();
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("LevelInfoLayer::onLevelInfo", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for onLevelInfo");
		}
	}
};

struct ExtraGJGameLevel : geode::Modify<ExtraGJGameLevel, GJGameLevel> {
	static GJGameLevel* create(cocos2d::CCDictionary* dict) {
		auto level = GJGameLevel::create(dict);

		auto objStr = dict->valueForKey("45");
		auto objectCount = objStr->intValue();
		level->m_objectCount = objectCount;

		return level;
	}
};

struct CustomCustomSongWidget : geode::Modify<CustomCustomSongWidget, CustomSongWidget> {
	struct Fields {
		CCMenuItemSpriteExtra* m_deleteBtn;
		CCMenuItemSpriteExtra* m_failureBtn;
	};

	void onDeleteSong(cocos2d::CCObject* /* target */) {
		auto menu = this->m_buttonMenu;
		auto download_btn = this->m_downloadBtn;

		auto song_object = this->m_songObject;
		auto song_id = song_object->m_songID;

		auto music_download_manager = MusicDownloadManager::sharedState();
		music_download_manager->deleteSong(song_id);

		auto delete_button = m_fields->m_deleteBtn;
		delete_button->setVisible(false);

		this->songStateChanged();
	};

	void onFailureInfo(cocos2d::CCObject*) {
		FLAlertLayer::create(
				nullptr,
				"Song Issues",
				"Due to <cl>recent changes</c> to the <cy>Newgrounds</c> and <co>Soundcloud</c> APIs, "
				"some songs are now <cr>unavailable</c>. "
				"Please use the <cg>Song Reupload</c> tool to <cl>reupload new songs</c>.",
				"OK", nullptr, 350.0f)->show();
	}

	void onGetSongInfo(cocos2d::CCObject* target) {
		CustomSongWidget::onGetSongInfo(target);

		auto menu = this->m_buttonMenu;
		auto potential_failure_btn = m_fields->m_failureBtn;
		if (potential_failure_btn != nullptr) {
			potential_failure_btn->setVisible(false);
		}
	}

	void onDownload(cocos2d::CCObject* target) {
		CustomSongWidget::onDownload(target);

		auto menu = this->m_buttonMenu;
		auto potential_failure_btn = m_fields->m_failureBtn;
		if (potential_failure_btn != nullptr) {
			potential_failure_btn->setVisible(false);
		}
	}

	void updateError(GJSongError error) {
		CustomSongWidget::updateError(error);

		if (error == GJSongError::NotFound || error == GJSongError::Unknown) {
			auto menu = this->m_buttonMenu;
			auto potential_failure_btn = m_fields->m_failureBtn;
			if (potential_failure_btn == nullptr) {
				auto failure_sprite = cocos2d::CCSprite::createWithSpriteFrameName(
						"GJ_reportBtn_001.png");
				failure_sprite->setScale(0.7f);

				auto failure_btn = CCMenuItemSpriteExtra::create(
					failure_sprite, nullptr, this,
					static_cast<cocos2d::SEL_MenuHandler>(&CustomCustomSongWidget::onFailureInfo)
				);

				menu->addChild(failure_btn);
				m_fields->m_failureBtn = failure_btn;

				auto cancel_download_btn = this->m_cancelDownloadBtn;

				failure_btn->setPositionX(cancel_download_btn->getPositionX());
				failure_btn->setPositionY(cancel_download_btn->getPositionY() + 40.0f);
				failure_btn->setSizeMult(1.1f);
			} else {
				potential_failure_btn->setVisible(true);
			}
		}
	}

	void updateSongInfo() {
		CustomSongWidget::updateSongInfo();

		auto show_playback_button = m_showPlayMusicBtn;
		if (show_playback_button) {
			return;
		}

		auto song_object = this->m_songObject;
		if (song_object == nullptr) {
			return;
		}

		auto menu = this->m_buttonMenu;

		auto song_id = song_object->m_songID;

		auto music_download_manager = MusicDownloadManager::sharedState();

		auto potential_delete_btn = m_fields->m_deleteBtn;
		if (music_download_manager->isSongDownloaded(song_id)) {
			if (potential_delete_btn != nullptr) {
				potential_delete_btn->setVisible(true);
				return;
			}

			auto delete_sprite = cocos2d::CCSprite::createWithSpriteFrameName(
				"GJ_deleteSongBtn_001.png");
			delete_sprite->setScale(0.9f);

			auto delete_btn = CCMenuItemSpriteExtra::create(delete_sprite, nullptr, this,
				static_cast<cocos2d::SEL_MenuHandler>(
					&CustomCustomSongWidget::onDeleteSong));

			menu->addChild(delete_btn);
			m_fields->m_deleteBtn = delete_btn;

			auto cancel_download_btn = this->m_cancelDownloadBtn;

			delete_btn->setPosition(cancel_download_btn->getPosition());
			delete_btn->setSizeMult(1.1f);
		} else {
			if (potential_delete_btn != nullptr) {
				potential_delete_btn->setVisible(false);
				return;
			}
		}
	}
};

struct CustomCustomSongLayer : geode::Modify<CustomCustomSongLayer, CustomSongLayer> {
	void onFailureInfo(cocos2d::CCObject*) {
		FLAlertLayer::create(
				nullptr,
				"Song Issues",
				"Due to <cl>recent changes</c> to the <cy>Newgrounds</c> and <co>Soundcloud</c> APIs, "
				"some songs are now <cr>unavailable</c>. "
				"Please use the <cg>Song Reupload</c> tool to <cl>reupload new songs</c>.",
				"OK", nullptr, 350.0f)->show();
	}

	void onSongList(cocos2d::CCObject*) {
		cocos2d::CCApplication::sharedApplication()->openURL(GDMOD_ENDPOINT_BASE_URL "/tools/stats/songsList.php");
	}

	void onSongReupload(cocos2d::CCObject*) {
		cocos2d::CCApplication::sharedApplication()->openURL(GDMOD_ENDPOINT_BASE_URL "/tools/songAdd.php");
	}

	bool init(LevelSettingsObject* level) {
		if (CustomSongLayer::init(level)) {
			auto menu = this->m_buttonMenu;

			// hide large go to newgrounds button
			auto orig_button = static_cast<CCMenuItemSpriteExtra*>(menu->getChildren()->objectAtIndex(4));
			orig_button->setVisible(false);

			auto newgrounds_sprite = ButtonSprite::create(
					"Newgrounds",
					220, 0, 0.4f, false,
					"bigFont.fnt", "GJ_button_01.png", 25.0f);

			auto newgrounds_btn = CCMenuItemSpriteExtra::create(
					newgrounds_sprite, nullptr, this,
					static_cast<cocos2d::SEL_MenuHandler>(&CustomCustomSongLayer::onNewgrounds));

			menu->addChild(newgrounds_btn);
			newgrounds_btn->setPositionX(orig_button->getPositionX() - 98.0f);
			newgrounds_btn->setPositionY(orig_button->getPositionY());

			auto list_sprite = ButtonSprite::create(
					"Song List",
					220, 0, 0.4f, false,
					"bigFont.fnt", "GJ_button_02.png", 25.0f);

			auto list_btn = CCMenuItemSpriteExtra::create(
					list_sprite, nullptr, this,
					static_cast<cocos2d::SEL_MenuHandler>(&CustomCustomSongLayer::onSongList));

			menu->addChild(list_btn);
			list_btn->setPositionX(orig_button->getPositionX());
			list_btn->setPositionY(orig_button->getPositionY());

			auto reupload_sprite = ButtonSprite::create(
					"Reupload",
					220, 0, 0.4f, false,
					"bigFont.fnt", "GJ_button_05.png", 25.0f);

			auto reupload_btn = CCMenuItemSpriteExtra::create(
					reupload_sprite, nullptr, this,
					static_cast<cocos2d::SEL_MenuHandler>(&CustomCustomSongLayer::onSongReupload));

			menu->addChild(reupload_btn);
			reupload_btn->setPositionX(orig_button->getPositionX() + 90.0f);
			reupload_btn->setPositionY(orig_button->getPositionY());

			auto failure_sprite = cocos2d::CCSprite::createWithSpriteFrameName(
					"GJ_reportBtn_001.png");
			failure_sprite->setScale(0.7f);

			auto failure_btn = CCMenuItemSpriteExtra::create(
					failure_sprite, nullptr, this,
					static_cast<cocos2d::SEL_MenuHandler>(&CustomCustomSongLayer::onFailureInfo));

			menu->addChild(failure_btn);
			failure_btn->setPositionX(orig_button->getPositionX() - 175.0f);
			failure_btn->setPositionY(orig_button->getPositionY() + 50.0f);

			m_songIDInput->m_maxLabelLength = 9;

			return true;
		}

		return false;
	}
};

struct CustomShareLevelLayer : geode::Modify<CustomShareLevelLayer, ShareLevelLayer> {
	bool init(GJGameLevel* lvl) {
		if (ShareLevelLayer::init(lvl)) {
			auto textbox = geode::cast::typeinfo_cast<TextArea*>(
				this->getInternalLayer()->getChildren()->objectAtIndex(4));

			if (textbox != nullptr) {
				static auto dev = std::random_device();
				static auto rng = std::mt19937(dev());
				static std::uniform_int_distribution<> distribution{1, 100};

				// easter egg
				if (distribution(rng) == 1) {
					textbox->setString("By sharing a level you give the rights to use the level (in any way) and your first born child to the 1.9 GDPS.");
				} else {
					textbox->setString("By sharing a level you give the rights to use the level (in any way) to the creators of the 1.9 GDPS.");
				}
			}

			return true;
		}

		return false;
	}
};

#ifdef GEODE_IS_ANDROID
struct ScaleCCDirector : geode::Modify<ScaleCCDirector, cocos2d::CCDirector> {
	void updateContentScale(cocos2d::TextureQuality n_quality) {
		if (static_cast<int>(n_quality) == 0) {
			// this value is 0 when the game starts
			auto gm = GameManager::sharedState();
			n_quality = gm->getTexQuality();

			if (static_cast<int>(n_quality) == 0) {
				n_quality = cocos2d::TextureQuality::kTextureQualityMedium;
			}
		}

		auto csf = 1.0f;
		if (n_quality == cocos2d::TextureQuality::kTextureQualityLow) {
			csf = 1.0f;
		}

		if (n_quality == cocos2d::TextureQuality::kTextureQualityMedium) {
			csf = 2.0f;
		}

		if (n_quality == cocos2d::TextureQuality::kTextureQualityHigh) {
			csf = 4.0f;
		}

		// todo, recreate float to int algo that rob uses
		this->setContentScaleFactor(csf);
		this->m_eTextureQuality = n_quality;

		return;
	}
};
#endif

struct RefreshInfoLayer : geode::Modify<RefreshInfoLayer, InfoLayer> {
	void onRefresh(cocos2d::CCObject*) {
		auto glm = GameLevelManager::sharedState();

		auto time_left = glm->getTimeLeft("upd_comments", 5.0f);
		if (time_left >= 1) {
			return;
		}
		glm->makeTimeStamp("upd_comments");

		auto key = this->m_commentsKey;
		glm->resetTimerForKey(key.c_str());

		this->loadPage(this->m_page);
	}

	bool init(GJGameLevel* lvl) {
		if (!InfoLayer::init(lvl)) {
			return false;
		}

		auto refresh_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
		refresh_sprite->setScale(0.75f);

		auto refresh_btn = CCMenuItemSpriteExtra::create(
			refresh_sprite, nullptr, this,
			static_cast<cocos2d::SEL_MenuHandler>(&RefreshInfoLayer::onRefresh)
		);

		this->m_buttonMenu->addChild(refresh_btn);
		refresh_btn->setPosition(404.0f, -144.0f);

		return true;
	}
};