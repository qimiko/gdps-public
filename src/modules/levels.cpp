#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LocalLevelManager.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/GameStatsManager.hpp>

#include <array>

struct CustomLocalLevelManager : geode::Modify<CustomLocalLevelManager, LocalLevelManager> {
	gd::string getMainLevelString(int id) {
		if (id < 19) {
			return LocalLevelManager::getMainLevelString(id);
		}

		auto filename = fmt::format("level_{}"_spr, id);

		auto res = geode::utils::file::readStringFromResources(filename);
		if (!res) {
			geode::log::warn("Failed to read main level: {}", res.unwrapErr());
		}

		return res.unwrap();
	}
};

struct CustomLevelTools : geode::Modify<CustomLevelTools, LevelTools> {
	static gd::string getAudioTitle(int audio_id) {
		switch (audio_id) {
		case 18:
			return "Thumper";
		case 19:
			return "Clutterfunk 2";
		case 20:
			return "Aura";
		case 21:
			return "Jack Russel";
		case 22:
			return "Streetwise";
		default:
			return LevelTools::getAudioTitle(audio_id);
		}
	}

	static int artistForAudio(int audio_id) {
		/* 0 - DJVI
		* 1 - Waterflame
		* 2 - OcularNebula
		* 3 - ForeverBound
		* 5 - DJ-Nate
		* 6 - Creo
		* 7 - Bossfight
		* */

		if (audio_id == -1) {
			return 2;
		}

		constexpr std::array<int, 23> audioArtists = {
				3, 0, 4, 0, 0, 0, 1, 1, 0, 0, 1, 5, 1, 5, 5, 1, 1, 5, 1, 1, 6, 7, 1
		};

		if (static_cast<std::size_t>(audio_id) > audioArtists.size()) {
			return 0;
		}

		return audioArtists.at(audio_id);
	}

	static gd::string urlForAudio(int audio_id) {
		switch (audio_id) {
		case 18:
			return "https://www.youtube.com/watch?v=Rge_r3Eri0Q";
		case 19:
			return "https://www.youtube.com/watch?v=YFrimAIrU8I";
		case 20:
			return "https://www.youtube.com/watch?v=LXTrXaYMgYs";
		case 21:
			return "https://www.youtube.com/watch?v=PteZ2d3UeTo";
		case 22:
			return "https://www.youtube.com/watch?v=yZHWw6ojMUw";
		default:
			return LevelTools::urlForAudio(audio_id);
		}
	}

	static gd::string nameForArtist(int artist_id) {
		switch (artist_id) {
		case 6:
			return "Creo";
		case 7:
			return "Bossfight";
		default:
			return LevelTools::nameForArtist(artist_id);
		}
	}

	static gd::string ngURLForArtist(int artist_id) {
		switch (artist_id) {
		case 6:
			return "https://creomusic.newgrounds.com";
		case 7:
			return "https://bossfightofficial.newgrounds.com";
		default:
			return LevelTools::ngURLForArtist(artist_id);
		}
	}

	static gd::string ytURLForArtist(int artist_id) {
		switch (artist_id) {
		case 6:
			return "http://www.youtube.com/creomusic";
		case 7:
			return "https://www.youtube.com/c/Bossfightmusic";
		default:
			return LevelTools::ytURLForArtist(artist_id);
		}
	}

	static gd::string getAudioFileName(int audio_id) {
		switch (audio_id) {
		case 18:
			return "Thumper.mp3"_spr;
		case 19:
			return "Clutterfunk2.mp3"_spr;
		case 20:
			return "Aura.mp3"_spr;
		case 21:
			return "JackRussel.mp3"_spr;
		case 22:
			return "Streetwise.mp3"_spr;
		default:
			return LevelTools::getAudioFileName(audio_id);
		}
	}

