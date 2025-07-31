#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/ShareLevelSettingsLayer.hpp>
#include <Geode/modify/ShareLevelLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/UILayer.hpp>

#include <unordered_map>
#include <fmt/format.h>

#include "base/platform_helper.hpp"
#include "base/game_variables.hpp"

#include "classes/gameplaysettingspopup.hpp"
#include "classes/managers/controllermanager.hpp"
#include "classes/managers/secretmanager.hpp"
#include "classes/extensions/playlayerext.hpp"
#include "classes/speedhack/speedhackmanagercard.hpp"
#include "classes/speedhack/speedhacklifecyclewatcher.hpp"

std::unordered_map<std::uint32_t, geode::Patch*> g_patches {};

struct ConditionalBytepatch {
	std::uint32_t rel_addr;
	std::vector<uint8_t> patch_bytes;
	std::vector<uint8_t> orig_bytes;
};

void perform_conditional_patch(const ConditionalBytepatch& patch, bool option) {
	if (!g_patches.contains(patch.rel_addr)) {
		if (!option) {
			return;
		}

		auto res = geode::Mod::get()->patch(
			reinterpret_cast<void*>(geode::base::get() + patch.rel_addr),
			patch.patch_bytes
		);

		if (!res) {
			geode::log::warn("Patch at addr {:#x} failed: {}", patch.rel_addr, res.unwrapErr());
			return;
		}

		g_patches[patch.rel_addr] = res.unwrap();
		return;
	}

	auto created = g_patches[patch.rel_addr];
	if (created->isEnabled() && !option) {
		(void)created->disable();
		return;
	}

	if (!created->isEnabled() && option) {
		(void)created->enable();
		return;
	}
}

bool updatePrevStartPos() {
	auto playLayer = GameManager::sharedState()->m_playLayer;
	auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(playLayer->getUserObject("hacks"_spr));

	auto startPos = extension_object->getStartPositions();
	auto startPosIdx = extension_object->getStartPositionIdx();

	if (startPosIdx == 0) {
		return false;
	} else {
		startPosIdx--;
	}

	if (startPosIdx > 0) {
		auto currentPos = startPos[startPosIdx - 1];
		playLayer->m_startPos = currentPos->getPosition();
		playLayer->m_startPosObject = currentPos;
	} else {
		playLayer->m_startPos = cocos2d::CCPoint(0.0f, 0.0f);
		playLayer->m_startPosObject = nullptr;
	}

	extension_object->setStartPositionIdx(startPosIdx);

	return true;
}

bool updateNextStartPos() {
	auto playLayer = GameManager::sharedState()->m_playLayer;
	auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(playLayer->getUserObject("hacks"_spr));

	auto startPos = extension_object->getStartPositions();
	auto startPosIdx = extension_object->getStartPositionIdx();

	if (startPosIdx == startPos.size()) {
		return false;
	} else {
		startPosIdx++;
	}

	if (startPosIdx > 0) {
		auto currentPos = startPos[startPosIdx - 1];
		playLayer->m_startPos = currentPos->getPosition();
		playLayer->m_startPosObject = currentPos;
	} else {
		playLayer->m_startPos = cocos2d::CCPoint(0.0f, 0.0f);
		playLayer->m_startPosObject = nullptr;
	}

	extension_object->setStartPositionIdx(startPosIdx);

	return true;
}

struct CustomPauseLayer : geode::Modify<CustomPauseLayer, PauseLayer> {
	struct Fields {
		cocos2d::CCLabelBMFont* m_startPosIndicator{nullptr};
		CCMenuItemSpriteExtra* m_startPosPrev{nullptr};
		CCMenuItemSpriteExtra* m_startPosNext{nullptr};

		bool m_hidden{false};
		cocos2d::CCMenu* m_hideMenu{nullptr};
	};

	void onGameplayOptions(cocos2d::CCObject*) {
		GameplaySettingsPopup::create()->show();
	}

	void onHide(cocos2d::CCObject*) {
		auto& fields = m_fields;
		for (auto child : geode::cocos::CCArrayExt<cocos2d::CCNode>(this->getChildren())) {
			if (child != fields->m_hideMenu) {
				child->setVisible(fields->m_hidden);
			}
		}

		if (fields->m_hidden) {
			this->setOpacity(75);
			fields->m_hideMenu->setOpacity(255);
		} else {
			this->setOpacity(0);
			fields->m_hideMenu->setOpacity(75);
		}

		fields->m_hidden = !fields->m_hidden;
	}

	// restart button
	void customSetup() {
		auto gm = GameManager::sharedState();
		bool restart_button_enabled = gm->getGameVariable(GameVariable::SHOW_RESTART);

#if defined(GEODE_IS_ANDROID)
		ConditionalBytepatch patch = {
			0x1ED2F0, // PauseLayer::customSetup
			{ 0x00, 0xBF }, // nop
			{ 0x12, 0xD1 } // ble
		};
#elif defined(GEODE_IS_WINDOWS)
		ConditionalBytepatch patch = {
			0xd64d9, // PauseLayer::customSetup
			{ 0x90, 0x90 }, // nop
			{ 0x75, 0x29 } // jnz
		};
#else
#error Missing patches for PauseLayer::customSetup (restart button)
#endif

		perform_conditional_patch(patch, restart_button_enabled);

		PauseLayer::customSetup();
		auto director = cocos2d::CCDirector::sharedDirector();

		auto options_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
		options_sprite->setScale(0.75f);

		auto options_btn = CCMenuItemSpriteExtra::create(
			options_sprite, nullptr, this,
			static_cast<cocos2d::SEL_MenuHandler>(&CustomPauseLayer::onGameplayOptions)
		);
		options_btn->setSizeMult(1.1f);

		auto menu = this->getChildByID("right-button-menu");

		menu->addChild(options_btn);
		menu->updateLayout();

		options_btn->setID("options-btn"_spr);

		auto hide_sprite = cocos2d::CCSprite::createWithSpriteFrameName("hideBtn.png"_spr);
		hide_sprite->setScale(0.75f);
		hide_sprite->setOpacity(75);

		auto hide_btn = CCMenuItemSpriteExtra::create(hide_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&CustomPauseLayer::onHide));

