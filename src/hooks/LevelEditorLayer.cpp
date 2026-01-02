#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#include "classes/managers/speedhackmanager.hpp"
#include "classes/editorhitboxlayer.hpp"

#include "base/game_variables.hpp"


#if GDMOD_ENABLE_LOGGING
#include <imgui.h>
#endif

struct CustomLevelEditorLayer : geode::Modify<CustomLevelEditorLayer, LevelEditorLayer> {
	struct Fields {
		bool m_hitboxesEnabled{false};
    bool m_hasShownPlayer{false};
    EditorHitboxLayer* m_hitboxLayer{nullptr};
	};

	void flipGravity(PlayerObject* acted, bool isFlipped, bool showEffect) {
		if (acted->m_gravityFlipped == isFlipped) {
			return;
		}

		acted->flipGravity(isFlipped, showEffect);

		if (!this->m_dualMode) {
			return;
		}

		if (this->m_levelSettings->m_twoPlayerMode) {
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
		auto fields = m_fields.self();
		auto hitboxLayer = fields->m_hitboxLayer;

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
		if (this->m_playerState > 0 || fields->m_hasShownPlayer) {
			hitboxLayer->drawPlayer(this->m_player);

			if (this->m_dualMode) {
				hitboxLayer->drawPlayer(this->m_player2);
			}

			fields->m_hasShownPlayer = true;
		}

	}

	void updateVisibility(float dt) {
		LevelEditorLayer::updateVisibility(dt);

		auto fields = m_fields.self();
		auto hitboxLayer = fields->m_hitboxLayer;
		hitboxLayer->beginUpdate();

		auto showHitboxes = GameManager::sharedState()->getGameVariable(GameVariable::SHOW_EDITOR_HITBOXES);
		if (!showHitboxes) {
			fields->m_hitboxesEnabled = false;
			return;
		}

		fields->m_hitboxesEnabled = true;
		this->updateHitboxView();
	}

	void update(float dt) {
		LevelEditorLayer::update(dt);

		if (this->m_playerState > 0) {
			// increase update frequency if a player is active
			auto fields = m_fields.self();
			if (!fields->m_hitboxesEnabled) {
				return;
			}

			auto hitboxLayer = fields->m_hitboxLayer;
			hitboxLayer->beginUpdate();

			this->updateHitboxView();
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

	void updateOrientedHitboxes() {
		for (auto section : geode::cocos::CCArrayExt<cocos2d::CCArray>(m_levelSections)) {
			for (auto object : geode::cocos::CCArrayExt<GameObject>(section)) {
				if (object->m_oriented) {
					object->updateOrientedBox();
				}
			}
		}
	}

	void onPlaytest() {
		SpeedhackManager::get().setPlaytestActive(true);
		LevelEditorLayer::onPlaytest();

		this->updateOrientedHitboxes();
	}

	void onResumePlaytest() {
		SpeedhackManager::get().setPlaytestActive(true);
		LevelEditorLayer::onResumePlaytest();

		this->updateOrientedHitboxes();
	}

	void onPausePlaytest() {
		SpeedhackManager::get().setPlaytestActive(false);
		LevelEditorLayer::onPausePlaytest();
	}

	void onStopPlaytest() {
		SpeedhackManager::get().setPlaytestActive(false);
		LevelEditorLayer::onStopPlaytest();
	}

#if GDMOD_ENABLE_LOGGING
	void updateOverlay(float) {
		// when saving and playing, the game clears all objects in the level
		// however, in this case, it doesn't update the selection which can cause this to crash
		if (this->m_objectCount == 0) {
			return;
		}

		auto uiLayer = this->m_uiLayer;
		if (uiLayer == nullptr) {
			return;
		}

		auto selection = uiLayer->m_selectedObjects;
		if (selection != nullptr && selection->count() == 1)
		{
			if (ImGui::Begin("Selection", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				auto first_obj = reinterpret_cast<GameObject *>(selection->objectAtIndex(0));
				if (first_obj == nullptr) {
					return;
				}

				ImGui::Text("obj: %i", first_obj->m_objectID);
				ImGui::Text("position: (%.2f, %.2f)", first_obj->getPositionX(), first_obj->getPositionY());
				ImGui::Text("type: %i, dmg %i", static_cast<uint32_t>(first_obj->m_objectType), first_obj->m_hazardousSlope);

				if (first_obj->m_objectType == GameObjectType::Slope) {
					ImGui::Separator();
					ImGui::Text("slope_type %i, angle %.2f", first_obj->m_slopeType, first_obj->m_slopeAngle);
					ImGui::Text("floorTop %i, wallLeft %i", first_obj->slopeFloorTop(), first_obj->slopeWallLeft());
				}
			}
			ImGui::End();
		}
	}
#endif

	bool init(GJGameLevel* lvl) {
		if (LevelEditorLayer::init(lvl)) {
			auto& speedhackManager = SpeedhackManager::get();

			auto speedhack_interval = speedhackManager.getSpeedhackInterval();
			speedhackManager.setSpeedhackValue(speedhack_interval);

			auto hitboxNode = EditorHitboxLayer::create();
			this->m_gameLayer->addChild(hitboxNode, 9);

			m_fields->m_hitboxLayer = hitboxNode;

#if GDMOD_ENABLE_LOGGING
			this->schedule(static_cast<cocos2d::SEL_SCHEDULE>(&CustomLevelEditorLayer::updateOverlay));
#endif
			return true;
		}

		return false;
	}

};
