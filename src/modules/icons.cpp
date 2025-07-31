#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/SimplePlayer.hpp>
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/modify/GaragePage.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GameLevelManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/AchievementManager.hpp>
#include <Geode/modify/MenuGameLayer.hpp>
#include <Geode/modify/AchievementBar.hpp>
#include <Geode/modify/AchievementCell.hpp>

#include "base/game_variables.hpp"

#include "classes/managers/outfitmanager.hpp"

#include <random>

struct ExtendedAchievementManager : geode::Modify<ExtendedAchievementManager, AchievementManager> {
	struct AchievementInfo {
		const char* identifier;
		const char* achievedDescription;
		float order;
		const char* title;
		const char* icon;
		const char* unachievedDescription;
	};

	void addCustomAchievement(AchievementInfo info) {
		auto subDict = cocos2d::CCDictionary::create();
		subDict->setObject(cocos2d::CCString::create(info.achievedDescription), "achievedDescription");
		subDict->setObject(cocos2d::CCString::create(info.identifier), "identifier");
		subDict->setObject(cocos2d::CCString::create(fmt::format("{}", info.order)), "order");
		subDict->setObject(cocos2d::CCString::create(info.title), "title");
		subDict->setObject(cocos2d::CCString::create(info.icon), "icon");
		subDict->setObject(cocos2d::CCString::create(info.unachievedDescription), "unachievedDescription");
		subDict->setObject(cocos2d::CCString::create(""), "googleID");

		this->m_allAchievements->setObject(subDict, info.identifier);
	}

	virtual bool init() override {
		if (!AchievementManager::init()) {
			return false;
		}

		this->addCustomAchievement({
			.identifier = "geometry.ach.level19a",
			.achievedDescription = "Completed \"Thumper\" in Practice mode",
			.order = 14.761f,
			.title = "Beetle",
			.icon = "color_31",
			.unachievedDescription = "Complete \"Thumper\" in Practice mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.level19b",
			.achievedDescription = "Completed \"Thumper\" in Normal mode",
			.order = 14.762f,
			.title = "Thumper!",
			.icon = "ship_19",
			.unachievedDescription = "Complete \"Thumper\" in Normal mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.level20a",
			.achievedDescription = "Completed \"Clutterfunk 2\" in Practice mode",
			.order = 14.763f,
			.title = "Second Clutter",
			.icon = "color_26",
			.unachievedDescription = "Complete \"Clutterfunk 2\" in Practice mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.level20b",
			.achievedDescription = "Completed \"Clutterfunk 2\" in Normal mode",
			.order = 14.764f,
			.title = "Clutterfunk 2!",
			.icon = "icon_53",
			.unachievedDescription = "Complete \"Clutterfunk 2\" in Normal mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.level21a",
			.achievedDescription = "Completed \"Aura\" in Practice mode",
			.order = 14.765f,
			.title = "Sigma",
			.icon = "color_33",
			.unachievedDescription = "Complete \"Aura\" in Practice mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.level21b",
			.achievedDescription = "Completed \"Aura\" in Normal mode",
			.order = 14.766f,
			.title = "Aura!",
			.icon = "icon_52",
			.unachievedDescription = "Complete \"Aura\" in Normal mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.level22a",
			.achievedDescription = "Completed \"Jack Russel\" in Practice mode",
			.order = 14.767f,
			.title = "Terrier",
			.icon = "color_30",
			.unachievedDescription = "Complete \"Jack Russel\" in Practice mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.level22b",
			.achievedDescription = "Completed \"Jack Russel\" in Normal mode",
			.order = 14.768f,
			.title = "Jack Russel!",
			.icon = "icon_50",
			.unachievedDescription = "Complete \"Jack Russel\" in Normal mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.level23a",
			.achievedDescription = "Completed \"Streetwise\" in Practice mode",
			.order = 14.769f,
			.title = "Book smart",
			.icon = "color_35",
			.unachievedDescription = "Complete \"Streetwise\" in Practice mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.level23b",
			.achievedDescription = "Completed \"Streetwise\" in Normal mode",
			.order = 14.77f,
			.title = "Streetwise!",
			.icon = "ball_11",
			.unachievedDescription = "Complete \"Streetwise\" in Normal mode"
		});

		// move this achievement for some more room

		this->addCustomAchievement({
			.identifier = "geometry.ach.demon01",
			.achievedDescription = "Completed a Demon difficulty level in Normal mode",
			.order = 14.78f,
			.title = "Reflex Champion",
			.icon = "icon_19",
			.unachievedDescription = "Complete a Demon difficulty level in Normal mode"
		});

		// next is 14.8

		this->addCustomAchievement({
			.identifier = "geometry.ach.demon11",
			.achievedDescription = "Completed 50 Demon difficulty levels in Normal mode",
			.order = 14.935f,
			.title = "Liberator of Souls",
			.icon = "color2_30",
			.unachievedDescription = "Complete 50 Demon difficulty levels in Normal mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.stars13",
			.achievedDescription = "Collected 3000 stars",
			.order = 14.989f,
			.title = "Wish on a fading star",
			.icon = "color2_33",
			.unachievedDescription = "Collect 3000 stars"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.mappacks08",
			.achievedDescription = "Completed 50 Map Packs",
			.order = 16.26f,
			.title = "Underneath a rainbow",
			.icon = "color_36",
			.unachievedDescription = "Complete 50 Map Packs"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.mappacks09",
			.achievedDescription = "Completed 100 Map Packs",
			.order = 16.27f,
			.title = "Was it worth it?",
			.icon = "special_03",
			.unachievedDescription = "Complete 100 Map Packs"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.custom09",
			.achievedDescription = "Completed 2000 user created levels in Normal mode",
			.order = 16.47f,
			.title = "How many more are there?!",
			.icon = "color2_35",
			.unachievedDescription = "Complete 2000 user created levels in Normal mode"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.jump05",
			.achievedDescription = "Jumped 15000 times",
			.order = 19.05f,
			.title = "Extreme hopper",
			.icon = "color2_31",
			.unachievedDescription = "Jump 15000 times"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.attempt05",
			.achievedDescription = "Did 5000 attempts",
			.order = 21.55f,
			.title = "Maybe it's the other route",
			.icon = "color2_26",
			.unachievedDescription = "Do 5000 attempts"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.attempt06",
			.achievedDescription = "Did 1000000 attempts",
			.order = 21.7f,
			.title = "Touch grass",
			.icon = "icon_51",
			.unachievedDescription = "Do 1000000 attempts"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.like04",
			.achievedDescription = "Liked or Disliked 500 online levels",
			.order = 22.15f,
			.title = "Arbitrator",
			.icon = "color2_36",
			.unachievedDescription = "Like or Dislike 500 online levels"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.clevel01",
			.achievedDescription = "Completed the \"Riots Choice Pack\"",
			.order = 27.1f,
			.title = "Swag",
			.icon = "special_04",
			.unachievedDescription = "Complete the \"Riots Choice Pack\""
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.clevel02",
			.achievedDescription = "Completed \"Lunar Intoxication\" (30449) in Normal mode",
			.order = 27.2f,
			.title = "GG Cyclic!",
			.icon = "icon_49",
			.unachievedDescription = "Complete \"Lunar Intoxication\" (30449) in Normal mode"
		});

		// replacing some original achievements

		this->addCustomAchievement({
			.identifier = "geometry.ach.moreGames",
			.achievedDescription = "Viewed the credits",
			.order = 26.0f,
			.title = "Brought to you by",
			.icon = "color2_05",
			.unachievedDescription = "Tap the Special Thanks button!"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.facebook",
			.achievedDescription = "Joined the 1.9 GDPS Discord server",
			.order = 26.1f,
			.title = "Number one fan!",
			.icon = "color2_14",
			.unachievedDescription = "Join the 1.9 GDPS Discord server"
		});

		this->addCustomAchievement({
			.identifier = "geometry.ach.youtube",
			.achievedDescription = "Subscribed to the 1.9 GDPS on YouTube",
			.order = 26.2f,
			.title = "RetroTube",
			.icon = "color2_21",
			.unachievedDescription = "Subscribe to the 1.9 GDPS on YouTube"
		});

		this->m_allAchievementsSorted->release();
		auto newSorted = this->getAllAchievementsSorted();
		newSorted->retain();

		this->m_allAchievementsSorted = newSorted;

		return true;
	}
};