		auto hide_menu = cocos2d::CCMenu::createWithItem(hide_btn);
		this->addChild(hide_menu);
		hide_menu->setPosition({director->getScreenLeft() + 12.5f, director->getScreenBottom() + 10.0f});

		hide_menu->setID("hide-menu"_spr);
		hide_btn->setID("hide-btn"_spr);

		m_fields->m_hideMenu = hide_menu;

		auto playLayer = GameManager::sharedState()->m_playLayer;
		if (auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(playLayer->getUserObject("hacks"_spr)); extension_object != nullptr) {
			auto startPos = extension_object->getStartPositions();
			auto startPosIdx = extension_object->getStartPositionIdx();

			if (!startPos.empty()) {
				auto actionsMenu = this->getChildByID("center-button-menu");

				auto spriteLeft = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");

				auto btnLeft = CCMenuItemSpriteExtra::create(
					spriteLeft, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&CustomPauseLayer::onPrevStartPos)
				);
				auto leftMostItem = static_cast<cocos2d::CCNode*>(actionsMenu->getChildren()->firstObject());
				auto leftItemPos = leftMostItem->getPosition();

				m_fields->m_startPosPrev = btnLeft;

				auto spriteRight = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
				spriteRight->setFlipX(true);

				auto btnRight = CCMenuItemSpriteExtra::create(
					spriteRight, nullptr, this,
					static_cast<cocos2d::SEL_MenuHandler>(&CustomPauseLayer::onNextStartPos)
				);

				auto rightItem = static_cast<cocos2d::CCNode*>(actionsMenu->getChildren()->lastObject());
				auto rightItemPos = rightItem->getPosition();

				actionsMenu->addChild(btnLeft);
				btnLeft->setPosition({leftItemPos.x - 60.0f, 0.0f});
				btnLeft->setVisible(false);
				btnLeft->setID("start-pos-prev"_spr);

				actionsMenu->addChild(btnRight);
				btnRight->setPosition({rightItemPos.x + 60.0f, 0.0f});
				btnRight->setVisible(false);
				btnRight->setID("start-pos-next"_spr);

				m_fields->m_startPosNext = btnRight;

				auto startPosLabel = cocos2d::CCLabelBMFont::create("", "bigFont.fnt");

				auto posX = director->getScreenLeft() + 30.0f;
				auto posY = director->getScreenTop() - 40.0f;

				this->addChild(startPosLabel, 2);
				startPosLabel->setScale(0.75f);
				startPosLabel->setPosition({posX, posY});
				startPosLabel->setAnchorPoint({0.0f, 0.5f});
				startPosLabel->setID("start-pos-label"_spr);
				startPosLabel->setOpacity(0);

				m_fields->m_startPosIndicator = startPosLabel;

				updateStartPosIndicators();
			}
		}
	}

	void onFullRestart() {
		auto playLayer = GameManager::sharedState()->m_playLayer;

		while (playLayer->m_checkpoints->count() > 0) {
			playLayer->removeLastCheckpoint();
		}

		// force audio to remain paused
		auto fmod = FMODAudioEngine::sharedEngine();
		auto volume = fmod->m_backgroundMusicVolume;
		fmod->m_backgroundMusicVolume = 0.0f;

		this->onRestart(nullptr);

		fmod->m_backgroundMusicVolume = volume;

		playLayer->pauseGame(false);
	}

	void onPrevStartPos(cocos2d::CCObject*) {
		updatePrevStartPos();

		this->updateStartPosIndicators();

		this->onFullRestart();
	}

	void onNextStartPos(cocos2d::CCObject*) {
		updateNextStartPos();

		this->updateStartPosIndicators();

		this->onFullRestart();
	}

	void updateStartPosIndicators() {
		auto& fields = m_fields;

		auto playLayer = GameManager::sharedState()->m_playLayer;
		auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(playLayer->getUserObject("hacks"_spr));

		auto startPos = extension_object->getStartPositions();
		auto startPosIdx = extension_object->getStartPositionIdx();

		fields->m_startPosPrev->setVisible(startPosIdx != 0);
		fields->m_startPosNext->setVisible(startPosIdx != startPos.size());

		auto startPosText = fmt::format("({}/{})", startPosIdx, startPos.size());
		fields->m_startPosIndicator->setString(startPosText.c_str());

		fields->m_startPosIndicator->setScale(0.75f);
		fields->m_startPosIndicator->limitLabelWidth(75.0f, 0.75f, 0.25f);
	}

	void onProgressBar(cocos2d::CCObject* target) {
		PauseLayer::onProgressBar(target);

		auto gm = GameManager::sharedState();

		if (auto pl = gm->m_playLayer; pl != nullptr) {
			if (auto percent_label = reinterpret_cast<cocos2d::CCLabelBMFont*>(pl->getChildByID("percentage-label"_spr)); percent_label != nullptr) {
				auto director = cocos2d::CCDirector::sharedDirector();
				auto w_size = director->getWinSize();

				if (gm->m_showProgressBar) {
					percent_label->setPositionX((w_size.width / 2) + 110.0f);
					percent_label->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
				} else {
					percent_label->setPositionX(w_size.width / 2);
					percent_label->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.0f));
				}
			}
		}
	}
};