	static GJGameLevel* getLevel(int level_id, bool p1) {
		switch (level_id) {
		case 19: { // getlevel ids are +1 the song id (for some odd reason)
			// thumper (which thinks that it's stereo madness)
			auto level = GJGameLevel::create();
			auto song_id = level_id - 1;

			level->m_levelID = level_id;

			level->m_levelName = LevelTools::getAudioTitle(song_id);
			level->m_audioTrack = song_id;

			level->m_stars = 10;
			level->m_difficulty = 4;

			// we'll use this if we're making our own gamelevel from scratch
			level->m_levelType = GJLevelType::Local;
			level->m_coins = 3;

			if (!p1) {
				LocalLevelManager* llm = LocalLevelManager::sharedState();
				auto level_string = llm->getMainLevelString(level_id);

				level->m_levelString = level_string;
			}

			return level;
		}
		case 20: {
			// clutterfunk 2
			auto level = GJGameLevel::create();
			auto song_id = level_id - 1;

			level->m_levelID = level_id;

			level->m_demon = true;

			level->m_levelName = LevelTools::getAudioTitle(song_id);
			level->m_audioTrack = song_id;

			level->m_stars = 16;
			level->m_difficulty = 6;

			level->m_levelType = GJLevelType::Local;
			level->m_coins = 3;

			if (!p1) {
				LocalLevelManager* llm = LocalLevelManager::sharedState();
				auto level_string = llm->getMainLevelString(level_id);

				level->m_levelString = level_string;
			}

			return level;
		}
		case 21: {
			// aura
			auto level = GJGameLevel::create();
			auto song_id = level_id - 1;

			level->m_levelID = level_id;

			level->m_levelName = LevelTools::getAudioTitle(song_id);
			level->m_audioTrack = song_id;

			level->m_stars = 11;
			level->m_difficulty = 5;

			level->m_levelType = GJLevelType::Local;
			level->m_coins = 3;

			if (!p1) {
				LocalLevelManager* llm = LocalLevelManager::sharedState();
				auto level_string = llm->getMainLevelString(level_id);

				level->m_levelString = level_string;
			}

			return level;
		}
		case 22: {
			// jack russel
			auto level = GJGameLevel::create();
			auto song_id = level_id - 1;

			level->m_levelID = level_id;

			level->m_levelName = LevelTools::getAudioTitle(song_id);
			level->m_audioTrack = song_id;

			level->m_stars = 12;
			level->m_difficulty = 5;

			level->m_levelType = GJLevelType::Local;
			level->m_coins = 3;

			if (!p1) {
				LocalLevelManager* llm = LocalLevelManager::sharedState();
				auto level_string = llm->getMainLevelString(level_id);

				level->m_levelString = level_string;
			}

			return level;
		}
		case 23: {
			// streetwise
			auto level = GJGameLevel::create();
			auto song_id = level_id - 1;

			level->m_levelID = level_id;

			level->m_levelName = LevelTools::getAudioTitle(song_id);
			level->m_audioTrack = song_id;

			level->m_stars = 12;
			level->m_difficulty = 5;

			level->m_levelType = GJLevelType::Local;
			level->m_coins = 3;

			if (!p1) {
				LocalLevelManager* llm = LocalLevelManager::sharedState();
				auto level_string = llm->getMainLevelString(level_id);

				level->m_levelString = level_string;
			}

			return level;
		}
		default:
			return LevelTools::getLevel(level_id, p1);
		}
	}
};

struct CopyLevelInfoLayer : geode::Modify<CopyLevelInfoLayer, LevelInfoLayer> {
	// stole this from mat btw (hi mat)
	void onClone(cocos2d::CCObject* target) {
		// this value gets set after onclone is run
		auto run_guard = this->m_willExitScene;
		LevelInfoLayer::onClone(target);

		if (!this->shouldDownloadLevel() && !run_guard) {
			auto level = this->m_level;
			auto new_level = static_cast<GJGameLevel*>(
				LocalLevelManager::sharedState()->m_localLevels->objectAtIndex(0));
			new_level->m_songID = level->m_songID;
		}
	}
};

struct CopyEditLevelLayer : geode::Modify<CopyEditLevelLayer, EditLevelLayer> {
	void onClone() {
		auto run_guard = this->m_isLoadingLevel;
		EditLevelLayer::onClone();

		if (!run_guard) {
			auto level = this->m_level;
			auto new_level = static_cast<GJGameLevel*>(
				LocalLevelManager::sharedState()->m_localLevels->objectAtIndex(0)
			);
			new_level->m_songID = level->m_songID;
		}
	}
};