struct CustomPlayerObject : geode::Modify<CustomPlayerObject, PlayerObject> {
	void setupStreak() {
		auto streakMax = OutfitManager::get_manager().max_for_type(IconType::Special);
		auto currentStreak = std::clamp(GameManager::sharedState()->m_playerStreak, 1, streakMax);

		this->m_streakType = currentStreak;

		auto streakName = OutfitManager::get_manager().is_custom(IconType::Special, currentStreak)
			? fmt::format("streak_{:02d}_001.png"_spr, currentStreak)
			: fmt::format("streak_{:02d}_001.png", currentStreak);

		auto stroke = this->m_streakType == 2 ? 14.0f : 10.0f;

		auto streak = cocos2d::CCMotionStreak::create(0.3f, 3.0f, stroke, {0xff, 0xff, 0xff}, streakName.c_str());
		this->m_playerStreak = streak;

		streak->setM_fMaxSeg(50.0f);
		this->m_gameLayer->addChild(this->m_playerStreak, 0);

		cocos2d::ccBlendFunc blendingFunc{GL_SRC_ALPHA, GL_ONE};
		this->m_playerStreak->setBlendFunc(blendingFunc);

		auto hardStreak = HardStreak::create();
		this->m_hardStreak = hardStreak;
		this->m_gameLayer->addChild(hardStreak, 0);

		if (GameManager::sharedState()->m_playerColor == 15) {
			hardStreak->m_disableDual = true;
		} else {
			// i'm copying what the game does
			hardStreak->cocos2d::CCDrawNode::setBlendFunc(blendingFunc);
		}

		this->deactivateStreak();
	}

	void updatePlayerFrame(int id) {
		auto frameMax = OutfitManager::get_manager().max_for_type(IconType::Cube);
		auto currentFrame = std::clamp(id, 0, frameMax);

		auto isCustom = OutfitManager::get_manager().is_custom(IconType::Cube, id);

		auto firstName = isCustom
			? fmt::format("player_{:02d}_001.png"_spr, currentFrame)
			: fmt::format("player_{:02d}_001.png", currentFrame);
		auto secondName = isCustom
			? fmt::format("player_{:02d}_2_001.png"_spr, currentFrame)
			: fmt::format("player_{:02d}_2_001.png", currentFrame);
		auto glowName = isCustom
			? fmt::format("player_{:02d}_glow_001.png"_spr, currentFrame)
			: fmt::format("player_{:02d}_glow_001.png", currentFrame);

		auto frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();

		if (auto frame = frameCache->spriteFrameByName(firstName.c_str()); frame != nullptr) {
			this->m_playerFrame->setDisplayFrame(frame);
		}

		if (auto frame = frameCache->spriteFrameByName(secondName.c_str()); frame != nullptr) {
			this->m_playerFrameSecondary->setDisplayFrame(frame);
		}

		if (auto frame = frameCache->spriteFrameByName(glowName.c_str()); frame != nullptr) {
			this->m_playerFrameGlow->setDisplayFrame(frame);
			this->m_playerFrameGlow->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
		}

		auto contentSize = this->m_playerFrame->getContentSize();

		this->m_playerFrameSecondary->setPosition({contentSize.width * 0.5f, contentSize.height * 0.5f});
	}

	void updatePlayerRollFrame(int id) {
		auto frameMax = OutfitManager::get_manager().max_for_type(IconType::Ball);
		auto currentFrame = std::clamp(id, 0, frameMax);

		auto isCustom = OutfitManager::get_manager().is_custom(IconType::Ball, id);

		auto firstName = isCustom
			? fmt::format("player_ball_{:02d}_001.png"_spr, currentFrame)
			: fmt::format("player_ball_{:02d}_001.png", currentFrame);
		auto secondName = isCustom
			? fmt::format("player_ball_{:02d}_2_001.png"_spr, currentFrame)
			: fmt::format("player_ball_{:02d}_2_001.png", currentFrame);
		auto glowName = isCustom
			? fmt::format("player_ball_{:02d}_glow_001.png"_spr, currentFrame)
			: fmt::format("player_ball_{:02d}_glow_001.png", currentFrame);

		auto frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();

		if (auto frame = frameCache->spriteFrameByName(firstName.c_str()); frame != nullptr) {
			this->m_playerFrame->setDisplayFrame(frame);
		}

		if (auto frame = frameCache->spriteFrameByName(secondName.c_str()); frame != nullptr) {
			this->m_playerFrameSecondary->setDisplayFrame(frame);
		}

		if (auto frame = frameCache->spriteFrameByName(glowName.c_str()); frame != nullptr) {
			this->m_playerFrameGlow->setDisplayFrame(frame);
			this->m_playerFrameGlow->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
		}

		auto contentSize = this->m_playerFrame->getContentSize();

		this->m_playerFrameSecondary->setPosition({contentSize.width * 0.5f, contentSize.height * 0.5f});
	}

