#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include <ctime>

#include "base/game_variables.hpp"
#include "classes/managers/discordmanager.hpp"
#include "classes/presencelifecyclewatcher.hpp"

#define GUARD_PRESENCE_ENABLED if (GameManager::sharedState()->getGameVariable(GameVariable::DISABLE_RICH_PRESENCE))

struct PresencePlayLayer : geode::Modify<PresencePlayLayer, PlayLayer> {
	struct Fields {
		std::int64_t m_beginTimestamp{};
	};

	const char* keyForLevel() {
		if (m_level->m_autoLevel) {
			return "auto";
		}

		if (m_level->m_demon) {
			return "demon";
		}

		auto levelRating = m_level->m_levelType == GJLevelType::Local
			? m_level->m_difficulty
			: (m_level->m_ratingsSum / 10);

		switch (levelRating) {
			default:
			case 0:
				return "na";
			case 1:
				return "easy";
			case 2:
				return "normal";
			case 3:
				return "hard";
			case 4:
				return "harder";
			case 5:
				return "insane";
		}
	}

	const char* titleForLevel() {
		if (m_level->m_autoLevel) {
			return "Auto";
		}

		if (m_level->m_demon) {
			return "Demon";
		}

		auto levelRating = m_level->m_levelType == GJLevelType::Local
			? m_level->m_difficulty
			: (m_level->m_ratingsSum / 10);

		switch (levelRating) {
			default:
			case 0:
				return "NA";
			case 1:
				return "Easy";
			case 2:
				return "Normal";
			case 3:
				return "Hard";
			case 4:
				return "Harder";
			case 5:
				return "Insane";
		}
	}

	const char* statusForLevel() {
		if (m_level->m_levelType == GJLevelType::Local) {
			return "Official";
		}

		if (m_level->m_featured == 184594917) {
			return "Super";
		}

		if (m_level->m_featured >= 1) {
			return "Featured";
		}

		if (m_level->m_stars > 0) {
			return "Rated";
		}

		return "Unrated";
	}

	void generateActivity() {
		DiscordRichPresence presence{};

		auto userName = m_level->m_levelType == GJLevelType::Saved ? fmt::format(" by {}", m_level->m_userName) : "";
		auto levelState = fmt::format("{}{}", m_level->m_levelName, userName);
		auto levelRating = fmt::format("{}* {} ({})", m_level->m_stars, titleForLevel(), statusForLevel());

		if (m_level->m_levelType == GJLevelType::Local || m_level->m_levelType == GJLevelType::Saved) {
			presence.details = levelState.c_str();
			presence.state = "Playing a level";
			presence.smallImageText = levelRating.c_str();
			presence.smallImageKey = keyForLevel();
			presence.swapDisplayType = true;
		} else {
			presence.state = "Playtesting a level";
		}

		presence.startTimestamp = m_fields->m_beginTimestamp;

		DiscordManager::get().send(presence);
	}

	bool init(GJGameLevel* level) {
		if (!PlayLayer::init(level)) {
			return true;
		}

		m_fields->m_beginTimestamp = std::time(nullptr);

		GUARD_PRESENCE_ENABLED {
			return true;
		}

		auto lifecycle = PresenceLifecycleWatcher::create();
		this->addChild(lifecycle);

		lifecycle->begin();

		this->generateActivity();

		return true;
	}

/*
	void showNewBest() {
		PlayLayer::showNewBest();

		GUARD_PRESENCE_ENABLED {
			return;
		}

		this->generateActivity();
	}
*/
};

struct PresenceLevelEditorLayer : geode::Modify<PresenceLevelEditorLayer, LevelEditorLayer> {
	struct Fields {
		std::int64_t m_beginTimestamp{};
		std::int32_t m_lastObjectCount{};
	};

	void generateActivity() {
		DiscordRichPresence presence{};

		auto levelState = fmt::format("{} objects", m_objectCount);

		presence.state = "Editing a level";
		presence.details = levelState.c_str();
		presence.startTimestamp = m_fields->m_beginTimestamp;

		m_fields->m_lastObjectCount = m_objectCount;

		DiscordManager::get().send(presence);
	}

	void updateActivity(float dt) {
		if (m_fields->m_lastObjectCount == m_objectCount) {
			return;
		}

		this->generateActivity();
	}

	bool init(GJGameLevel* level) {
		if (!LevelEditorLayer::init(level)) {
			return true;
		}

		m_fields->m_beginTimestamp = std::time(nullptr);

		GUARD_PRESENCE_ENABLED {
			return true;
		}

		auto lifecycle = PresenceLifecycleWatcher::create();
		this->addChild(lifecycle);

		lifecycle->begin();

		this->generateActivity();

		// minimum period between updates is 15 seconds, so might as well
		this->schedule(static_cast<cocos2d::SEL_SCHEDULE>(&PresenceLevelEditorLayer::updateActivity), 15.0f);

		return true;
	}
};

struct PresenceMenuLayer : geode::Modify<PresenceMenuLayer, MenuLayer> {
	virtual bool init() override {
		if (!MenuLayer::init()) {
			return false;
		}

		GUARD_PRESENCE_ENABLED {
			return true;
		}

		DiscordManager::get().decrementIdleCounter();

		return true;
	}
};