struct CustomPlayLayer : geode::Modify<CustomPlayLayer, PlayLayer> {
	struct Fields {
		int m_smoothFrames{0};
	};

	// practice music
	void togglePracticeMode(bool change) {
		auto gm = GameManager::sharedState();
		bool practice_music_enabled = gm->getGameVariable(GameVariable::PRACTICE_MUSIC);

#if defined(GEODE_IS_ANDROID)
		const std::vector<ConditionalBytepatch> patches {
			{
				0x1B9050, // PlayLayer::togglePracticeMode
				{ 0x03, 0xE0 }, // nop
				{ 0x1D, 0xB1 } // cbz
			},
			{
				0x1B8EBA, // PlayLayer::resetLevel
				{ 0x00, 0xBF }, // nop
				{ 0x79, 0xD1 } // bne
			},
			{
				0x1B60C6, // PlayLayer::destroyPlayer
				{ 0x00, 0xBF }, // nop
				{ 0x93, 0xB9 } // cbnz
			},
			{
				0x1B60F2, // PlayLayer::destroyPlayer
				{ 0x00, 0xBF }, // nop
				{ 0x1B, 0xB9 } // cbnz
			},
			{
				0x1B6498, // PlayLayer::updateVisibility
				{ 0x1B, 0x45 }, // cmp r3, r3
				{ 0x00, 0x2B } // cmp r3, #0x0
			},
			{
				0x1B7666, // PlayLayer::resume
				{ 0x00, 0xbf }, // nop
				{ 0x44, 0xd1 } // bne
			},
			{
				0x1b757e, // PlayLayer::pauseGame
				{ 0x04, 0xe0 }, // b
				{ 0x23, 0xb1 } // cbz
			}
		};
#elif defined(GEODE_IS_WINDOWS)
		const std::vector<ConditionalBytepatch> patches {
			{
				0xf3663, // PlayLayer::togglePracticeMode
				{ 0x90, 0x90 }, // nop
				{ 0x75, 0x41 } // jnz
			},
			{
				0xf284f, // PlayLayer::resetLevel
				{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, // nop
				{ 0x0f, 0x85, 0x4d, 0x07, 0x00, 0x00 } // jnz
			},
			{
				0xf0699, // PlayLayer::destroyPlayer
				{ 0x90, 0x90 }, // nop
				{ 0x75, 0x3c } // jnz
			},
			{
				0xf06cb, // PlayLayer::destroyPlayer
				{ 0x90, 0x90 }, // nop
				{ 0x75, 0x0c } // jnz
			},
			{
				0xeb441, // PlayLayer::updateVisibility
				{ 0xeb, 0x16 }, // jmp
				{ 0x74, 0x16 } // jz
			},
			{
				0xf3a96, // PlayLayer::resume
				{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, // nop
				{ 0x0f, 0x85, 0xb5, 0x00, 0x00, 0x00 } // jnz
			},
			{
				0xf3943, // PlayLayer::pauseGame
				{ 0xeb, 0x11 }, // jmp
				{ 0x74, 0x11 } // jnz
			}
		};
#else
#error Missing patches for PlayLayer::togglePracticeMode (practice music)
#endif

		for (auto& patch : patches) {
			perform_conditional_patch(patch, practice_music_enabled && change);
		}

		if (practice_music_enabled && change && !this->m_practiceMode) {
			this->m_practiceMode = change;

#ifdef GEODE_IS_WINDOWS
			if (gm->getGameVariable(GameVariable::HIDE_PRACTICE_UI)) {
				change = false;
			}
#endif

			this->m_uiLayer->toggleCheckpointsMenu(change);

			this->stopActionByTag(18);
			return;
		}

		PlayLayer::togglePracticeMode(change);
	}

	// noclip
	void destroyPlayer(PlayerObject* player) {
		auto gm = GameManager::sharedState();
		bool noclip_enabled = gm->getGameVariable(GameVariable::IGNORE_DAMAGE);

		// yes this disables special noclip
		if (noclip_enabled)
			return;

		auto illegitimate_run = false;
		if (auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(this->getUserObject("hacks"_spr)); extension_object != nullptr) {
			illegitimate_run = extension_object->getIsIllegitimate() || extension_object->getIsFalseCompletion();

			// show hitboxes if enabled
			auto hitboxes_enabled = GameManager::sharedState()->getGameVariable(GameVariable::SHOW_HITBOXES_ON_DEATH);
			if (hitboxes_enabled) {
				auto hitboxes = extension_object->getHitboxLayer();

				hitboxes->setVisible(true);
				hitboxes->beginUpdate();

				// just assume this is the leftmost point
				auto camera = this->m_cameraPos;

				auto width = cocos2d::CCDirector::sharedDirector()->getWinSize().width;

				auto beginSection = static_cast<int>(std::floor(camera.x / 100.0f));
				auto endSection = static_cast<int>(std::ceil((camera.x + width) / 100.0f));

				auto lvlSections = this->m_levelSections;
				auto sectionCount = lvlSections->count();

				beginSection = std::max(beginSection, 0);
				endSection = std::min(endSection, static_cast<int>(sectionCount - 1));

				for (auto i = beginSection; i <= endSection; i++) {
					auto sectionObjs = reinterpret_cast<cocos2d::CCArray*>(lvlSections->objectAtIndex(i));
					auto objCount = sectionObjs->count();
					for (auto j = 0u; j < objCount; j++) {
						auto obj = reinterpret_cast<GameObject*>(sectionObjs->objectAtIndex(j));
						hitboxes->drawObject(obj, -1);
					}
				}

				hitboxes->drawPlayer(this->m_player);
				if (this->m_dualMode) {
					hitboxes->drawPlayer(this->m_player2);
				}
			}
		}

		auto was_testmode = this->m_testMode;

		this->m_testMode = was_testmode || illegitimate_run;
		PlayLayer::destroyPlayer(player);

		// we don't have to care about new best, at least
		bool fast_reset_enabled = gm->getGameVariable(GameVariable::FAST_PRACTICE_RESET);
		if (this->m_practiceMode && this->m_resetQueued && fast_reset_enabled) {
			this->stopActionByTag(16);
			auto resetSequence = cocos2d::CCSequence::createWithTwoActions(
				cocos2d::CCDelayTime::create(0.5f),
				cocos2d::CCCallFunc::create(this, static_cast<cocos2d::SEL_CallFunc>(&PlayLayer::delayedResetLevel))
			);
			resetSequence->setTag(16);
			this->runAction(resetSequence);
		}

		this->m_testMode = was_testmode;
	}

	void showSecretAchievement() {
		auto achievement = AchievementBar::create("Congratulations!", "The Gnome has been unlocked.", nullptr);
		auto gnomeSprite = cocos2d::CCSprite::create("gnome.png"_spr);

		auto iconSprite = achievement->m_layerColor->getChildByType<cocos2d::CCSprite>(0);
		achievement->m_layerColor->addChild(gnomeSprite);

		if (iconSprite) {
			gnomeSprite->setPosition(iconSprite->getPosition());
		}

		gnomeSprite->setZOrder(4);
		gnomeSprite->setScale(0.75f);

		auto an = AchievementNotifier::sharedState();
		an->m_achievementBarArray->addObject(achievement);
		if (an->m_activeAchievementBar == nullptr) {
			an->showNextAchievement();
		}
	}

	// restart button
	void levelComplete() {
		bool ignore_completion = false;

		if (auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(this->getUserObject("hacks"_spr)); extension_object != nullptr) {
			if (extension_object->getIsIllegitimate() || extension_object->getIsFalseCompletion()) {
				geode::log::info("legitimacy tripped, exiting level");
				ignore_completion = true;
			}
		}

		auto was_testmode = this->m_testMode;

		if (!ignore_completion && !was_testmode && !m_practiceMode && m_level->m_levelType == GJLevelType::Local) {
			auto& secretManager = SecretManager::get();
			secretManager.completeLevel(m_level->m_levelID);

			auto gnomeCollected = GameStatsManager::sharedState()->hasUniqueItem("gnome02");
			auto gnomeUnlocked = GameManager::sharedState()->getGameVariable(GameVariable::SECRET_COMPLETED) || gnomeCollected;
			if (secretManager.finishedRun() && !gnomeUnlocked) {
				showSecretAchievement();
				GameManager::sharedState()->setGameVariable(GameVariable::SECRET_COMPLETED, true);
			}
		}

		this->m_testMode = was_testmode || ignore_completion;
		PlayLayer::levelComplete();

		this->m_testMode = was_testmode;
	}

	void updatePercentageLabel() {
		auto player = this->m_player;
		auto max_level_size = this->m_levelLength;

		auto percentage = std::min((player->getPositionX() / max_level_size) * 100.0f, 100.0f);

		auto label = reinterpret_cast<cocos2d::CCLabelBMFont*>(this->getChildByID("percentage-label"_spr));
		if (label != nullptr) {
			// todo, this probably should be optimized since this reduces fps
			auto accurate_percentage = GameManager::sharedState()->getGameVariable(GameVariable::ACCURATE_PERCENTAGE);
			if (accurate_percentage) {
				auto percentage_string = fmt::format("{:.2f}%", percentage);
				label->setString(percentage_string.c_str(), true);
			} else {
				auto percentage_string = fmt::format("{}%", static_cast<int>(percentage));
				label->setString(percentage_string.c_str(), true);
			}
		}
	}

	bool init(GJGameLevel* level) {
		if (!PlayLayer::init(level)) {
			return false;
		}

		auto gm = GameManager::sharedState();

#ifdef GEODE_IS_ANDROID
		if (!gm->getGameVariable(GameVariable::SHOW_CURSOR)) {
			if (PlatformToolbox::isControllerConnected()) {
				ControllerManager::getManager().hideCursor();
			}
		}
#endif

		bool secret_enabled = gm->getGameVariable(GameVariable::REPLAY_CONTROLS);

		auto extension_object = new PlayLayerExt();

		if (secret_enabled) {
			this->m_recordActions = true;
			geode::log::info("recording feature force enabled");
		}

		// bug in pre 1.93, would allow for completion of deleted levels
		auto lvl_objs = this->m_levelSections;
		uint32_t obj_count = 0;
		for (uint32_t i = 0; i < lvl_objs->count(); i++) {
			obj_count += reinterpret_cast<cocos2d::CCArray*>(lvl_objs->objectAtIndex(i))->count();
			// don't count all the objects in the level if we know there's enough to get past the check
			if (obj_count > 1) {
				break;
			}
		}

		if (this->m_testMode) {
			// determine the start positions in the level. we know there's at least one
			for (auto i = 0u; i < lvl_objs->count(); i++) {
				auto sectionObjs = static_cast<cocos2d::CCArray*>(lvl_objs->objectAtIndex(i));
				for (auto j = 0u; j < sectionObjs->count(); j++) {
					auto obj = static_cast<GameObject*>(sectionObjs->objectAtIndex(j));
					if (obj->m_objectID == 31) {
						auto startPos = static_cast<StartPosObject*>(obj);
						extension_object->insertStartPosition(startPos);
						// should this vector be sorted after?
					}
				}
			}

			// the player automatically starts at the last start position
			extension_object->setStartPositionIdx(extension_object->getStartPositions().size());
		}

		// end portal counts as an object, don't count it here obviously
		if (obj_count <= 1 && this->m_level->m_stars >= 1) {
			extension_object->setIsFalseCompletion(true);

			auto warning_text = cocos2d::CCLabelBMFont::create("I see you :)", "bigFont.fnt");
			this->m_gameLayer->addChild(warning_text, 4);
			warning_text->setPosition(700.0f, 240.0f);
		}

		if (level->m_audioTrack >= 18) {
			GameSoundManager::sharedManager()->enableMetering();
			this->m_meteringEnabled = true;
		}

		auto director = cocos2d::CCDirector::sharedDirector();
		auto w_size = director->getWinSize();

		auto percentage_precision = GameManager::sharedState()->getGameVariable(GameVariable::ACCURATE_PERCENTAGE) ? 2 : 0;
		auto percentage_string = fmt::format("{:.{}f}%", 0.0f, percentage_precision);

		auto percentage_label = cocos2d::CCLabelBMFont::create(percentage_string.c_str(), "bigFont.fnt");
		this->addChild(percentage_label, 21);
		percentage_label->setID("percentage-label"_spr);

		if (gm->m_showProgressBar) {
			percentage_label->setPositionX((w_size.width / 2) + 110.0f);
			percentage_label->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
		} else {
			percentage_label->setPositionX(w_size.width / 2);
			percentage_label->setAnchorPoint(cocos2d::CCPoint(0.5f, 0.0f));
		}

		percentage_label->setPositionY(director->getScreenTop() - 16.0f);
		percentage_label->setScale(0.5f);

		if (!gm->getGameVariable(GameVariable::SHOW_PERCENTAGE)) {
			percentage_label->setVisible(false);
		}

		auto hitboxNode = EditorHitboxLayer::create();
		this->m_gameLayer->addChild(hitboxNode, 9);
		hitboxNode->setVisible(false);
		hitboxNode->setSkipHitboxUpdates(true);

		extension_object->setHitboxLayer(hitboxNode);

		auto time_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
		this->addChild(time_sprite, 21);
		extension_object->setTimeIcon(time_sprite);

		auto screen_side = gm->getGameVariable(GameVariable::FLIP_PAUSE) ? director->getScreenRight() : director->getScreenLeft();
		auto screen_positioning_scale = gm->getGameVariable(GameVariable::FLIP_PAUSE) ? -1 : 1;
		auto screen_top = director->getScreenTop();

		time_sprite->setPositionX(screen_side + screen_positioning_scale * 13.0f);
		time_sprite->setPositionY(screen_top - 13.0f);

		time_sprite->setOpacity(127);
		time_sprite->setScale(0.9f);
		time_sprite->setID("speedhack-icon"_spr);

		auto speedhack_interval = SpeedhackManagerCard::getSpeedhackInterval();
		auto speedhack_string = cocos2d::CCString::createWithFormat("%.2fx", speedhack_interval);
		auto speedhack_label = cocos2d::CCLabelBMFont::create(
				speedhack_string->getCString(), "bigFont.fnt");

		this->addChild(speedhack_label, 21);
		speedhack_label->setPosition(screen_side + screen_positioning_scale * 27.0f, screen_top - 12.0f);
		speedhack_label->setScale(0.5f);
		speedhack_label->setOpacity(127);
		speedhack_label->setAnchorPoint({0.0f, 0.5f});
		speedhack_label->setID("speedhack-label"_spr);
		extension_object->setTimeLabel(speedhack_label);

		if (gm->getGameVariable(GameVariable::FLIP_PAUSE)) {
			speedhack_label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentRight);
			speedhack_label->setAnchorPoint({1.0f, 0.5f});
		}

		auto cheat_indicator = cocos2d::CCLabelBMFont::create(".", "bigFont.fnt");
		this->addChild(cheat_indicator, 21);
		cheat_indicator->setPosition(
			director->getScreenRight() - 20.0f,
			director->getScreenBottom() + 20.0f
		);
		extension_object->setCheatIndicator(cheat_indicator);
		cheat_indicator->setID("cheat-indicator"_spr);

		auto lifecycle = SpeedhackLifecycleWatcher::create();
		extension_object->setSpeedhackWatcher(lifecycle);

		lifecycle->beginSpeedhack();

		SpeedhackManagerCard::setSpeedhackValue(speedhack_interval);

		this->setUserObject("hacks"_spr, extension_object);

		extension_object->resetCheats();

		return true;
	}

#ifndef GEODE_IS_WINDOWS
	void setupReplay(gd::string replay) {
		PlayLayer::setupReplay(replay);

		if (this->getPlaybackMode()) {
			auto label = cocos2d::CCLabelBMFont::create("Playback", "bigFont.fnt");
			this->addChild(label, 10);

			auto director = cocos2d::CCDirector::sharedDirector();
			auto screen_left = director->getScreenLeft();
			auto screen_top = director->getScreenTop();

			label->setPositionX(screen_left + 43.0f);
			label->setPositionY(screen_top - 10.0f);

			label->setScale(0.5f);
			label->setOpacity(127);
		}
	}
#endif

	void updateProgressbar() {
		PlayLayer::updateProgressbar();
		this->updatePercentageLabel();
	}

	// updateProgressBar is inlined on windows in these places
	void resetLevel() {
		if (m_level->m_levelType == GJLevelType::Local) {
			SecretManager::get().playLevel(m_level->m_levelID);
		} else {
			SecretManager::get().clearStatus();
		}

		if (this->m_practiceMode || this->m_testMode) {
			m_fields->m_smoothFrames = 2;
		}

		PlayLayer::resetLevel();

#ifdef GEODE_IS_WINDOWS
		this->updatePercentageLabel();
#endif
	}

	virtual void update(float dt) override {
		if (this->m_practiceMode || this->m_testMode) {
			auto& fields = m_fields;
			if (fields->m_smoothFrames > 0) {
				auto ideal_dt = cocos2d::CCDirector::sharedDirector()->getAnimationInterval();

				if (dt - ideal_dt < 1) {
					fields->m_smoothFrames--;
				}

				dt = ideal_dt;
			}
		}

		PlayLayer::update(dt);

#ifdef GEODE_IS_WINDOWS
		this->updatePercentageLabel();
#endif
	}

	float timeForXPos(float xPos, bool clean) {
		if (this->m_practiceMode) {
			// this causes practice music to be based on checked speed portals on unpause
			// not ideal, but this was already the case on respawn so... whatever
			// (the game clears the list of activated speed portals on reset)
			clean = true;
		}

		return PlayLayer::timeForXPos(xPos, clean);
	}

	void loadLastCheckpoint() {
		// fix for the practice blending bug
		// the game stores the current coloraction based on the current blending status,
		// but doesn't restore blending until after creating the color action
		if (this->m_checkpoints->count() > 0) {
			auto checkpoint = static_cast<CheckpointObject*>(this->m_checkpoints->lastObject());

			this->updateCustomColorBlend(3, checkpoint->m_customColor01Action->m_blend);
			this->updateCustomColorBlend(4, checkpoint->m_customColor02Action->m_blend);
			this->updateCustomColorBlend(6, checkpoint->m_customColor03Action->m_blend);
			this->updateCustomColorBlend(7, checkpoint->m_customColor04Action->m_blend);
			this->updateCustomColorBlend(8, checkpoint->m_dLineColorAction->m_blend);
		}

		PlayLayer::loadLastCheckpoint();

		// silly bugfix for restarting while endscreen is active
		if (this->m_checkpoints->count() > 0) {
			auto checkpoint = static_cast<CheckpointObject*>(this->m_checkpoints->lastObject());

			if (m_endPortalObject->m_spawnXPosition <= checkpoint->m_playerCheck01->m_playerPos.x) {
				m_endPortalObject->triggerObject();
			}
		}
	}

	void resume() {
		PlayLayer::resume();

		// i don't know what bug rob was fixing with this one, but i'm unfixing it. sorry robtop
		if (AppDelegate::get()->m_paused || m_isDead || m_player->getPositionX() > 0.0f) {
			return;
		}

		auto audioEngine = FMODAudioEngine::sharedEngine();
		if (!audioEngine->isBackgroundMusicPlaying()) {
			auto fadeMusic = m_levelSettings->m_fadeIn;

			auto filename = m_level->getAudioFileName();
			if (fadeMusic) {
				audioEngine->pauseBackgroundMusic();
			}

			GameSoundManager::sharedManager()->playBackgroundMusic(filename, false, false);

			auto offset = m_levelSettings->m_songOffset;
			if (offset > 0.0f) {
				audioEngine->setBackgroundMusicTime(offset);
			}

			if (fadeMusic) {
				audioEngine->fadeBackgroundMusic(true, 2.0f);
			}
		}
	}

	void pauseGame(bool p1) {
		auto extension_object = static_cast<PlayLayerExt*>(this->getUserObject("hacks"_spr));
		if (extension_object && !extension_object->getPausingSafe()) {
			return;
		}

		PlayLayer::pauseGame(p1);

		if (AppDelegate::get()->m_paused || m_endTriggered || m_showingEndLayer) {
			return;
		}
	}

	virtual void onEnterTransitionDidFinish() override {
		PlayLayer::onEnterTransitionDidFinish();

		if (auto extension_object = static_cast<PlayLayerExt*>(this->getUserObject("hacks"_spr)); extension_object != nullptr) {
			extension_object->setPausingSafe(true);
		}
	}
};

#ifdef GDMOD_ENABLE_LOGGING

struct BypassEditLevelLayer : geode::Modify<BypassEditLevelLayer, EditLevelLayer> {
	void onShare(cocos2d::CCObject* target) {
		auto gm = GameManager::sharedState();
		bool vfb_enabled = gm->getGameVariable(GameVariable::BYPASS_VERIFY);

		if (vfb_enabled) {
			auto was_verified = this->m_level->m_isVerified;
			this->m_level->m_isVerified = true;
			EditLevelLayer::onShare(target);
			this->m_level->m_isVerified = was_verified;

			return;
		}

		EditLevelLayer::onShare(target);
	}
};

#endif

struct AwakeFMODAudioEngine : geode::Modify<AwakeFMODAudioEngine, FMODAudioEngine> {
	void playBackgroundMusic(gd::string path, bool loop, bool paused) {
		if (path != "menuLoop.mp3") {
#ifdef GEODE_IS_ANDROID
			// ignore menu song
			if (GameManager::sharedState()->getGameVariable(GameVariable::KEEP_AWAKE)) {
				PlatformHelper::keep_screen_awake();
			}
#endif
		} else {
			if (GameManager::sharedState()->getGameVariable(GameVariable::DISABLE_MENU_MUSIC)) {
				return;
			}
		}

		FMODAudioEngine::playBackgroundMusic(path, loop, paused);
	}

#ifdef GEODE_IS_ANDROID
	void pauseBackgroundMusic() {
		FMODAudioEngine::pauseBackgroundMusic();

		if (GameManager::sharedState()->getGameVariable(GameVariable::KEEP_AWAKE)) {
			PlatformHelper::remove_screen_awake();
		}
	}

