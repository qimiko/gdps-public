#include <unordered_map>

#include <Geode/Geode.hpp>

#include <Geode/modify/GameLevelManager.hpp>
#include <Geode/utils/web.hpp>

#include "base/config.hpp"
#include "base/game_variables.hpp"

namespace {
	std::string encodeUrlParam(std::string_view str) {
		std::ostringstream oss;
		oss.fill('0');
		oss << std::hex;

		for (auto c : str) {
			if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				oss << c;
				continue;
			}

			oss << std::uppercase << '%' << std::setw(2) << static_cast<int>(static_cast<std::uint8_t>(c)) << std::nouppercase;
		}

		return oss.str();
	}
}

// i couldn't figure out how to correctly call removeDelimiterChars so we can pretend this is a very smart optimization
std::string removeDelimiterChars2(std::string_view str, bool paramSet) {
	std::string s;
	s.reserve(str.length());

	if (paramSet) {
		std::copy_if(str.begin(), str.end(), std::back_inserter(s), [](const auto c){
			return c != '~' && c != '|' && c != '#';
		});
	} else {
		std::copy_if(str.begin(), str.end(), std::back_inserter(s), [](const auto c){
			return c != ':' && c != '|' && c != '#';
		});
	}

	return s;
}

using namespace geode::utils;

// this roughly matches the behavior of WebToolbox::getResponse
std::string getRequestBody(web::WebResponse* res) {
	if (!res->ok()) {
		geode::log::warn("web request failed: {}", res->code());
		return "-1";
	}

	auto body = res->string().unwrapOrDefault();
	if (body.empty()) {
		geode::log::warn("web request empty: {}", res->code());
		return "-1";
	}

	return body;
}

struct AsyncGameLevelManager : geode::Modify<AsyncGameLevelManager, GameLevelManager> {
	struct Fields {
		std::unordered_map<std::string, geode::EventListener<web::WebTask>> m_listeners;
	};

