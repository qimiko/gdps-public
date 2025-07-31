#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CCControlUtils.hpp>
#include <Geode/modify/DrawGridLayer.hpp>

#include "base/game_variables.hpp"

#include "classes/extensions/leveleditorlayerext.hpp"

struct YellowCCControlUtils : geode::Modify<YellowCCControlUtils, cocos2d::extension::CCControlUtils> {
	static cocos2d::extension::RGBA RGBfromHSV(cocos2d::extension::HSV c) {
		if (std::isnan(c.h))
			c.h = 0.0f;

		if (std::isnan(c.s))
			c.s = 0.0f;

		if (std::isnan(c.v))
			c.v = 0.0f;

		return cocos2d::extension::CCControlUtils::RGBfromHSV(c);
	}
};

struct CustomLevelEditorLayer : geode::Modify<CustomLevelEditorLayer, LevelEditorLayer> {
	void flipGravity(PlayerObject* acted, bool isFlipped, bool showEffect) {
		if (acted->m_gravityFlipped == isFlipped) {
			return;
		}

		acted->flipGravity(isFlipped, showEffect);

		if (!this->m_dualMode) {
			return;
		}

		auto player = this->m_player;
		auto secondPlayer = this->m_player2;

		auto inSameFly = player->m_flyMode == secondPlayer->m_flyMode;
		if (!inSameFly) {
			return;
		}

		auto inSameRoll = player->m_rollMode == secondPlayer->m_rollMode;
		if (!inSameRoll) {
			return;
		}

		// original game does birdmode == rollmode (this is an error)
		auto inSameBird = player->m_birdMode == secondPlayer->m_birdMode;
		if (!inSameBird) {
			return;
		}

		// players are in the same gamemode, flip gravity
		auto actedId = acted->m_ID;
		auto playerId = player->m_ID;

		auto otherPlayer = player;
		if (actedId == playerId) {
			otherPlayer = secondPlayer;
		}

		otherPlayer->flipGravity(!isFlipped, showEffect);
	}

	void updateHitboxView() {
		if (auto ext_obj = static_cast<LevelEditorLayerExt *>(this->getUserObject("hitboxes"_spr)); ext_obj != nullptr) {
			auto hitboxLayer = ext_obj->getHitboxLayer();

			auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
			auto gameLayer = this->m_gameLayer;

			auto gameZoom = 1 / gameLayer->getScale();

			// kinda just guessing this after looking at 2.1 in ghidra
			// the goal here is to offset the start/end pos based on the zoom
			// so that things offscreen aren't processed but everything onscreen are
			auto offsetFactor = winSize.width * gameZoom;
			auto offset = ((offsetFactor - winSize.width) * 0.5f) + 15.0f;

			auto startPos = (gameLayer->getPositionX() * -gameZoom) - offset;
			auto endPos = startPos + (offsetFactor + 30.0f);

			auto startSection = static_cast<int>(std::floor(startPos / 100.0f));
			auto endSection = static_cast<int>(std::ceil(endPos / 100.0f));

			auto levelSections = this->m_levelSections;
			auto sectionCount = levelSections->count();

			startSection = std::max(startSection, 0);
			endSection = std::min(endSection, static_cast<int>(sectionCount - 1));

			auto groupID = this->m_groupIDFilter;

			for (auto i = startSection; i <= endSection; i++) {
				auto sectionObjs = reinterpret_cast<cocos2d::CCArray*>(levelSections->objectAtIndex(i));
				auto objCount = sectionObjs->count();
				for (auto j = 0u; j < objCount; j++) {
					auto obj = reinterpret_cast<GameObject*>(sectionObjs->objectAtIndex(j));
					hitboxLayer->drawObject(obj, groupID);
				}
			}

			// show the player post playtest as well
			if (this->m_playerState > 0 || ext_obj->getHasShownPlayer()) {
				hitboxLayer->drawPlayer(this->m_player);

				if (this->m_dualMode) {
					hitboxLayer->drawPlayer(this->m_player2);
				}

				ext_obj->setHasShownPlayer(true);
			}
		}
	}

	void updateVisibility(float dt) {
		LevelEditorLayer::updateVisibility(dt);

		if (auto ext_obj = static_cast<LevelEditorLayerExt *>(this->getUserObject("hitboxes"_spr)); ext_obj != nullptr) {
			auto hitboxLayer = ext_obj->getHitboxLayer();
			hitboxLayer->beginUpdate();

			auto showHitboxes = GameManager::sharedState()->getGameVariable(GameVariable::SHOW_EDITOR_HITBOXES);
			if (!showHitboxes) {
				ext_obj->setHitboxesEnabled(false);
				return;
			}
			ext_obj->setHitboxesEnabled(true);

			this->updateHitboxView();
		}
	}

	void update(float dt) {
		LevelEditorLayer::update(dt);

		if (this->m_playerState > 0) {
			// increase update frequency if a player is active
			if (auto ext_obj = static_cast<LevelEditorLayerExt *>(this->getUserObject("hitboxes"_spr)); ext_obj != nullptr) {
				if (!ext_obj->getHitboxesEnabled()) {
					return;
				}

				auto hitboxLayer = ext_obj->getHitboxLayer();
				hitboxLayer->beginUpdate();

				this->updateHitboxView();
			}
		}
	}

	GameObject* addObjectFromString(gd::string objString) {
		auto object = LevelEditorLayer::addObjectFromString(objString);

		// just unconditionally add it here
		if (object && object->canRotateFree() && object->m_objectRadius <= 0.0f) {
			object->calculateOrientedBox();
		}

		return object;
	}

	GameObject* createObject(int key, cocos2d::CCPoint at) {
		auto object = LevelEditorLayer::createObject(key, at);

		if (object && object->canRotateFree() && object->m_objectRadius <= 0.0f) {
			object->calculateOrientedBox();
		}

		return object;
	}
};

struct FixDrawGridLayer : geode::Modify<FixDrawGridLayer, DrawGridLayer> {
	void loadTimeMarkers(gd::string markers) {
		DrawGridLayer::loadTimeMarkers(markers);

		// another bugfix, xPosForTime does not properly handle custom start speeds unless guidelines are set
		auto startSpeed = m_levelEditorLayer->m_levelSettings->m_startSpeed;

		switch (startSpeed) {
			case 0:
			default:
				this->m_guidelineSpacing = this->m_normalGuidelineSpacing;
				break;
			case 1:
				this->m_guidelineSpacing = this->m_slowGuidelineSpacing;
				break;
			case 2:
				this->m_guidelineSpacing = this->m_fastGuidelineSpacing;
				break;
			case 3:
				this->m_guidelineSpacing = this->m_fasterGuidelineSpacing;
				break;
		}
	}
};