	void stopBackgroundMusic(bool p1) {
		FMODAudioEngine::stopBackgroundMusic(p1);

		if (GameManager::sharedState()->getGameVariable(GameVariable::KEEP_AWAKE)) {
			PlatformHelper::remove_screen_awake();
		}
	}
#endif

	static void onModify(auto& self) {
		if (!self.setHookPriority("FMODAudioEngine::playBackgroundMusic", geode::Priority::Early)) {
			geode::log::warn("failed to set hook priority for FMODAudioEngine::playBackgroundMusic");
		}
	}
};

struct CustomLevelInfoLayer : geode::Modify<CustomLevelInfoLayer, LevelInfoLayer> {
	bool init(GJGameLevel* lvl) {
		bool ch_enabled = GameManager::sharedState()->getGameVariable(GameVariable::COPY_HACK);

		if (ch_enabled) {
			int old_password = lvl->m_password;
			lvl->m_password = 1;

			auto r = LevelInfoLayer::init(lvl);
			lvl->m_password = old_password;
			return r;
		}

		return LevelInfoLayer::init(lvl);
	}

	void tryCloneLevel(cocos2d::CCObject* target) {
		// TODO: this was not the lazy way
		bool ch_enabled = GameManager::sharedState()->getGameVariable(GameVariable::COPY_HACK);

		if (ch_enabled) {
			int old_password = this->m_level->m_password;
			this->m_level->m_password = 1;

			LevelInfoLayer::tryCloneLevel(target);
			this->m_level->m_password = old_password;

			return;
		}

		LevelInfoLayer::tryCloneLevel(target);
	}
};

struct CopyShareLevelSettingsLayer : geode::Modify<CopyShareLevelSettingsLayer, ShareLevelSettingsLayer> {
	bool init(GJGameLevel* lvl) {
		bool ch_enabled = GameManager::sharedState()->getGameVariable(GameVariable::COPY_HACK);
		if (ch_enabled) {
			lvl->m_password = 1;
		}

		if (!ShareLevelSettingsLayer::init(lvl)) {
			return false;
		}

		if (ch_enabled) {
			auto password_check = this->m_passwordToggle;
			password_check->m_notClickable = true;
			password_check->setEnabled(false);
			password_check->setColor({ 0xAB, 0xAB, 0xAB });

			auto password_label = this->m_passwordToggleLabel;
			password_label->setColor({ 0xAB, 0xAB, 0xAB });

			auto copyable_check = reinterpret_cast<CCMenuItemToggler*>(this->m_buttonMenu->getChildren()->objectAtIndex(1));
			copyable_check->m_notClickable = true;
			copyable_check->setEnabled(false);
			copyable_check->setColor({ 0xAB, 0xAB, 0xAB });

			auto copyable_label = reinterpret_cast<cocos2d::CCLabelBMFont*>(this->m_mainLayer->getChildren()->objectAtIndex(4));
			copyable_label->setColor({ 0xAB, 0xAB, 0xAB });

			auto sorry = cocos2d::CCLabelBMFont::create("Copy protection broke :(\nSorry...", "goldFont.fnt");
			this->getInternalLayer()->addChild(sorry);

			auto director = cocos2d::CCDirector::sharedDirector();

			sorry->setPositionX(director->getScreenRight() / 2);
			sorry->setPositionY(director->getScreenTop() / 2);
			sorry->setScale(0.8f);
		}

		return true;
	}
};

struct CopyShareLevelLayer : geode::Modify<CopyShareLevelLayer, ShareLevelLayer> {
	bool init(GJGameLevel* lvl) {
		bool ch_enabled = GameManager::sharedState()->getGameVariable(GameVariable::COPY_HACK);
		if (ch_enabled) {
			lvl->m_password = 1;
		}

		return ShareLevelLayer::init(lvl);
	}
};

struct CustomEndLevelLayer : geode::Modify<CustomEndLevelLayer, EndLevelLayer> {
	void customSetup() {
		auto cheated = false;
		auto playLayer = GameManager::sharedState()->m_playLayer;
		if (auto extension_object = geode::cast::typeinfo_cast<PlayLayerExt*>(playLayer->getUserObject("hacks"_spr)); extension_object != nullptr) {
			auto cheats_enabled = extension_object->determineCheatStatus();
			auto is_cheating = extension_object->getIsIllegitimate();
			auto attemptIgnored = playLayer->m_practiceMode || playLayer->m_testMode;

			if ((cheats_enabled || is_cheating) && !attemptIgnored) {
				cheated = true;
			}
		}

		if (cheated) {
			// honestly, probably the easiest way to get it to not show the coins
			auto level = playLayer->m_level;
			auto oldCoins = level->m_coins;
			level->m_coins = 0;

			EndLevelLayer::customSetup();

			level->m_coins = oldCoins;

			auto message = "You cannot complete a level after using cheats.";
			auto isVerifying = playLayer->m_level->m_levelType == GJLevelType::Editor;
			if (isVerifying) {
				message = "You cannot verify a level after using cheats.";
			}

			auto textArea = static_cast<TextArea*>(this->m_mainLayer->getChildByID("complete-message"));
			if (!textArea) {
				// this is the case for beating a level in normal mode, a label is used instead
				// so we remake the textarea :)

				auto messageArea = TextArea::create(" ", 620.0f, 0, {0.5f, 1.0f}, "bigFont.fnt", 20.0f);
				this->m_mainLayer->addChild(messageArea);
				messageArea->setID("cheats-message"_spr);

				messageArea->setScale(0.55f);
				messageArea->m_height = 34.0f;

				auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
				messageArea->setPosition({winSize.width * 0.5f, winSize.height * 0.5f - 51.0f});

				messageArea->setString(message);

				// hide the message that is originally being shown
				auto winLabel = this->m_mainLayer->getChildByType<cocos2d::CCLabelBMFont>(-1);
				winLabel->setVisible(false);

				return;
			}

			textArea->setString(message);
			return;
		}

		EndLevelLayer::customSetup();

		if (playLayer->m_practiceMode) {
			auto buttonMenu = static_cast<cocos2d::CCMenu*>(m_mainLayer->getChildByID("button-menu"));
			auto restartSprite = cocos2d::CCSprite::createWithSpriteFrameName("restartCheckBtn.png"_spr);
			auto restartBtn = CCMenuItemSpriteExtra::create(restartSprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&CustomEndLevelLayer::onReplayLastCheckpoint));

			auto retryBtn = buttonMenu->getChildByID("retry-button");

			buttonMenu->addChild(restartBtn);
			restartBtn->setID("practice-retry-btn"_spr);

			restartBtn->setPositionX(-183.0f);
			restartBtn->setPositionY(retryBtn->getPositionY());
		}
	}

