#include <Geode/Geode.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#if GDMOD_ENABLE_LOGGING
#include <imgui.h>
#endif

#include "classes/speedhack/speedhackmanagercard.hpp"
#include "classes/speedhack/speedhacklifecyclewatcher.hpp"
#include "classes/extensions/leveleditorlayerext.hpp"
#include "classes/extensions/playlayerext.hpp"
#include "classes/editorhitboxlayer.hpp"

struct SpeedhackFMODAudioEngine : geode::Modify<SpeedhackFMODAudioEngine, FMODAudioEngine> {
	void update(float dt) {
		FMODAudioEngine::update(dt);
		auto control = this->m_globalChannel;

		if (SpeedhackManagerCard::isSpeedhackActive()) {
			auto interval = SpeedhackManagerCard::getSpeedhackInterval();
			control->setPitch(interval);

			return;
		}

		// will need to determine if this is necessary
		control->setPitch(1.0f);
	}
};

struct SpeedhackPlayLayer : geode::Modify<SpeedhackPlayLayer, PlayLayer> {
	void resetLevel() {
		PlayLayer::resetLevel();

		if (auto pl_ext = geode::cast::typeinfo_cast<PlayLayerExt*>(this->getUserObject("hacks"_spr)); pl_ext != nullptr) {
			if (!this->m_practiceMode) {
				pl_ext->resetCheats();
			}

			// clear hitboxes
			if (auto hitboxes = pl_ext->getHitboxLayer(); hitboxes != nullptr) {
				hitboxes->beginUpdate();
				hitboxes->setVisible(false);
			}
		}
	}
};

struct SpeedhackLevelEditorLayer : geode::Modify<SpeedhackLevelEditorLayer, LevelEditorLayer> {
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
			auto speedhack_interval = SpeedhackManagerCard::getSpeedhackInterval();

			auto lifecycle = SpeedhackLifecycleWatcher::create();
			this->addChild(lifecycle);

			lifecycle->beginSpeedhack();

			SpeedhackManagerCard::setSpeedhackValue(speedhack_interval);

			auto ext_object = new LevelEditorLayerExt();
			this->setUserObject("hitboxes"_spr, ext_object);

			auto hitboxNode = EditorHitboxLayer::create();
			this->m_gameLayer->addChild(hitboxNode, 9);

			ext_object->setHitboxLayer(hitboxNode);

#if GDMOD_ENABLE_LOGGING
			this->schedule(static_cast<cocos2d::SEL_SCHEDULE>(&SpeedhackLevelEditorLayer::updateOverlay));
#endif
			return true;
		}

		return false;
	}
};