	void uploadLevel(GJGameLevel* level) {
		auto& listeners = m_fields->m_listeners;

		GameManager::sharedState()->reportAchievementWithID("geometry.ach.submit", 100, false);
		auto key = fmt::format("upload_{}", level->m_m_ID);

		if (listeners.contains(key)) {
			return;
		}

		auto ingameKey = std::to_string(level->m_m_ID);

		auto udid = GameManager::sharedState()->m_playerUDID;
		auto userName = removeDelimiterChars2(GameManager::sharedState()->m_playerName, false);
		auto levelName = removeDelimiterChars2(level->m_levelName, false);
		auto levelDesc = removeDelimiterChars2(level->m_levelDesc, false);
		auto accountId = GJAccountManager::sharedState()->m_accountID;

		auto params = fmt::format("udid={}&accountID={}&userName={}&levelID={}&levelName={}"
			"&levelDesc={}&levelVersion={}&levelLength={}&audioTrack={}&gameVersion=19&auto={:d}"
			"&password={}&original={}&twoPlayer={:d}&songID={}&objects={}",
			udid, accountId, userName, level->m_levelID, levelName,
			levelDesc, level->m_levelVersion, level->m_levelLength, level->m_audioTrack, level->m_autoLevel,
			level->m_password, level->m_originalLevel, level->m_twoPlayerMode, level->m_songID, level->m_objectCount 
		);

		auto extraString = level->getExtraString();
		if (!extraString.empty()) {
			params += "&extraString=";
			params += extraString;
		}

		// bold placement
		params += "&levelString=";
		params += level->m_levelString;

		params += "&levelReplay=";
		params += level->m_recordString;

		params += "&secret=Wmfd2893gb7";

		listeners[key].bind([this, key, ingameKey](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onUploadLevelCompleted(getRequestBody(res), ingameKey);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onUploadLevelCompleted("-1", ingameKey);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/uploadGJLevel19.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void getOnlineLevels(GJSearchObject* search) {
		auto& listeners = m_fields->m_listeners;

		std::string key = search->getKey();
		if (listeners.contains(key)) {
			return;
		}

		// this is what the game does, which is weird but whatever
		bool uncompletedFilter = search->m_uncompletedFilter;
		std::string completedLevelString;
		if (uncompletedFilter) {
			auto completedLevels = this->getCompletedLevels();
			if (completedLevels->count() > 0) {
				completedLevelString += "(";
				auto isFirstLevel = true;

				for (const auto& level : geode::cocos::CCArrayExt<GJGameLevel>(completedLevels)) {
					if (!isFirstLevel) {
						completedLevelString += ",";
					} else {
						isFirstLevel = false;
					}

					completedLevelString += std::to_string(level->m_levelID);
				}

				completedLevelString += ")";
			} else {
				uncompletedFilter = false;
			}
		}

		auto params = fmt::format("type={}&str={}&diff={}&len={}&page={}&total={}&uncompleted={:d}"
			"&featured={:d}&original={:d}&twoPlayer={:d}&gameVersion=19",
			static_cast<int>(search->m_searchType), search->m_searchQuery, search->m_difficulty, search->m_length,
			search->m_page, search->m_total, uncompletedFilter, search->m_featuredFilter, search->m_originalFilter,
			search->m_twoPlayerFilter
		);

		if (search->m_starFilter) {
			params += "&star=1";
		} else if (search->m_noStarFilter) {
			params += "&noStar=1";
		}

		if (search->m_songFilter != 0) {
			params += "&song=" + std::to_string(search->m_songID);
			if (search->m_customSongFilter) {
				params += "&customSong=1";
			}
		}

		if (!completedLevelString.empty()) {
			params += "&completedLevels=" + completedLevelString;
		}

		params += "&secret=Wmfd2893gb7";

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onGetOnlineLevelsCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onGetOnlineLevelsCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/getGJLevels19.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void getMapPacks(GJSearchObject* search) {
		auto& listeners = m_fields->m_listeners;

		std::string key = search->getKey();
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onGetMapPacksCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onGetMapPacksCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/getGJMapPacks.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("page={}&gameVersion=19&binaryVersion=25&secret=Wmfd2893gb7", search->m_page);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void downloadLevel(int levelId) {
		auto& listeners = m_fields->m_listeners;

		std::string key = this->getLevelKey(levelId);
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onDownloadLevelCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onDownloadLevelCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/downloadGJLevel19.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("levelID={}&inc={:d}&extras=0&secret=Wmfd2893gb7", levelId, this->hasDownloadedLevel(levelId));
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void updateLevel(GJGameLevel* level) {
		auto levelId = level->m_levelID;
		auto& listeners = m_fields->m_listeners;

		std::string key = this->getLevelKey(levelId);
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onUpdateLevelCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onUpdateLevelCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/downloadGJLevel19.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("levelID={}&gameVersion=19&secret=Wmfd2893gb7", levelId);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void rateLevel(int id, int rating) {
		GameManager::sharedState()->reportAchievementWithID("geometry.ach.rateDiff", 100, false);

		if (this->hasRatedLevel(id)) {
			return;
		}

		auto& listeners = m_fields->m_listeners;

		auto key = fmt::format("rate_{}", id);
		if (listeners.contains(key)) {
			return;
		}

		auto ingameKey = std::to_string(id);

		this->markLevelAsRated(id);

		listeners[key].bind([this, key, ingameKey](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onRateLevelCompleted(getRequestBody(res), ingameKey);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onRateLevelCompleted("-1", ingameKey);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/rateGJLevel.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("levelID={}&rating={}&secret=Wmfd2893gb7", id, rating);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void rateStars(int id, int rating) {
		if (this->hasRatedLevelStars(id)) {
			return;
		}

		auto& listeners = m_fields->m_listeners;

		auto key = fmt::format("rateStars_{}", id);
		if (listeners.contains(key)) {
			return;
		}

		auto ingameKey = std::to_string(id);

		this->markLevelAsRatedStars(id);

		listeners[key].bind([this, key, ingameKey](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onRateStarsCompleted(getRequestBody(res), ingameKey);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onRateStarsCompleted("-1", ingameKey);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/rateGJStars.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("levelID={}&rating={}&secret=Wmfd2893gb7", id, rating);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void deleteServerLevel(int id) {
		auto& listeners = m_fields->m_listeners;

		auto key = fmt::format("delete_{}", id);
		if (listeners.contains(key)) {
			return;
		}

		auto ingameKey = std::to_string(id);

		listeners[key].bind([this, key, ingameKey](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onDeleteServerLevelCompleted(getRequestBody(res), ingameKey);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onDeleteServerLevelCompleted("-1", ingameKey);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/deleteGJLevelUser19.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("udid={}&accountID={}&levelID={}&secret=Wmfv2898gc9",
			GameManager::sharedState()->m_playerUDID, GJAccountManager::sharedState()->m_accountID, id);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	// setLevelStars and setLevelFeatured are unused, no point in rewriting

	void updateUserScore() {
		auto& listeners = m_fields->m_listeners;

		std::string key = "user_score";
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				// nice one rob
				auto body = getRequestBody(res);
				if (body == "1") {
					// i don't think anyone with an id of 1 exists
					body = "-1";
				}

				this->onUpdateUserScoreCompleted(body, "Update user score");
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onUpdateUserScoreCompleted("-1", "Update user score");
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/updateGJUserScore19.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto gm = GameManager::sharedState();

		auto accountId = GJAccountManager::sharedState()->m_accountID;

		auto gsm = GameStatsManager::sharedState();
		auto stars = gsm->getStat("6");
		auto demons = gsm->getStat("5");
		auto coins = gsm->getStat("8");

		auto iconType = gm->m_playerIconType;
		auto playerIcon = gm->m_playerFrame;
		switch (iconType) {
			default:
			case IconType::Cube:
				break;
			case IconType::Ship:
				playerIcon = gm->m_playerShip;
				break;
			case IconType::Ball:
				playerIcon = gm->m_playerBall;
				break;
			case IconType::Ufo:
				playerIcon = gm->m_playerBird;
				break;
		}

		auto params = fmt::format("udid={}&accountID={}&userName={}&stars={}&demons={}&icon={}"
			"&color1={}&color2={}&iconType={}&coins={}&special={}&gameVersion=19&secret=Wmfd2893gb7",
		gm->m_playerUDID, accountId, gm->m_playerName, stars, demons, playerIcon,
		gm->m_playerColor, gm->m_playerColor2, static_cast<int>(iconType), coins, gm->m_playerStreak);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void getLeaderboardScores(const char* tag) {
		auto& listeners = m_fields->m_listeners;

		std::string key = tag;
		if (listeners.contains(key)) {
			return;
		}

		auto count = 100;
		auto url = GDMOD_ENDPOINT_BASE_URL "/getGJScores19.php";
		auto type = "top";

		if (key == "leaderboard_global") {
			count = 50;
			type = "relative";
		} else if (key == "leaderboard_creator") {
			url = GDMOD_ENDPOINT_BASE_URL "/getGJCreators19.php";
		} else if (key == "leaderboard_week") {
			type = "week";
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onGetLeaderboardScoresCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onGetLeaderboardScoresCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("type={}&udid={}&accountID={}&count={}&secret=Wmfd2893gb7",
			type, GameManager::sharedState()->m_playerUDID, GJAccountManager::sharedState()->m_accountID, count);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void getLevelComments(int id, int page, int total) {
		auto& listeners = m_fields->m_listeners;

		std::string key = this->getCommentKey(id, page);
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onGetLevelCommentsCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onGetLevelCommentsCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/getGJComments19.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("levelID={}&page={}&userID={}&total={}&secret=Wmfd2893gb7",
			id, page, GameManager::sharedState()->m_playerUserID, total);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void uploadComment(int id, gd::string comment) {
		auto& listeners = m_fields->m_listeners;

		std::string key = this->getPostCommentKey(id);
		if (listeners.contains(key)) {
			return;
		}

		// both keys are used, but the comment key is only used for same request checks
		std::string ingameKey = this->getCommentKey(id, 0);

		this->makeTimeStamp(key.c_str());

		listeners[key].bind([this, key, ingameKey](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onUploadCommentCompleted(getRequestBody(res), ingameKey);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onUploadCommentCompleted("-1", ingameKey);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/uploadGJComment19.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto udid = GameManager::sharedState()->m_playerUDID;
		auto accountId = GJAccountManager::sharedState()->m_accountID;
		auto userName = removeDelimiterChars2(GameManager::sharedState()->m_playerName, false);
		auto cleanComment = removeDelimiterChars2(comment, true);

		auto params = fmt::format("udid={}&accountID={}&userName={}&levelID={}&comment={}&secret=Wmfd2893gb7",
			udid, accountId, userName, id, cleanComment);

		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void deleteComment(int id, long comment) {
		auto& listeners = m_fields->m_listeners;

		std::string key = this->getDeleteCommentKey(id, comment);
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onDeleteCommentCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onDeleteCommentCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/deleteGJComment19.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto udid = GameManager::sharedState()->m_playerUDID;
		auto accountId = GJAccountManager::sharedState()->m_accountID;

		auto params = fmt::format("udid={}&accountID={}&commentID=%{}&levelID={}&secret=Wmfd2893gb7",
			udid, accountId, comment, id);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void likeItem(LikeItemType type, int id, bool liked) {
		if (type == LikeItemType::Level) {
			GameManager::sharedState()->reportAchievementWithID("geometry.ach.like", 100, false);
		}

		if (hasLikedItem(type, id)) {
			return;
		}

		markItemAsLiked(type, id, liked);

		auto& listeners = m_fields->m_listeners;

		std::string key = getLikeItemKey(type, id, liked);
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onLikeItemCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onLikeItemCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/likeGJItem.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto udid = GameManager::sharedState()->m_playerUDID;
		auto accountId = GJAccountManager::sharedState()->m_accountID;

		auto params = fmt::format("itemID={}&like={:d}&type={}&secret=Wmfd2893gb7", id, liked, static_cast<int>(type));
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void restoreItems() {
		auto& listeners = m_fields->m_listeners;

		std::string key = "restoreKey";
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onRestoreItemsCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onRestoreItemsCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/restoreGJItems.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("udid={}&secret=Wmfd2893gb7", GameManager::sharedState()->m_playerUDID);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	// submitUserInfo
	// this endpoint doesn't exist on 1.9 GDPS so whatever

	void reportLevel(int id) {
		auto& listeners = m_fields->m_listeners;

		auto key = fmt::format("report_{}", id);
		if (listeners.contains(key)) {
			return;
		}

		std::string ingameKey = std::to_string(id);

		listeners[key].bind([this, key, ingameKey](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onReportLevelCompleted(getRequestBody(res), ingameKey);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onReportLevelCompleted("-1", ingameKey);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/reportGJLevel.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("levelID={}&secret=Wmfd2893gb7", id);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void verifyMapPackUnlocks() {
		auto stored = this->getStoredOnlineLevels("verifyMapPacks");
		if (stored != nullptr) {
			this->doVerifyMapPacks(stored);
			return;
		}

		auto& listeners = m_fields->m_listeners;

		std::string key = "verifyMapPacks";
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onGetMapPacksCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onGetMapPacksCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/getGJMapPacks.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = "page=0&gameVersion=19&isVerify=1&secret=Wmfd2893gb7";
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("GameLevelManager::uploadLevel", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for uploadLevel");
		}

		if (!self.setHookPriority("GameLevelManager::getOnlineLevels", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for getOnlineLevels");
		}

		if (!self.setHookPriority("GameLevelManager::getMapPacks", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for getMapPacks");
		}

		if (!self.setHookPriority("GameLevelManager::downloadLevel", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for downloadLevel");
		}

		if (!self.setHookPriority("GameLevelManager::updateLevel", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for updateLevel");
		}

		if (!self.setHookPriority("GameLevelManager::rateLevel", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for rateLevel");
		}

		if (!self.setHookPriority("GameLevelManager::rateStars", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for rateStars");
		}

		if (!self.setHookPriority("GameLevelManager::deleteServerLevel", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for deleteServerLevel");
		}

		if (!self.setHookPriority("GameLevelManager::updateUserScore", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for updateUserScore");
		}

		if (!self.setHookPriority("GameLevelManager::getLeaderboardScores", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for getLeaderboardScores");
		}

		if (!self.setHookPriority("GameLevelManager::getLevelComments", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for getLevelComments");
		}

		if (!self.setHookPriority("GameLevelManager::uploadComment", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for getLevelComments");
		}

		if (!self.setHookPriority("GameLevelManager::deleteComment", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for getLevelComments");
		}

		if (!self.setHookPriority("GameLevelManager::likeItem", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for likeItem");
		}

		if (!self.setHookPriority("GameLevelManager::restoreItems", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for restoreItems");
		}

		if (!self.setHookPriority("GameLevelManager::reportLevel", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for reportLevel");
		}

		if (!self.setHookPriority("GameLevelManager::verifyMapPackUnlocks", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for verifyMapPackUnlocks");
		}
	}
};

#include <Geode/modify/MusicDownloadManager.hpp>

struct AsyncMusicDownloadManager : geode::Modify<AsyncMusicDownloadManager, MusicDownloadManager> {
	struct Fields {
		std::unordered_map<std::string, geode::EventListener<web::WebTask>> m_listeners;
	};

	void downloadSong(int id) {
		auto& listeners = m_fields->m_listeners;

		std::string key = this->getSongDownloadKey(id);
		if (listeners.contains(key)) {
			return;
		}

		std::string downloadUrl;
		auto songObject = this->getSongInfoObject(id);
		if (songObject != nullptr) {
			downloadUrl = songObject->m_songUrl;
			if (downloadUrl == "CUSTOMURL") {
				auto customSongId = id;
				if (customSongId > 10000000) {
					customSongId -= 10000000;
				}
				downloadUrl = fmt::format("https://geometrydashfiles.b-cdn.net/music/{}.ogg", customSongId);
			}
		} else {
			downloadUrl = fmt::format("http://www.newgrounds.com/audio/download/{}", id);
		}

		auto reqObj = geode::Ref(new cocos2d::extension::CCHttpRequest());
		reqObj->setShouldCancel(false);
		reqObj->setDownloadProgress(0);

		// this is probably the most complicated logic you'll get
		listeners[key].bind([this, id, key, reqObj](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				if (!res->ok()) {
					geode::log::warn("failed to download song: {}", res->code());
					this->removeDLFromActive(key.c_str());
					this->downloadSongFailed(id, GJSongError::Unknown);

					this->m_fields->m_listeners.erase(key);
					return;
				}

				this->removeDLFromActive(key.c_str());

				auto songPath = this->pathForSong(id);
				auto saveResult = res->into(std::string(songPath));

				if (!saveResult) {
					geode::log::warn("failed to write song: {}", saveResult.unwrapErr());
					this->downloadSongFailed(id, GJSongError::Unknown);
					this->m_fields->m_listeners.erase(key);
					return;
				}

				auto songObject = this->getSongInfoObject(id);
				if (songObject != nullptr) {
					songObject->m_priority = this->getSongPriority();
				}

				if (!GameManager::sharedState()->getGameVariable(GameVariable::NO_SONG_LIMIT)) {
					this->limitDownloadedSongs();
				}

				this->downloadSongFinished(songObject);
				this->m_fields->m_listeners.erase(key);
			} else if (auto progress = e->getProgress()) {
				if (reqObj->getShouldCancel()) {
					e->cancel();
				}

				if (auto pr = progress->downloadProgress(); pr.has_value()) {
					reqObj->setDownloadProgress(*pr);
				}
			} else if (e->isCancelled()) {
				this->removeDLFromActive(key.c_str());
				this->downloadSongFailed(id, GJSongError::Cancelled);

				this->m_fields->m_listeners.erase(key);
			}
		});

		auto req = web::WebRequest();
		// req.userAgent(Config::USER_AGENT);
		req.header("Cookie", "gd=1;");

		listeners[key].setFilter(req.get(downloadUrl));

		this->addDLToActive(key.c_str(), reqObj.data());
	}

	void getSongInfo(int id, bool force) {
		auto songObject = this->getSongInfoObject(id);
		if (songObject != nullptr && songObject->m_isVerified && !force) {
			this->loadSongInfoFinished(songObject);
			return;
		}

		auto& listeners = m_fields->m_listeners;

		std::string key = this->getSongInfoKey(id);
		if (listeners.contains(key)) {
			return;
		}

		auto ingameTag = std::to_string(id);

		listeners[key].bind([this, key, ingameTag](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onGetSongInfoCompleted(getRequestBody(res), ingameTag);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onGetSongInfoCompleted("-1", ingameTag);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto req = web::WebRequest();

		auto url = GDMOD_ENDPOINT_BASE_URL "/getGJSongInfo.php";
		if (id >= 20000000) {
			id -= 10000000;

			url = "https://www.boomlings.com/database/getGJSongInfo.php";
			req.header("Cookie", "gd=1;");
		} else {
			req.userAgent(Config::USER_AGENT);
		}


		auto params = fmt::format("songID={}&secret=Wmfd2893gb7", id);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("MusicDownloadManager::downloadSong", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for downloadSong");
		}

		if (!self.setHookPriority("MusicDownloadManager::getSongInfo", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for getSongInfo");
		}
	}
};

#include <Geode/modify/SongInfoObject.hpp>

struct AsyncSongInfoObject : geode::Modify<AsyncSongInfoObject, SongInfoObject> {
	static SongInfoObject* create(cocos2d::CCDictionary* dict) {
		auto obj = SongInfoObject::create(dict);
		if (obj->m_songUrl == "CUSTOMURL") {
			obj->m_songID += 10000000;
		}

		return obj;
	}
};

#include <Geode/modify/GJAccountManager.hpp>

struct AsyncGJAccountManager : geode::Modify<AsyncGJAccountManager, GJAccountManager> {
	struct Fields {
		std::unordered_map<std::string, geode::EventListener<web::WebTask>> m_listeners;
	};

	void backupAccount() {
		auto& listeners = m_fields->m_listeners;

		auto key = "bak_account";
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onBackupAccountCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onBackupAccountCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/accounts/backupGJAccount.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto udid = GameManager::sharedState()->m_playerUDID;

		auto params = fmt::format("userName={}&password={}&secret=Wmfv3899gc9", this->m_username, encodeUrlParam(this->m_password));

		params += "&saveData=";

		// clear the password for safety
		auto password = this->m_password;
		this->m_password = "";
		{

			auto gm = GameManager::sharedState();
			gm->m_quickSave = true;
			auto saveString = gm->getCompressedSaveString();
			gm->m_quickSave = false;

			params += saveString;
		}
		this->m_password = password;

		params += ";";

		{
			auto llm = LocalLevelManager::sharedState();
			llm->updateLevelOrder();
			auto llmString = llm->getCompressedSaveString();
			params += llmString;
		}

		params += "&secret=Wmfd2893gb7";

		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void syncAccount() {
		auto& listeners = m_fields->m_listeners;

		auto key = "sync_account";
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onSyncAccountCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onSyncAccountCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/accounts/syncGJAccount.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("userName={}&password={}&secret=Wmfv3899gc9", this->m_username, encodeUrlParam(this->m_password));
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void loginAccount(gd::string userName, gd::string password) {
		auto& listeners = m_fields->m_listeners;

		auto key = "login_account";
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onLoginAccountCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onLoginAccountCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/accounts/loginGJAccount.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto udid = GameManager::sharedState()->m_playerUDID;

		auto params = fmt::format("udid={}&userName={}&password={}&secret=Wmfv3899gc9", udid, userName, password);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	void registerAccount(gd::string userName, gd::string password, gd::string email) {
		auto& listeners = m_fields->m_listeners;

		auto key = "reg_account";
		if (listeners.contains(key)) {
			return;
		}

		listeners[key].bind([this, key](web::WebTask::Event* e) {
			if (auto res = e->getValue()) {
				this->onRegisterAccountCompleted(getRequestBody(res), key);
				this->m_fields->m_listeners.erase(key);
			} else if (e->isCancelled()) {
				this->onRegisterAccountCompleted("-1", key);
				this->m_fields->m_listeners.erase(key);
			}
		});

		auto url = GDMOD_ENDPOINT_BASE_URL "/accounts/registerGJAccount.php";

		auto req = web::WebRequest();
		req.userAgent(Config::USER_AGENT);

		auto params = fmt::format("userName={}&password={}&email={}&secret=Wmfv3899gc9", userName, password, email);
		req.bodyString(params);

		listeners[key].setFilter(req.post(url));
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("GJAccountManager::backupAccount", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for backupAccount");
		}

		if (!self.setHookPriority("GJAccountManager::syncAccount", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for syncAccount");
		}

		if (!self.setHookPriority("GJAccountManager::loginAccount", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for loginAccount");
		}

		if (!self.setHookPriority("GJAccountManager::registerAccount", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for registerAccount");
		}
	}
};
