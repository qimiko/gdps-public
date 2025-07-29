#include <Geode/Geode.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

#if GDMOD_ENABLE_LOGGING
#include <imgui.h>
#endif

#include "base/game_variables.hpp"

#include "classes/speedhack/speedhackmanagercard.hpp"
#include "classes/speedhack/speedhacklifecyclewatcher.hpp"
#include "classes/extensions/leveleditorlayerext.hpp"
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

struct SpeedhackEditorPauseLayer : geode::Modify<SpeedhackEditorPauseLayer, EditorPauseLayer> {
	void onShowHitboxes(cocos2d::CCObject*) {
		GameManager::sharedState()->toggleGameVariable(GameVariable::SHOW_EDITOR_HITBOXES);
	}

	void onShowInfoLabel(cocos2d::CCObject*) {
		GameManager::sharedState()->toggleGameVariable(GameVariable::SHOW_INFO_LABEL);

		if (!GameManager::sharedState()->getGameVariable(GameVariable::SHOW_INFO_LABEL)) {
			if (auto infoLabel = m_levelEditorLayer->m_uiLayer->getChildByID("info-label"_spr); infoLabel != nullptr) {
				infoLabel->setVisible(false);
			}
		}
	}

	void customSetup() {
		auto director = cocos2d::CCDirector::sharedDirector();

		EditorPauseLayer::customSetup();

		auto main_menu = reinterpret_cast<cocos2d::CCMenu*>(this->getChildren()->objectAtIndex(1));

		auto show_hitboxes = GameManager::sharedState()->getGameVariable(GameVariable::SHOW_EDITOR_HITBOXES);
		GameToolbox::createToggleButton(
			"Show Hitboxes",
			static_cast<cocos2d::SEL_MenuHandler>(&SpeedhackEditorPauseLayer::onShowHitboxes),
			show_hitboxes, main_menu,
			{ 30.0f, director->getScreenBottom() + 120.0f },
			this, this,
			0.7f, 0.4f, 80.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false);

		GameToolbox::createToggleButton(
			"Show Info Label",
			static_cast<cocos2d::SEL_MenuHandler>(&SpeedhackEditorPauseLayer::onShowInfoLabel),
			GameManager::sharedState()->getGameVariable(GameVariable::SHOW_INFO_LABEL),
			main_menu,
			{30.0f, director->getScreenBottom() + 150.0f},
			this, this, 0.7f, 0.4f, 80.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false
		);

		auto screen_right = director->getScreenRight();
		auto screen_top = director->getScreenTop();

		auto card = SpeedhackManagerCard::create();
		this->addChild(card);
		card->setPosition({screen_right - 50.0f, screen_top - 50.0f});
	}

	std::string formatTimeToString(float seconds) {
		auto levelLength = "Tiny";
		auto intSeconds = static_cast<int>(seconds);

		if (intSeconds >= 120) {
			levelLength = "Extra-Long";
		} else if (intSeconds > 60) {
			levelLength = "Long";
		} else if (intSeconds > 30) {
			levelLength = "Medium";
		} else if (intSeconds > 10) {
			levelLength = "Short";
		}

		auto mins = std::div(intSeconds, 60);
		if (mins.quot == 0) {
			return fmt::format("{:.1f}s ({})", seconds, levelLength);
		}

		auto hours = std::div(mins.quot, 60);
		if (hours.quot == 0) {
			return fmt::format("{}m {}s ({})", mins.quot, mins.rem, levelLength);
		}

		return fmt::format("{}h {}m {}s ({})", hours.quot, hours.rem, mins.rem, levelLength);
	}

	bool init(LevelEditorLayer* editor) {
		if (!EditorPauseLayer::init(editor)) {
			return false;
		}

		auto levelTime = editor->m_gridLayer->timeForXPos(editor->m_levelDistance);

		auto director = cocos2d::CCDirector::sharedDirector();
		auto screen_left = director->getScreenLeft();
		auto screen_top = director->getScreenTop();

		auto timeString = formatTimeToString(levelTime);
		auto timeLabel = cocos2d::CCLabelBMFont::create(timeString.c_str(), "goldFont.fnt");
		timeLabel->setAnchorPoint({0.0f, 1.0f});
		this->addChild(timeLabel);

		timeLabel->setPosition({screen_left + 10.0f, screen_top - 20.0f});
		timeLabel->setScale(0.5f);

		return true;
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