	void onReplayLastCheckpoint(cocos2d::CCObject*) {
		if (!GameManager::sharedState()->getGameVariable(GameVariable::SHOW_CURSOR)) {
			PlatformToolbox::hideCursor();
		}

		GameSoundManager::sharedManager()->stopBackgroundMusic();
		GameSoundManager::sharedManager()->playEffect("playSound_01.ogg", 1.0f, 0.0f, 0.3f);

		this->exitLayer(nullptr);

		auto playLayer = GameManager::sharedState()->m_playLayer;

		auto sequence = cocos2d::CCSequence::createWithTwoActions(
			cocos2d::CCDelayTime::create(0.5f),
			cocos2d::CCCallFunc::create(playLayer, static_cast<cocos2d::SEL_CallFunc>(&PlayLayer::resetLevel))
		);

		playLayer->runAction(sequence);
	}
};

struct QuickUILayer : geode::Modify<QuickUILayer, UILayer> {
	virtual void keyUp(cocos2d::enumKeyCodes key) override {
		UILayer::keyUp(key);

		auto playLayer = GameManager::sharedState()->m_playLayer;

		if (key == cocos2d::enumKeyCodes::KEY_W) {
			this->m_p1Jumping = false;
			playLayer->releaseButton(1, true);
			return;
		}
	}