	void updatePlayerShipFrame(int id) {
		auto frameMax = OutfitManager::get_manager().max_for_type(IconType::Ship);
		auto currentFrame = std::clamp(id, 1, frameMax);

		auto isCustom = OutfitManager::get_manager().is_custom(IconType::Ship, id);

		auto firstName = isCustom
			? fmt::format("ship_{:02d}_001.png"_spr, currentFrame)
			: fmt::format("ship_{:02d}_001.png", currentFrame);
		auto secondName = isCustom
			? fmt::format("ship_{:02d}_2_001.png"_spr, currentFrame)
			: fmt::format("ship_{:02d}_2_001.png", currentFrame);
		auto glowName = isCustom
			? fmt::format("ship_{:02d}_glow_001.png"_spr, currentFrame)
			: fmt::format("ship_{:02d}_glow_001.png", currentFrame);

		auto frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();

		if (auto frame = frameCache->spriteFrameByName(firstName.c_str()); frame != nullptr) {
			this->m_vehicleFrame->setDisplayFrame(frame);
		}

		if (auto frame = frameCache->spriteFrameByName(secondName.c_str()); frame != nullptr) {
			this->m_vehicleFrameSecondary->setDisplayFrame(frame);
		}

		if (auto frame = frameCache->spriteFrameByName(glowName.c_str()); frame != nullptr) {
			this->m_vehicleFrameGlow->setDisplayFrame(frame);
			this->m_vehicleFrameGlow->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
		}

		auto contentSize = this->m_vehicleFrame->getContentSize();

		this->m_vehicleFrameSecondary->setPosition({contentSize.width * 0.5f, contentSize.height * 0.5f});
	}

	void updatePlayerBirdFrame(int id) {
		auto frameMax = OutfitManager::get_manager().max_for_type(IconType::Ufo);
		auto currentFrame = std::clamp(id, 1, frameMax);

		auto isCustom = OutfitManager::get_manager().is_custom(IconType::Ufo, id);

		auto firstName = isCustom
						? fmt::format("bird_{:02d}_001.png"_spr, currentFrame)
						: fmt::format("bird_{:02d}_001.png", currentFrame);
		auto secondName = isCustom
			? fmt::format("bird_{:02d}_2_001.png"_spr, currentFrame)
			: fmt::format("bird_{:02d}_2_001.png", currentFrame);
				auto thirdName = isCustom
			? fmt::format("bird_{:02d}_3_001.png"_spr, currentFrame)
			: fmt::format("bird_{:02d}_3_001.png", currentFrame);
		auto glowName = isCustom
						? fmt::format("bird_{:02d}_glow_001.png"_spr, currentFrame)
			: fmt::format("bird_{:02d}_glow_001.png", currentFrame);

		auto frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();

		if (auto frame = frameCache->spriteFrameByName(firstName.c_str()); frame != nullptr) {
			this->m_vehicleFrame->setDisplayFrame(frame);
		}

		if (auto frame = frameCache->spriteFrameByName(secondName.c_str()); frame != nullptr) {
			this->m_vehicleFrameSecondary->setDisplayFrame(frame);
		}

		if (auto frame = frameCache->spriteFrameByName(thirdName.c_str()); frame != nullptr) {
			this->m_vehicleFrameThird->setDisplayFrame(frame);
		}

		if (auto frame = frameCache->spriteFrameByName(glowName.c_str()); frame != nullptr) {
			this->m_vehicleFrameGlow->setDisplayFrame(frame);
			this->m_vehicleFrameGlow->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
		}

		auto contentSize = this->m_vehicleFrame->getContentSize();

		this->m_vehicleFrameSecondary->setPosition({contentSize.width * 0.5f, contentSize.height * 0.5f});

		this->m_vehicleFrameThird->setPosition(this->m_vehicleFrameSecondary->getPosition());
	}

	bool init(int frame, int ship, cocos2d::CCLayer* gameLayer) {
		if (!PlayerObject::init(frame, ship, gameLayer)) {
			return false;
		}

		// the ship is unnecessary, because it gets reset anyways
		this->updatePlayerFrame(frame);

		// correct it here as our icon fix breaks it
		this->m_playerFrameGlow->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
		this->m_vehicleFrameGlow->setBlendFunc({GL_SRC_ALPHA, GL_ONE});

		return true;
	}

	void updateGlowColor() {
		auto originalSpecial = this->m_streakType;

		auto glowEnabled = GameManager::sharedState()->getGameVariable(GameVariable::ICON_GLOW);
		this->m_streakType = glowEnabled ? 2 : 0;

		PlayerObject::updateGlowColor();

		this->m_streakType = originalSpecial;

		if (originalSpecial < 2) {
			this->m_playerStreak->setColor(this->m_glowColor2);
		} else {
			this->m_playerStreak->setColor({0xff, 0xff, 0xff});
		}
	}

	void resetPlayerIcon() {
		PlayerObject::resetPlayerIcon();

		if (this->m_streakType < 2) {
			this->m_playerStreak->setColor(this->m_glowColor2);
		} else {
			this->m_playerStreak->setColor({0xff, 0xff, 0xff});
		}
	}

	void updatePlayerGlow() {
		auto originalSpecial = this->m_streakType;

		auto glowEnabled = GameManager::sharedState()->getGameVariable(GameVariable::ICON_GLOW);
		this->m_streakType = glowEnabled ? 2 : 0;

		PlayerObject::updatePlayerGlow();

		this->m_streakType = originalSpecial;
	}