namespace {
constexpr uint8_t max_lvls = 23;
constexpr uint8_t max_songs = 23;

constexpr uint8_t max_lvl_p1 = max_lvls + 1;
constexpr uint8_t max_song_m1 = max_songs - 1;
}

struct CustomLevelSelectLayer : geode::Modify<CustomLevelSelectLayer, LevelSelectLayer> {
	// thanks capeling: https://github.com/Capeling/rewritten-layers/blob/de93a7dda40c52b025bddaf41254e5e0d90bf882/src/LevelSelectLayer.cpp#L202
	virtual void scrollLayerMoved(cocos2d::CCPoint point) override {
		float x = -point.x / this->m_width;
		while (x < 0.0f) {
			x += max_lvl_p1;
		}

		auto floorX = static_cast<int>(std::floor(x));
		m_scrolls = floorX;

		auto x2 = floorX % max_lvl_p1 + 1;
		if (x2 == max_lvl_p1) {
			x2 = 0;
		}

		cocos2d::ccColor3B color;
		color = getColorValue(floorX % max_lvl_p1, x2, x - floorX);

		m_backgroundSprite->setColor(color);
		m_groundSprite->setColor({
			static_cast<GLubyte>(color.r * 0.8),
			static_cast<GLubyte>(color.g * 0.8),
			static_cast<GLubyte>(color.b * 0.8)
		});
	}
};

struct LevelsGameStatsManager : geode::Modify<LevelsGameStatsManager, GameStatsManager> {
	bool isUniqueValid(gd::string item) {
		if (!GameStatsManager::isUniqueValid(item)) {
			std::string_view itemStr = item;
			if (!itemStr.starts_with("unique_")) {
				return false;
			}

			auto noPrefix = itemStr.substr(7); // strip "unique_"
			if (noPrefix == "gnome02") {
				return true;
			}

			// main level check
			auto sepPos = noPrefix.find('_');
			if (sepPos == std::string_view::npos) {
				return false;
			}

			auto first = noPrefix.substr(0, sepPos);
			auto second = noPrefix.substr(sepPos + 1);

			auto levelId = geode::utils::numFromString<int>(first).unwrapOrDefault();
			auto coinNumber = geode::utils::numFromString<int>(second).unwrapOrDefault();

			if (levelId < 19 || levelId > max_lvls) {
				return false;
			}

			if (coinNumber < 1 || coinNumber > 3) {
				return false;
			}

			return true;
		}

		return true;
	}
};

$execute {
	// TODO: some of these patches could be ported to proper hooks now

#if defined(GEODE_IS_ANDROID)
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x20D558),
		{max_lvl_p1}
	); // cmp i, 0x13

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1be706),
		{max_lvl_p1}
	); // cmp i, 0x13

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1be532),
		{max_lvl_p1}
	); // cmp i, 0x13

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1F261C),
		{max_songs}
	); // cmp audio_id, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1F25B8),
		{max_songs}
	); // cmp audio_id, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1F25BE),
		{max_songs}
	); // mov.ge song_id, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x219118),
		{max_songs}
	); // cmp tmp, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1DE7A0),
		{max_song_m1}
	); // cmp audio_id, 0x11

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1DE724),
		{max_song_m1}
	); // cmp audio_id, 0x11

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1DE72A),
		{max_song_m1}
	); // mov.ge audio_id, 0x11
#elif defined(GEODE_IS_WINDOWS)
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0xa6158),
		{max_lvls}
	); // cmp esi, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x68635),
		{max_lvls}
	); // cmp esi, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x68201),
		{max_lvls}
	); // cmp edi, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0xa4dbd),
		{max_songs}
	); // cmp eax, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0xa4eb1),
		{max_songs}
	); // mov eax, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0xfca35),
		{max_songs}
	); // cmp edi, 0x12

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x9a37e),
		{max_song_m1}
	); // cmp eax, 0x11

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x9a38b),
		{max_song_m1}
	); // mov eax, 0x11
#else
#error Unsupported platform
#endif
}