	virtual void keyDown(cocos2d::enumKeyCodes key) override {
		UILayer::keyDown(key);

		auto playLayer = GameManager::sharedState()->m_playLayer;

		if (key == cocos2d::enumKeyCodes::KEY_W) {
			if (!this->m_p1Jumping) {
				this->m_p1Jumping = true;
				playLayer->pushButton(1, true);
			}
			return;
		}

		auto canPause = !playLayer->m_endTriggered && !playLayer->m_showingEndLayer;

		if (!canPause) {
			return;
		}

		if (key == cocos2d::enumKeyCodes::KEY_R) {
			auto ctrlPressed = cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher()->getControlKeyPressed();
			if (ctrlPressed) {
				playLayer->fullReset();
			} else {
				playLayer->resetLevel();
			}
		}

		if (key == cocos2d::enumKeyCodes::KEY_F) {
			if (updatePrevStartPos()) {
				while (playLayer->m_checkpoints->count() > 0) {
					playLayer->removeLastCheckpoint();
				}

				playLayer->resetLevel();
			}
		}

		if (key == cocos2d::enumKeyCodes::KEY_H) {
			if (updateNextStartPos()) {
				while (playLayer->m_checkpoints->count() > 0) {
					playLayer->removeLastCheckpoint();
				}

				playLayer->resetLevel();
			}
		}
	}
};

#include <Geode/modify/LevelSettingsObject.hpp>

// fixes a bug where strings that fail to unzip crash the game
struct FixLevelSettingsObject : geode::Modify<FixLevelSettingsObject, LevelSettingsObject> {
	static LevelSettingsObject* objectFromDict(cocos2d::CCDictionary* dict) {
		if (dict == nullptr) {
			return LevelSettingsObject::create();
		}

		return LevelSettingsObject::objectFromDict(dict);
	}
};