	// invisible dual fix
	// this is called in spawnPlayer2, but LevelEditorLayer::spawnPlayer2 is inlined
	void spawnFromPlayer(PlayerObject* player) {
		this->setVisible(true);
		PlayerObject::spawnFromPlayer(player);
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("PlayerObject::setupStreak", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for setupStreak");
		}

		if (!self.setHookPriority("PlayerObject::updatePlayerFrame", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for updatePlayerFrame");
		}

		if (!self.setHookPriority("PlayerObject::updatePlayerRollFrame", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for updatePlayerRollFrame");
		}

		if (!self.setHookPriority("PlayerObject::updatePlayerShipFrame", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for updatePlayerShipFrame");
		}

		if (!self.setHookPriority("PlayerObject::updatePlayerBirdFrame", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for updatePlayerBirdFrame");
		}
	}
};

struct CustomSimplePlayer : geode::Modify<CustomSimplePlayer, SimplePlayer> {
		void updatePlayerFrame(int id, IconType type) {
		const char* frameName = nullptr;
		auto iconMax = OutfitManager::get_manager().max_for_type(type);

		// this switch statement is partially inlined on android, so you can't set ball/ufo max
		switch (type) {
			default:
			case IconType::Cube:
				frameName = "player";
				break;
			case IconType::Ship:
				frameName = "ship";
				break;
			case IconType::Ball:
				frameName = "player_ball";
				break;
			case IconType::Ufo:
				frameName = "bird";
				break;
			case IconType::Wave:
				frameName = "dart";
				break;
		}

		id = std::clamp(id, 1, iconMax);

		auto isCustom = OutfitManager::get_manager().is_custom(type, id);

		auto firstFmt = isCustom
			? fmt::format("{}_{:02d}_001.png"_spr, frameName, id)
			: fmt::format("{}_{:02d}_001.png", frameName, id);
		auto secondFmt = isCustom
			? fmt::format("{}_{:02d}_2_001.png"_spr, frameName, id)
			: fmt::format("{}_{:02d}_2_001.png", frameName, id);
		auto thirdFmt = isCustom
			? fmt::format("{}_{:02d}_3_001.png"_spr, frameName, id)
			: fmt::format("{}_{:02d}_3_001.png", frameName, id);
		auto glowFmt = isCustom
			? fmt::format("{}_{:02d}_glow_001.png"_spr, frameName, id)
			: fmt::format("{}_{:02d}_glow_001.png", frameName, id);

		this->setFrames(
			firstFmt.c_str(),
			secondFmt.c_str(),
			type == IconType::Ufo ? thirdFmt.c_str() : nullptr,
			glowFmt.c_str()
		);

		if (type == IconType::Wave || type == IconType::Ship || type == IconType::Cube) {
			this->m_firstLayer->setScale(1.0f);
			this->m_firstLayer->setPosition({0.0f, 0.0f});
		} else if (type == IconType::Ball) {
			this->m_firstLayer->setScale(0.88f);
			this->m_firstLayer->setPosition({0.0f, 0.0f});
		} else if (type == IconType::Ufo) {
			this->m_firstLayer->setScale(0.9f);
			this->m_firstLayer->setPosition({0.0f, -7.0f});
		}
	}

	bool init(int id) {
		if (!SimplePlayer::init(id)) {
				return false;
		}

		this->updatePlayerFrame(id, IconType::Cube);

		return true;
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("SimplePlayer::updatePlayerFrame", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for updatePlayerFrame");
		}
	}
};

struct CustomGJGarageLayer : geode::Modify<CustomGJGarageLayer, GJGarageLayer> {
	// this is another function where optimizations makes it tricky, so we rewrite it
	// (limits us to multiples of two)
	void setupColorSelect() {
		auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
		this->m_colorSelector1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_select_001.png");
		this->m_colorSelector1->setScale(0.8f);
		this->addChild(this->m_colorSelector1, 1);

		this->m_colorSelector2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_select_001.png");
		this->m_colorSelector2->setScale(0.8f);
		this->addChild(this->m_colorSelector2, 1);

		auto colorMax = this->m_colorMaxX;
		auto colorMin = this->m_colorMinX;

		auto rowTop = cocos2d::CCArray::create();
		auto rowBottom = cocos2d::CCArray::create();

		auto initialPos = 0;
		auto itemWidth = 0.0f;

		auto maxId = OutfitManager::get_manager().max_colors() + 1;
		auto overflowCount = maxId % 12;

		for (auto i = 0; i < maxId; i++) {
			auto currentColor = GameManager::sharedState()->colorForPos(i);

			for (auto j = 0; j < 2; j++) {
				auto firstColor = j == 0;

				auto colorSprite = cocos2d::CCSprite::createWithSpriteFrameName("whiteSquare60_001.png");
				colorSprite->setScale(0.8f);
				colorSprite->setColor(GameManager::sharedState()->colorForIdx(currentColor));

				itemWidth = colorSprite->getTextureRect().size.width;

				auto callbackFn = static_cast<cocos2d::SEL_MenuHandler>(&GJGarageLayer::onPlayerColor1);
				if (!firstColor) {
					callbackFn = static_cast<cocos2d::SEL_MenuHandler>(&GJGarageLayer::onPlayerColor2);
				}

				auto colorBtn = CCMenuItemSpriteExtra::create(colorSprite, nullptr, this, callbackFn);
				colorBtn->setTag(currentColor);

				if (firstColor) {
					rowTop->addObject(colorBtn);
				} else {
					rowBottom->addObject(colorBtn);
				}

				if (!GameManager::sharedState()->isColorUnlocked(currentColor, firstColor)) {
					auto lockSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lockGray_001.png");
					lockSprite->setScale(0.8f);
					colorSprite->addChild(lockSprite);

					auto& colorRect = colorSprite->getTextureRect();
					lockSprite->setPosition({colorRect.size.width * 0.5f, colorRect.size.height * 0.5f});
				}

				if (firstColor) {
					if (GameManager::sharedState()->m_playerColor == currentColor) {
						this->m_selectedColor = colorBtn;
						initialPos = i;
					}
				} else {
					if (GameManager::sharedState()->m_playerColor2 == currentColor) {
						this->m_selectedColor2 = colorBtn;
					}
				}
			}
		}

		auto listItems = cocos2d::CCArray::create();

		while (rowTop->count() != 0) {
			for (int i = 0; i < 12; i++) {
				if (rowTop->count() == 0) {
					break;
				}

				auto first = rowTop->objectAtIndex(0);
				listItems->addObject(first);
				rowTop->removeObjectAtIndex(0, true);
			}

			for (int i = 0; i < 12; i++) {
				if (rowBottom->count() == 0) {
					break;
				}

				if (overflowCount != 0 && rowBottom->count() == overflowCount) {
					// in 2.1, he adds a special spacer tag for this, but we don't have one
					for (int j = 0; j < overflowCount; j++) {
						auto spacer = cocos2d::CCNode::create();
						spacer->setTag(-2);
						listItems->addObject(spacer);
					}
				}

				auto first = rowBottom->objectAtIndex(0);
				listItems->addObject(first);
				rowBottom->removeObjectAtIndex(0, true);
			}
		}

		auto bar = ListButtonBar::create(listItems, {winSize.width * 0.5f, (colorMax - colorMin * 0.5f) + 5.0f}, 12, 2, 5.0f, 5.0f, itemWidth * 0.8f, 200.0f, 2);
		this->addChild(bar, 100);
		bar->m_delegate = this;
		bar->setTag(1);
		bar->goToPage(initialPos / 12);
	}

	virtual bool init() override {
		if (!GJGarageLayer::init()) {
			return false;
		}

		auto glowEnabled = GameManager::sharedState()->getGameVariable(GameVariable::ICON_GLOW);

		this->m_playerPreview->m_special = glowEnabled ? 2 : 0;
		this->m_playerPreview->updateColors();

		return true;
	}

	void onGlow(cocos2d::CCObject*) {
		GameManager::sharedState()->toggleGameVariable(GameVariable::ICON_GLOW);
		GameManager::sharedState()->m_playerScoreValid = false;

		auto glowEnabled = GameManager::sharedState()->getGameVariable(GameVariable::ICON_GLOW);
		this->m_playerPreview->m_special = glowEnabled ? 2 : 0;
		this->m_playerPreview->updateColors();

		if (glowEnabled) {
			this->playRainbowEffect();
		}
		return;
	}

	void onSpecialIcon(cocos2d::CCObject* target) {
		auto special = static_cast<CCNode*>(target)->getTag();
		auto unlocked = this->checkSpecial(special);

		this->m_doSelect = unlocked;
		if (unlocked) {
			GameManager::sharedState()->m_playerStreak = special;
		}
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("GJGarageLayer::setupColorSelect", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for setupColorSelect");
		}

		if (!self.setHookPriority("GJGarageLayer::onSpecialIcon", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for onSpecialIcon");
		}
	}
};

struct CustomGaragePage : geode::Modify<CustomGaragePage, GaragePage> {
	bool init(IconType type, GJGarageLayer* garage, cocos2d::SEL_MenuHandler callback) {
		if (!cocos2d::CCLayer::init()) {
			return false;
		}

		this->m_garageLayer = garage;
		this->m_callback = callback;
		this->m_iconType = type;

		auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
		auto selectSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_select_001.png");
		this->m_selectSprite = selectSprite;

		selectSprite->setScale(0.85f);
		this->addChild(selectSprite, 1);

		auto icons = cocos2d::CCArray::create();
		this->m_perPage = 36;

		const char* frameName = nullptr;
		auto iconMax = OutfitManager::get_manager().max_for_type(type);
		UnlockType unlock = UnlockType::Cube;
		auto selectedIcon = 1;

		switch (type) {
			default:
			case IconType::Cube:
				frameName = "player";
				unlock = UnlockType::Cube;
				selectedIcon = GameManager::sharedState()->m_playerFrame;
				break;
			case IconType::Ship:
				frameName = "ship";
				unlock = UnlockType::Ship;
				selectedIcon = GameManager::sharedState()->m_playerShip;
				break;
			case IconType::Ball:
				frameName = "player_ball";
				unlock = UnlockType::Ball;
				selectedIcon = GameManager::sharedState()->m_playerBall;
				break;
			case IconType::Ufo:
				frameName = "bird";
				unlock = UnlockType::Bird;
				selectedIcon = GameManager::sharedState()->m_playerBird;
				break;
			case IconType::Special:
				frameName = "player_special";
				unlock = UnlockType::Streak;
				selectedIcon = GameManager::sharedState()->m_playerStreak;
				break;
		}

		auto sizeSprite = cocos2d::CCSprite::createWithSpriteFrameName("playerSquare_001.png");
		auto iconWidth = sizeSprite->getTextureRect().size.width;

		for (auto i = 1; i <= iconMax; i++) {
			cocos2d::CCSprite* addedSprite = nullptr;
			auto scaleFactor = 0.85f;

			if (!GameManager::sharedState()->isIconUnlocked(i, type)) {
#ifdef GEODE_IS_ANDROID
				// hide the steam exclusive icons on mobile
				if (type == IconType::Cube && (i == 47 || i == 48)) {
						continue;
				}
#endif

				auto baseSprite = cocos2d::CCSprite::createWithSpriteFrameName("playerSquare_001.png");
				auto lockSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_lock_001.png");

				baseSprite->addChild(lockSprite);
				auto& baseRect = baseSprite->getTextureRect();
				lockSprite->setPosition({baseRect.size.width * 0.5f, baseRect.size.height * 0.5f});

				baseSprite->setColor({0xaf, 0xaf, 0xaf});
				if (type == IconType::Cube && i == 13) {
					this->m_garageLayer->m_rateSprite = baseSprite;
				}

				addedSprite = baseSprite;
			} else {
				auto firstName = OutfitManager::get_manager().is_custom(type, i)
					? fmt::format("{}_{:02d}_001.png"_spr, frameName, i)
					: fmt::format("{}_{:02d}_001.png", frameName, i);

				auto secondName = OutfitManager::get_manager().is_custom(type, i)
					? fmt::format("{}_{:02d}_2_001.png"_spr, frameName, i)
					: fmt::format("{}_{:02d}_2_001.png", frameName, i);

				auto firstSprite = cocos2d::CCSprite::createWithSpriteFrameName(firstName.c_str());
				if (unlock != UnlockType::Streak) {
					auto secondSprite = cocos2d::CCSprite::createWithSpriteFrameName(secondName.c_str());
					firstSprite->addChild(secondSprite, -1);

					auto pos = firstSprite->convertToNodeSpace({0.0f, 0.0f});
					secondSprite->setPosition(pos);

					firstSprite->setColor({0xaf, 0xaf, 0xaf});
					secondSprite->setColor({0xff, 0xff, 0xff});
				}

				auto referenceSize = sizeSprite->getContentSize();
				auto referenceWidth = referenceSize.width * 0.85f;
				auto mainSize = firstSprite->getContentSize();
				auto mainWidth = mainSize.width;

				if ((referenceWidth / mainWidth) <= 0.85f) {
					scaleFactor = referenceWidth / mainWidth;
				}

				addedSprite = firstSprite;
			}

			addedSprite->setScale(scaleFactor);
			auto iconBtn = CCMenuItemSpriteExtra::create(addedSprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&GaragePage::onSelect));
			iconBtn->setTag(i);
			icons->addObject(iconBtn);

			if (i == selectedIcon) {
				this->m_selectedSprite = iconBtn;
			}
		}

		auto baseHeight = winSize.height * 0.5f - 5.0f;

		auto bar = ListButtonBar::create(icons, {winSize.width * 0.5f, baseHeight - 31.0f}, 12, 3, 5.0f, 5.0f, iconWidth * 0.85f, 220.0f, 1);
		this->addChild(bar, 100);
		bar->m_delegate = this;
		bar->setTag(static_cast<int>(type));
		bar->goToPage((selectedIcon - 1) / this->m_perPage);

		auto scrollLayer = bar->m_scrollLayer;
		scrollLayer->setPagesIndicatorPosition({winSize.width * 0.5f, baseHeight - 62.0f - 15.0f});
		scrollLayer->setDotScale(0.5f);
		scrollLayer->m_showPagesIndicator = false;

		if (unlock == UnlockType::Streak && GameManager::sharedState()->isIconUnlocked(2, IconType::Special)) {
			auto glowMenu = cocos2d::CCMenu::create();
			this->addChild(glowMenu);

			GameToolbox::createToggleButton(
				"Glow",
				static_cast<cocos2d::SEL_MenuHandler>(&CustomGJGarageLayer::onGlow),
				GameManager::sharedState()->getGameVariable(GameVariable::ICON_GLOW),
				glowMenu,
				{winSize.width * 0.5f + 120.0f, baseHeight},
				this->m_garageLayer,
				this
			);
		}

		return true;
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("GaragePage::init", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for GaragePage::init");
		}
	}
};

struct CustomGameManager : geode::Modify<CustomGameManager, GameManager> {
	cocos2d::ccColor3B colorForIdx(int idx) {
		auto color = OutfitManager::get_manager().get_color(idx);
		return color;
	}

	int colorForPos(int pos) {
		return OutfitManager::get_manager().color_for_pos(pos);
	}

	bool isIconUnlocked(int id, IconType type) {
		if ((type == IconType::Cube && id <= 4) || id == 1) {
			return true;
		}

		if (this->getGameVariable(GameVariable::UNLOCK_ALL)) {
			// unlock all logic
			if (id <= OutfitManager::get_manager().vanilla_max_for_type(type)) {
				return true;
			}
		}

#ifdef GDMOD_ENABLE_LOGGING
		if (this->getGameVariable(GameVariable::ALL_ICON_BYPASS)) {
			return true;
		}
#endif

		return this->m_valueKeeper->valueForKey(this->iconKey(id, type))->boolValue();
	}

	bool isColorUnlocked(int id, bool col2) {
		if (id <= 3) {
			return true;
		}

		if (this->getGameVariable(GameVariable::UNLOCK_ALL) && id <= 23) {
			// unlock all logic
			return true;
		}

#ifdef GDMOD_ENABLE_LOGGING
		if (this->getGameVariable(GameVariable::ALL_ICON_BYPASS)) {
			return true;
		}
#endif

		return this->m_valueKeeper->valueForKey(this->colorKey(id, col2))->boolValue();
	}

	void checkCoinCompletion(GJGameLevel* level) {
		if (level->m_levelID != 20 || level->m_levelType != GJLevelType::Local) {
			return;
		}

		auto coinCount = 0;

		auto gsm = GameStatsManager::sharedState();
		for (int i = 1; i <= 3; i++) {
			if (gsm->hasUniqueItem(level->getCoinKey(i))) {
				coinCount++;
			}
		}

		GameManager::sharedState()->reportAchievementWithID("geometry.ach.demoncoin04", std::min(static_cast<int>((coinCount / 3.0f) * 100.0f), 100), false);
	}

	virtual void dataLoaded(DS_Dictionary* data) override {
		GameManager::dataLoaded(data);

		if (data->getIntegerForKey("binaryVersion") < 26) {
			auto skipNotify = AchievementManager::sharedState()->m_dontNotify;
			AchievementManager::sharedState()->m_dontNotify = true;

			// recheck all custom achievements
			GameStatsManager::sharedState()->incrementStat("1", 0);
			GameStatsManager::sharedState()->incrementStat("2", 0);
			GameStatsManager::sharedState()->incrementStat("4", 0);
			GameStatsManager::sharedState()->incrementStat("5", 0);
			GameStatsManager::sharedState()->incrementStat("6", 0);
			GameStatsManager::sharedState()->incrementStat("7", 0);
			GameStatsManager::sharedState()->incrementStat("10", 0);

			// update main level achievements
			for (auto i = 19u; i <= 22; i++) {
				auto main_level = GameLevelManager::sharedState()->getMainLevel(i, false);
				if (!main_level) {
					continue;
				}

				GameManager::sharedState()->reportPercentageForLevel(main_level->m_levelID, main_level->m_normalPercent, false);
				GameManager::sharedState()->reportPercentageForLevel(main_level->m_levelID, main_level->m_practicePercent, true);

/*
				// update cf2 coin
				if (i == 20) {
					this->checkCoinCompletion(main_level);
				}
*/
				// no more context.
				main_level->m_levelType = GJLevelType::Local;
			}

			// custom level achievements
			if (auto level = GameLevelManager::sharedState()->getSavedLevel(30449); level != nullptr) {
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.clevel02", level->m_normalPercent, false);
			}

			// riot pack
			if (GameStatsManager::sharedState()->hasCompletedMapPack(186)) {
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.clevel01", 100, false);
			}

/*
			// cyclic pack
			if (GameStatsManager::sharedState()->hasCompletedMapPack(188)) {
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.clevel02", 100, false);
			}
*/

			AchievementManager::sharedState()->m_dontNotify = skipNotify;
		}
	}

	virtual void encodeDataTo(DS_Dictionary* data) override {
		GameManager::encodeDataTo(data);

		data->setIntegerForKey("binaryVersion", 26);
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("GameManager::colorForIdx", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for colorForIdx");
		}

		if (!self.setHookPriority("GameManager::colorForPos", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for colorForPos");
		}

		if (!self.setHookPriority("GameManager::isIconUnlocked", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for isIconUnlocked");
		}

		if (!self.setHookPriority("GameManager::isColorUnlocked", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for isColorUnlocked");
		}
	}
};

struct GlowGameLevelManager : geode::Modify<GlowGameLevelManager, GameLevelManager> {
	void updateUserScore() {
		auto originalSpecial = GameManager::sharedState()->m_playerStreak;

		auto glowEnabled = GameManager::sharedState()->getGameVariable(GameVariable::ICON_GLOW);
		GameManager::sharedState()->m_playerStreak = glowEnabled ? 2 : 0;

		GameLevelManager::updateUserScore();

		GameManager::sharedState()->m_playerStreak = originalSpecial;
	}
};

struct AchievementsGameStatsManager : geode::Modify<AchievementsGameStatsManager, GameStatsManager> {
	// 1 - jumps, 2 - attempts, 4 - user created level, 5 - demon, 6 - stars, 7 - mappacks, 8 - coins, 9 - destroyed player, 10 - liked levels, 11 - rated levels
	void checkAchievement(const char* name) {
		GameStatsManager::checkAchievement(name);

		auto int_name = atoi(name);
		switch (int_name) {
			case 1: {
				auto jumps = this->getStat("1");
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.jump05", std::min(static_cast<int>((jumps / 15'000.0f) * 100.0f), 100), false);
				break;
			}
			case 2: {
				auto attempts = this->getStat("2");

				auto gm = GameManager::sharedState();
				gm->reportAchievementWithID("geometry.ach.attempt05", std::min(static_cast<int>((attempts / 5'000.0f) * 100.0f), 100), false);
				gm->reportAchievementWithID("geometry.ach.attempt06", std::min(static_cast<int>((attempts / 1'000'000.0f) * 100.0f), 100), false);
				break;
			}
			case 4: {
				auto completed = this->getStat("4");
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.custom09", std::min(static_cast<int>((completed / 2'000.0f) * 100.0f), 100), false);
				break;
			}
			case 5: {
				auto demons = this->getStat("5");
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.demon11", std::min(static_cast<int>((demons / 50.0f) * 100.0f), 100), false);
				break;
			}
			case 6: {
				auto stars = this->getStat("6");
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.stars13", std::min(static_cast<int>((stars / 3000.0f) * 100.0f), 100), false);
				break;
			}
			case 7: {
				auto packs = this->getStat("7");

				auto gm = GameManager::sharedState();
				gm->reportAchievementWithID("geometry.ach.mappacks08", std::min(static_cast<int>((packs / 50.0f) * 100.0f), 100), false);
				gm->reportAchievementWithID("geometry.ach.mappacks09", std::min(static_cast<int>((packs / 100.0f) * 100.0f), 100), false);
				break;
			}
			case 10: {
				auto liked = this->getStat("10");
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.like04", std::min(static_cast<int>((liked / 500.0f) * 100.0f), 100), false);
				break;
			}
		}
	}

	void completedMapPack(GJMapPack* pack) {
		GameStatsManager::completedMapPack(pack);

		// it doesn't matter if we grant the achievement multiple times over
		switch (pack->m_packID) {
			// riot pack
			case 186:
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.clevel01", 100, false);
				break;
/*
			// cyclic pack
			case 188:
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.clevel02", 100, false);
				break;
*/
		}
	}
};

struct AchievementsGameManager : geode::Modify<AchievementsGameManager, GameManager> {
	void reportPercentageForLevel(int id, int percentage, bool practice) {
		GameManager::reportPercentageForLevel(id, percentage, practice);

		if (id < 19) {
			return;
		}

		if (practice) {
			switch (id) {
				case 19:
					GameManager::sharedState()->reportAchievementWithID("geometry.ach.level19a", percentage, false);
					break;
				case 20:
					GameManager::sharedState()->reportAchievementWithID("geometry.ach.level20a", percentage, false);
					break;
				case 21:
					GameManager::sharedState()->reportAchievementWithID("geometry.ach.level21a", percentage, false);
					break;
				case 22:
					GameManager::sharedState()->reportAchievementWithID("geometry.ach.level22a", percentage, false);
					break;
				case 23:
					GameManager::sharedState()->reportAchievementWithID("geometry.ach.level23a", percentage, false);
					break;
				default:
					break;
			}

			return;
		}

		switch (id) {
			case 19:
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.level19b", percentage, false);
				break;
			case 20:
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.level20b", percentage, false);
				break;
			case 21:
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.level21b", percentage, false);
				break;
			case 22:
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.level22b", percentage, false);
				break;
			case 23:
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.level23b", percentage, false);
				break;
			default:
				break;
		}
	}
};

struct AchievementsPlayLayer : geode::Modify<AchievementsPlayLayer, PlayLayer> {
	void reportSavedLevelForAchievement(int id, int percentage) {
		switch (id) {
			case 30449:
				GameManager::sharedState()->reportAchievementWithID("geometry.ach.clevel02", percentage, false);
				break;
			default:
				break;
		}
	}

	void destroyPlayer(PlayerObject* player) {
		PlayLayer::destroyPlayer(player);

		if (this->m_testMode || this->m_practiceMode) {
				return;
		}

		if (this->m_level->m_levelType == GJLevelType::Saved) {
			reportSavedLevelForAchievement(this->m_level->m_levelID, this->m_level->m_normalPercent);
		}
	}

	void levelComplete() {
		PlayLayer::levelComplete();

		if (this->m_testMode || this->m_practiceMode) {
			return;
		}

		if (this->m_level->m_levelType == GJLevelType::Saved) {
			auto level = this->m_level;
			reportSavedLevelForAchievement(level->m_levelID, level->m_normalPercent);
		}

/*
		if (this->m_level->m_levelType == GJLevelType::Local) {
			static_cast<CustomGameManager*>(GameManager::sharedState())->checkCoinCompletion(this->m_level);
		}
*/
	}

	static void onModify(auto& self) {
		// make sure the anticheat kicks in
		(void)self.setHookPriority("PlayLayer::destroyPlayer", geode::Priority::EarlyPost);
		(void)self.setHookPriority("PlayLayer::levelComplete", geode::Priority::EarlyPost);
	}
};

struct IconsMenuGameLayer : geode::Modify<IconsMenuGameLayer, MenuGameLayer> {
	void resetPlayer() {
		static auto dev = std::random_device();
		static auto rng = std::mt19937(dev());
		static std::uniform_real_distribution<> posDistribution{0, 5};

		m_playerObject->deactivateStreak();
		m_playerObject->deactivateParticle();

		auto posOffset = static_cast<float>(posDistribution(rng));
		auto playerPos = m_playerObject->getPosition();

		m_playerObject->setPosition({-100.0f - posOffset * 100.0f, playerPos.y});

		m_playerObject->resetAllParticles();
		m_playerObject->togglePlayerScale(false);

		auto& outfitManager = OutfitManager::get_manager();
		auto gm = GameManager::sharedState();

		std::uniform_int_distribution<> colorDistribution(1, outfitManager.max_colors());
		m_playerObject->setColor(gm->colorForIdx(colorDistribution(rng)));
		m_playerObject->setSecondColor(gm->colorForIdx(colorDistribution(rng)));

		std::uniform_int_distribution frameChoice{1, outfitManager.max_for_type(IconType::Cube)};
		m_playerObject->updatePlayerFrame(frameChoice(rng));

		static std::uniform_int_distribution<> iconTypeDistribution(0, 10);
		auto iconTypeChoice = iconTypeDistribution(rng);

		if (iconTypeChoice < 2 && !m_playerObject->m_flyMode) {
			m_playerObject->toggleFlyMode(true);
			std::uniform_int_distribution shipChoice{1, outfitManager.max_for_type(IconType::Ship)};
			m_playerObject->updatePlayerShipFrame(shipChoice(rng));

		} else if (iconTypeChoice < 4 && !m_playerObject->m_rollMode) {
			m_playerObject->toggleRollMode(true);
			std::uniform_int_distribution ballChoice{1, outfitManager.max_for_type(IconType::Ball)};
			m_playerObject->updatePlayerRollFrame(ballChoice(rng));

		} else if (iconTypeChoice < 6 && !m_playerObject->m_birdMode) {
			m_playerObject->toggleBirdMode(true);
			std::uniform_int_distribution birdChoice{1, outfitManager.max_for_type(IconType::Ufo)};
			m_playerObject->updatePlayerBirdFrame(birdChoice(rng));

		} else if (iconTypeChoice < 7 && !m_playerObject->m_dartMode) {
			m_playerObject->toggleDartMode(true);
			std::uniform_int_distribution waveChoice{1, outfitManager.max_for_type(IconType::Wave)};
			m_playerObject->updatePlayerDartFrame(waveChoice(rng));

		} else {
			m_playerObject->toggleFlyMode(false);
			m_playerObject->toggleRollMode(false);
			m_playerObject->toggleBirdMode(false);
			m_playerObject->toggleDartMode(false);
		}

		static std::uniform_int_distribution<> sizeDistribution(0, 10);
		m_playerObject->togglePlayerScale(sizeDistribution(rng) <= 1);
		m_playerObject->updateGlowColor();

		static std::uniform_int_distribution<> speedDistribution(0, 10);
		auto speedChoice = speedDistribution(rng);

		if (speedChoice < 2) {
			m_playerObject->updateTimeMod(1.3);
		} else if (speedChoice < 4) {
			m_playerObject->updateTimeMod(1.1);
		} else if (speedChoice < 6) {
			m_playerObject->updateTimeMod(0.7);
		} else {
			m_playerObject->updateTimeMod(0.9);
		}
	}

	static void onModify(auto& self) {
		(void)self.setHookPriority("MenuGameLayer::resetPlayer", geode::Priority::Replace);
	}
};

struct IconsAchievementBar : geode::Modify<IconsAchievementBar, AchievementBar> {
	bool init(char const* name, char const* desc, char const* icon) {
		auto iconSv = std::string_view(icon);
		auto useCustomIcon = iconSv.starts_with("special_");

		if (!AchievementBar::init(name, desc, useCustomIcon ? nullptr : icon)) {
			return false;
		}

		if (useCustomIcon) {
			iconSv.remove_prefix(8);
			auto id = geode::utils::numFromString<int>(iconSv)
				.unwrapOr(1);

			auto frameName = OutfitManager::get_manager().is_custom(IconType::Special, id)
				? fmt::format("player_special_{:02d}_001.png"_spr, id)
				: fmt::format("player_special_{:02d}_001.png", id);

			auto iconLayer = m_layerColor->getChildByType<cocos2d::CCSprite>(0);
			auto iconSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName.c_str());

			if (iconLayer && iconSprite) {
				iconSprite->setPosition(iconLayer->getPosition());
				m_layerColor->addChild(iconSprite, 4);

				iconLayer->setVisible(false);
			}
		}

		return true;
	}
};

struct IconsAchievementCell : geode::Modify<IconsAchievementCell, AchievementCell> {
	void loadFromDict(cocos2d::CCDictionary* dict) {
		AchievementCell::loadFromDict(dict);

		if (auto playerSprite = m_mainLayer->getChildByType<SimplePlayer>(0)) {
			auto icon = static_cast<cocos2d::CCString*>(dict->objectForKey("icon"));
			if (!icon) {
				return;
			}

			auto iconSv = std::string_view(icon->getCString());
			if (!iconSv.starts_with("special_")) {
				return;
			}

			iconSv.remove_prefix(8);
			auto id = geode::utils::numFromString<int>(iconSv)
				.unwrapOr(1);

			auto frameName = OutfitManager::get_manager().is_custom(IconType::Special, id)
				? fmt::format("player_special_{:02d}_001.png"_spr, id)
				: fmt::format("player_special_{:02d}_001.png", id);

			auto iconSprite = cocos2d::CCSprite::createWithSpriteFrameName(frameName.c_str());

			iconSprite->setPosition(playerSprite->getPosition());
			m_mainLayer->addChild(iconSprite, 4);

			playerSprite->setVisible(false);
		}
	}
};

$execute {
#if defined(GEODE_IS_ANDROID)
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1bbbee),
		{0xa4, 0xf0, 0xd3, 0xff}
	); // replaces player batchnode with node

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1bbc1c),
		{0xa4, 0xf0, 0xbc, 0xff}
	); // replaces glow batchnode with node

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1bbc1c),
		{0x00, 0xbf}
	); // removes glow batchnode blendfunc call

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1dfb5e),
		{0x81, 0xf0, 0x1b, 0xf8}
	); // replaces player batchnode with node
#elif defined(GEODE_IS_WINDOWS)
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0xe3c3f),
		{0xff, 0x15, 0x3c, 0xf5, 0x50, 0x00}
	); // replaces player batchnode with ccnode

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0xe3c76),
		{0xff, 0x15, 0x3c, 0xf5, 0x50, 0x00}
	); // replaces glow batchnode with ccnode

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0xe3c85),
		{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }
	); // patches out the setblendfunc call

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x8c46e),
		{0xff, 0x15, 0x3c, 0xf5, 0x50, 0x00}
	); // replaces leveleditorlayer player batchnode with ccnode

#else
#error Missing patches!
#endif
}
