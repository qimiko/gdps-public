#include "hooks/EditorUI.hpp"
#include "hooks/ColorSelectPopup.hpp"

#include "base/game_variables.hpp"

#include <fmt/format.h>

namespace {
	const char* colorModeInfo(GJCustomColorMode color, GJCustomColorMode defaultColor) {
		switch (color) {
			case GJCustomColorMode::Default:
				if (defaultColor == GJCustomColorMode::Default) {
					return "P-Col 1";
				} else {
					return colorModeInfo(defaultColor, defaultColor);
				}
			case GJCustomColorMode::PCol1:
				return "P-Col 1";
			case GJCustomColorMode::PCol2:
				return "P-Col 2";
			case GJCustomColorMode::Col1:
				return "1";
			case GJCustomColorMode::Col2:
				return "2";
			case GJCustomColorMode::LightBG:
				return "Light BG";
			case GJCustomColorMode::Col3:
				return "3";
			case GJCustomColorMode::Col4:
				return "4";
			case GJCustomColorMode::Col3DL:
				return "3DL";
			default:
				return "White";
		}
	}

	const char* objectTypeInfo(GameObjectType type) {
		switch (type) {
			default:
			case GameObjectType::Solid:
				return "Solid";
			case GameObjectType::Basic:
				return "Basic";
			case GameObjectType::Hazard:
				return "Hazard";
			case GameObjectType::Slope:
				return "Slope";
			case GameObjectType::Decoration:
				return "Decoration";
			case GameObjectType::PulsingDecoration:
				return "Pulsing";
			case GameObjectType::Modifier:
				return "Modifier";
			case GameObjectType::Breakable:
				return "Breakable";
			case GameObjectType::SecretCoin:
				return "Secret Coin";
			case GameObjectType::YellowJumpPad:
			case GameObjectType::PinkJumpPad:
			case GameObjectType::GravityPad:
				return "Pad";
			case GameObjectType::YellowJumpRing:
			case GameObjectType::PinkJumpRing:
			case GameObjectType::GravityRing:
				return "Orb";
			case GameObjectType::InverseGravityPortal:
			case GameObjectType::NormalGravityPortal:
			case GameObjectType::ShipPortal:
			case GameObjectType::CubePortal:
			case GameObjectType::NormalMirrorPortal:
			case GameObjectType::InverseMirrorPortal:
			case GameObjectType::BallPortal:
			case GameObjectType::RegularSizePortal:
			case GameObjectType::MiniSizePortal:
			case GameObjectType::UfoPortal:
			case GameObjectType::DualPortal:
			case GameObjectType::SoloPortal:
			case GameObjectType::WavePortal:
				return "Portal";
		}
	}

	constexpr bool isColorTrigger(int key) {
		return key == 29 || key == 30 || key == 104 || key == 105 || key == 221 || key == 717 || key == 718 || key == 743 || key == 744;
	}
}

void CustomEditorUI::Fields::colorSelectClosed(ColorSelectPopup* popup) {
	auto& customPopup = static_cast<CustomColorSelectPopup*>(popup)->m_fields;
	for (auto object : geode::cocos::CCArrayExt<GameObject>(m_self->m_selectedObjects)) {
		if (customPopup->m_colorDirty) {
			object->m_tintColor = m_multiColorObject->m_tintColor;
		}

		if (customPopup->m_durationDirty) {
			object->m_tintDuration = m_multiColorObject->m_tintDuration;
		}

		if (customPopup->m_tintGroundDirty) {
			object->m_tintGround = m_multiColorObject->m_tintGround;
		}

		if (customPopup->m_touchTriggedDirty) {
			object->m_touchTriggered = m_multiColorObject->m_touchTriggered;
		}

		if (customPopup->m_blendDirty) {
			object->m_customColorBlend = m_multiColorObject->m_customColorBlend;
		}

		if (customPopup->m_pColDirty) {
			object->m_copyPlayerColor1 = m_multiColorObject->m_copyPlayerColor1;
			object->m_copyPlayerColor2 = m_multiColorObject->m_copyPlayerColor2;
		}
	}

	m_multiColorObject = nullptr;
}

// persistent clipboard
void CustomEditorUI::onCopy(cocos2d::CCObject* target) {
	EditorUI::onCopy(target);

	if (this->m_selectedObject == nullptr && m_selectedObjects->count() == 0) {
		return;
	}

	auto string_ptr = this->m_clipboard;

	auto manager = GameManager::sharedState();
	manager->m_editorCopyString = string_ptr;
}

bool CustomEditorUI::init(LevelEditorLayer *editor) {
	if (!EditorUI::init(editor)) {
		return false;
	}

	auto manager = GameManager::sharedState();
	auto copy_string = manager->m_editorCopyString;

	if (!copy_string.empty()) {
		this->m_clipboard = copy_string;

		this->updateButtons();
	}

	auto infoLabel = cocos2d::CCLabelBMFont::create("", "chatFont.fnt");
	this->addChild(infoLabel);

	auto ccd = cocos2d::CCDirector::sharedDirector();

	infoLabel->setPosition({
		ccd->getScreenLeft() + 50.0f,
		ccd->getScreenTop() - 50.0f
	});
	infoLabel->setVisible(false);
	infoLabel->setID("info-label"_spr);
	infoLabel->setScale(0.65f);
	infoLabel->setAnchorPoint({0.0f, 1.0f});

	m_fields->m_infoLabel = infoLabel;

	auto statusIndicator = StatusIndicator::create();
	this->addChild(statusIndicator);

	statusIndicator->setPosition({
		ccd->getWinSize().width / 2,
		ccd->getScreenTop() - 55.0f
	});

	statusIndicator->setScale(0.75f);
	statusIndicator->setID("status-indicator"_spr);

	m_fields->m_statusIndicator = statusIndicator;

	return true;
}

cocos2d::CCMenu* CustomEditorUI::get_menu_bar_for_index(uint32_t ind, uint32_t page) {
	// CCArray of EditButtonBars
	auto edit_categories = this->m_createButtonBars;

	auto button_bar = static_cast<EditButtonBar*>(edit_categories->objectAtIndex(ind));

	// CCArray of `ButtonPage`s
	auto pages = button_bar->m_pagesArray;

	auto scroll_layer = static_cast<BoomScrollLayer*>(button_bar->getChildren()->objectAtIndex(0));
	auto extended_layer = static_cast<cocos2d::CCLayer*>(scroll_layer->getChildren()->objectAtIndex(0));

	auto scroll_layer_dot_node = static_cast<cocos2d::CCLayer*>(scroll_layer->getChildren()->objectAtIndex(1));
	auto scroll_layer_dot_container = scroll_layer->m_dotsArray;

	while (pages->count() <= page) {
		// page creation stuffs
		auto last_page = static_cast<ButtonPage*>(pages->lastObject());
		auto penultimate_page = static_cast<ButtonPage*>(pages->objectAtIndex(pages->count() - 2));

		auto last_pos = last_page->getPosition();

		auto blank_page = ButtonPage::create(
			cocos2d::CCArray::create(),
			// todo, find the origin of this constant (likely changes per device?)
			cocos2d::CCPoint(last_page->getPositionX() - penultimate_page->getPositionX(), last_pos.y)
		);

//          pages->addObject(blank_page);
		extended_layer->addChild(blank_page);

		scroll_layer->addPage(blank_page, page);

//          blank_page->setVisible(false);

		auto page_dot = cocos2d::CCSprite::create("smallDot.png");
		scroll_layer_dot_container->addObject(page_dot);
		scroll_layer_dot_node->addChild(page_dot);
	}

	auto last_page = static_cast<ButtonPage*>(pages->objectAtIndex(page));

	// CCMenu of `CreateMenuItem`
	return static_cast<cocos2d::CCMenu*>(
		last_page->getChildren()->objectAtIndex(0));
}

void CustomEditorUI::setupCreateMenu() {
	EditorUI::setupCreateMenu();

	auto gm = GameManager::sharedState();
	auto custom_obj_enabled = gm->getGameVariable(GameVariable::SHOW_UNUSED_OBJS);
	if (!custom_obj_enabled) {
		return;
	}

	auto chain_button_menu = this->get_menu_bar_for_index(8, 5);

	auto circle_create_btn = this->getCreateBtn(725, 4);
	auto grass_lg_create_btn = this->getCreateBtn(406, 4);
	auto grass_md_create_btn = this->getCreateBtn(407, 4);
	auto grass_sm_create_btn = this->getCreateBtn(408, 4);

	chain_button_menu->addChild(circle_create_btn);
	chain_button_menu->addChild(grass_lg_create_btn);
	chain_button_menu->addChild(grass_md_create_btn);
	chain_button_menu->addChild(grass_sm_create_btn);

	// button size is 40.0f
	// initial position is -105, -92
	auto initial_position = static_cast<cocos2d::CCNode*>(chain_button_menu->getChildren()->firstObject())->getPosition();

	auto btn_size = 40.0f;

	circle_create_btn->setPosition(initial_position.x + (btn_size * 5), initial_position.y);
	grass_lg_create_btn->setPosition(initial_position.x, initial_position.y - btn_size);
	grass_md_create_btn->setPosition(initial_position.x + 40.0f, initial_position.y - btn_size);
	grass_sm_create_btn->setPosition(initial_position.x + (btn_size * 2), initial_position.y - btn_size);

/*
	auto gold_coin_create_btn = this->getCreateBtn(142, 4);
	chain_button_menu->addChild(gold_coin_create_btn);
	gold_coin_create_btn->setPosition(initial_position.x + (btn_size * 3), initial_position.y - btn_size);
*/

	auto spike_button_menu = this->get_menu_bar_for_index(4, 2);

	auto spiky_ground_create_btn = this->getCreateBtn(421, 4);
	auto spiky_ground_edge_create_btn = this->getCreateBtn(422, 4);

	spike_button_menu->addChild(spiky_ground_create_btn);
	spike_button_menu->addChild(spiky_ground_edge_create_btn);

	spiky_ground_create_btn->setPosition(initial_position.x + (btn_size * 4), initial_position.y - btn_size);
	spiky_ground_edge_create_btn->setPosition(initial_position.x + (btn_size * 5), initial_position.y - btn_size);

	auto slope_button_menu = this->get_menu_bar_for_index(3, 7);

	auto short_slope_btn = this->getCreateBtn(371, 4);
	auto long_slope_btn = this->getCreateBtn(372, 4);
	auto short_slope_corner_btn = this->getCreateBtn(373, 4);
	auto long_slope_corner_btn = this->getCreateBtn(374, 4);

	slope_button_menu->addChild(short_slope_btn);
	slope_button_menu->addChild(long_slope_btn);
	slope_button_menu->addChild(short_slope_corner_btn);
	slope_button_menu->addChild(long_slope_corner_btn);

	short_slope_btn->setPosition(initial_position.x + (btn_size * 0), initial_position.y);
	long_slope_btn->setPosition(initial_position.x + (btn_size * 1), initial_position.y);
	short_slope_corner_btn->setPosition(initial_position.x + (btn_size * 2), initial_position.y);
	long_slope_corner_btn->setPosition(initial_position.x + (btn_size * 3), initial_position.y);

	auto ground_menu_first = this->get_menu_bar_for_index(7, 1);

	auto spiky_deco_btn = this->getCreateBtn(419, 4);
	ground_menu_first->addChild(spiky_deco_btn);

	spiky_deco_btn->setPosition(initial_position.x + (btn_size * 5), initial_position.y - btn_size);

	auto ground_menu_second = this->get_menu_bar_for_index(7, 2);

	auto spiky_deco_edge_btn = this->getCreateBtn(420, 4);
	ground_menu_second->addChild(spiky_deco_edge_btn);

	spiky_deco_edge_btn->setPosition(initial_position.x + (btn_size * 0), initial_position.y);

	auto triggers_menu = this->get_menu_bar_for_index(11, 1);
	auto test_transition_btn = this->getCreateBtn(55, 4);

	triggers_menu->addChild(test_transition_btn);

	test_transition_btn->setPosition(initial_position.x + (btn_size * 5), initial_position.y - btn_size);

	return;
}

void CustomEditorUI::updateInfoLabel(float) {
	m_fields->m_updateInfoLabel = false;

	if (!GameManager::sharedState()->getGameVariable(GameVariable::SHOW_INFO_LABEL)) {
		m_fields->m_infoLabel->setVisible(false);
		return;
	}

	if (m_selectedObject != nullptr) {
		auto selected = m_selectedObject;
		auto info = fmt::format(
			"{} (#{})\n({:.2f}, {:.2f})\nOrder: {} ({})\nRotation: {}",
			objectTypeInfo(selected->m_objectType),
			selected->m_objectID,
			selected->getPositionX(),
			selected->getPositionY(),
			selected->m_defaultZOrder,
			(selected->m_forceBottomLayer || selected->m_defaultZOrder < 0) ? "B" : "T",
			selected->getRotation()
		);

		if (selected->canChangeCustomColor()) {
			info += fmt::format("\nColor: {}", colorModeInfo(selected->m_customColorMode, selected->m_defaultColorMode));
		}

		if (selected->m_objectType == GameObjectType::SecretCoin) {
			info += fmt::format("\nCoin ID: {}", selected->m_secretCoinID);
		}

		m_fields->m_infoLabel->setString(info.c_str());
		m_fields->m_infoLabel->setVisible(true);

		return;
	}

	if (m_selectedObjects->count() > 0) {
		auto info = fmt::format(
			"Objects: {}",
			m_selectedObjects->count()
		);

		m_fields->m_infoLabel->setString(info.c_str());
		m_fields->m_infoLabel->setVisible(true);

		return;
	}

	m_fields->m_infoLabel->setVisible(false);
}

void CustomEditorUI::scheduleInfoLabelUpdate() {
	if (!m_fields->m_updateInfoLabel) {
		this->scheduleOnce(static_cast<cocos2d::SEL_SCHEDULE>(&CustomEditorUI::updateInfoLabel), 0);
		m_fields->m_updateInfoLabel = true;
	}
}

void CustomEditorUI::angleChanged(float angle) {
	EditorUI::angleChanged(angle);
	this->scheduleInfoLabelUpdate();
}

void CustomEditorUI::selectObject(GameObject* object) {
	EditorUI::selectObject(object);
	this->scheduleInfoLabelUpdate();
}

void CustomEditorUI::moveObject(GameObject* object, cocos2d::CCPoint to) {
	EditorUI::moveObject(object, to);
	this->scheduleInfoLabelUpdate();
}

void CustomEditorUI::deselectObject(GameObject* object) {
	EditorUI::deselectObject(object);
	this->scheduleInfoLabelUpdate();
}

void CustomEditorUI::deselectAll() {
	EditorUI::deselectAll();
	this->scheduleInfoLabelUpdate();
}

void CustomEditorUI::selectObjects(cocos2d::CCArray* objects) {
	EditorUI::selectObjects(objects);
	this->scheduleInfoLabelUpdate();
}

void CustomEditorUI::transformObjectCall(EditCommand command) {
	EditorUI::transformObjectCall(command);
	this->scheduleInfoLabelUpdate();
}

bool CustomEditorUI::multiSelectingColorTriggers() {
	if (m_selectedObject != nullptr || m_selectedObjects->count() == 0) {
		return false;
	}

	auto objectsCount = m_selectedObjects->count();
	for (auto object : geode::cocos::CCArrayExt<GameObject>(m_selectedObjects)) {
		auto key = object->m_objectID;
		if (!isColorTrigger(key)) {
			return false;
		}
	}

	return true;
}

void CustomEditorUI::editObject(cocos2d::CCObject* callback) {
	if (multiSelectingColorTriggers()) {
		auto object = GameObject::create("edit_eTint3DLBtn_001.png");

		auto showBlending = false;
		auto showTintGround = false;

		auto isSameColor = true;
		auto isSameBlend = true;
		auto isSameTintGround = true;
		auto isSameCopyP1 = true;
		auto isSameCopyP2 = true;
		auto isSameDuration = true;
		auto isSameTouchTriggered = true;
		auto isSameKey = true;

		auto firstObject = static_cast<GameObject*>(m_selectedObjects->firstObject());

		for (auto object : geode::cocos::CCArrayExt<GameObject>(m_selectedObjects)) {
			auto key = object->m_objectID;

			if (firstObject->m_objectID != key) {
				isSameKey = false;
			}

			if (firstObject->m_tintColor != object->m_tintColor) {
				isSameColor = false;
			}

			if (firstObject->m_customColorBlend != object->m_customColorBlend) {
				isSameBlend = false;
			}

			if (firstObject->m_tintGround != object->m_tintGround) {
				isSameTintGround = false;
			}

			if (firstObject->m_copyPlayerColor1 != object->m_copyPlayerColor1) {
				isSameCopyP1 = false;
			}

			if (firstObject->m_copyPlayerColor2 != object->m_copyPlayerColor2) {
				isSameCopyP2 = false;
			}

			if (firstObject->m_tintDuration != object->m_tintDuration) {
				isSameDuration = false;
			}

			if (firstObject->m_touchTriggered != object->m_touchTriggered) {
				isSameTouchTriggered = false;
			}

			switch (key) {
				// bg trigger
				case 29:
					showTintGround = true;
					break;
				// ground trigger
				case 30:
				// line trigger
				case 104:
				// object trigger
				case 105:
					break;
				// col1 trigger
				case 221:
				// col2 trigger
				case 717:
				// col3 trigger
				case 718:
				// col4 trigger
				case 743:
				// 3dl trigger
				case 744:
					showBlending = true;
					break;
			}
		}

		auto selectedObject = 30;
		if (isSameKey) {
			selectedObject = firstObject->m_objectID;
		} else if (showBlending) {
			selectedObject = 744;
		} else if (showTintGround) {
			selectedObject = 29;
		}

		object->m_objectID = selectedObject;

		if (isSameColor) {
			object->m_tintColor = firstObject->m_tintColor;
		}

		if (isSameBlend) {
			object->m_customColorBlend = firstObject->m_customColorBlend;
		}

		if (isSameTintGround) {
			object->m_tintGround = firstObject->m_tintGround;
		}

		if (isSameCopyP1) {
			object->m_copyPlayerColor1 = firstObject->m_copyPlayerColor1;
		}

		if (isSameCopyP2) {
			object->m_copyPlayerColor2 = firstObject->m_copyPlayerColor2;
		}

		if (isSameDuration) {
			object->m_tintDuration = firstObject->m_tintDuration;
		}

		if (isSameTouchTriggered) {
			object->m_touchTriggered = firstObject->m_touchTriggered;
		}

		m_fields->m_multiColorObject.swap(object);

		auto selectPopup = ColorSelectPopup::create(object);

		if (!isSameDuration) {
			selectPopup->m_durationLabel->setString("FadeTime: Mixed");
		}

		if (!isSameColor) {
			// this is dumb
			selectPopup->m_colorPicker->m_colourPicker->getSlider()->setVisible(false);
		}

		if (!isSameKey) {
			object->m_objectID = 1;
		}

		selectPopup->m_delegate = m_fields.self();
		m_fields->m_self = this;

		object->setUserObject("multi-selection"_spr, m_selectedObjects);

		selectPopup->show();

		return;
	}

	EditorUI::editObject(callback);
}

bool CustomEditorUI::editButtonUsable() {
	if (multiSelectingColorTriggers()) {
		return true;
	}

	return EditorUI::editButtonUsable();
}

void CustomEditorUI::keyDown(cocos2d::enumKeyCodes key) {
	EditorUI::keyDown(key);

	if (key == cocos2d::enumKeyCodes::KEY_F5) {
		auto hitboxesEnabled = GameManager::sharedState()->getGameVariable(GameVariable::SHOW_EDITOR_HITBOXES);
		GameManager::sharedState()->setGameVariable(GameVariable::SHOW_EDITOR_HITBOXES, !hitboxesEnabled);

		auto statusIndicator = m_fields->m_statusIndicator;
		if (hitboxesEnabled) {
			statusIndicator->setLabel("Show Hitboxes: Off");
		} else {
			statusIndicator->setLabel("Show Hitboxes: On");
		}
		statusIndicator->show();
	}

	if (key == cocos2d::enumKeyCodes::KEY_F6) {
		auto varEnabled = GameManager::sharedState()->getGameVariable(GameVariable::EDITOR_FOLLOW_PLAYER);
		GameManager::sharedState()->setGameVariable(GameVariable::EDITOR_FOLLOW_PLAYER, !varEnabled);

		auto statusIndicator = m_fields->m_statusIndicator;
		if (varEnabled) {
			statusIndicator->setLabel("Follow Player: Off");
		} else {
			statusIndicator->setLabel("Follow Player: On");
		}
		statusIndicator->show();
	}

	if (key == cocos2d::enumKeyCodes::KEY_W) {
		m_editorLayer->pushButton(1, true);
	}

	if (key == cocos2d::enumKeyCodes::KEY_Up) {
		m_editorLayer->pushButton(1, false);
	}
}

void CustomEditorUI::keyUp(cocos2d::enumKeyCodes key) {
	EditorUI::keyUp(key);

	if (key == cocos2d::enumKeyCodes::KEY_W) {
		m_editorLayer->releaseButton(1, true);
	}

	if (key == cocos2d::enumKeyCodes::KEY_Up) {
		m_editorLayer->releaseButton(1, false);
	}
}

void CustomEditorUI::scrollWheel(float y, float x) {
	auto keyboardDispatcher = cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher();
	if (keyboardDispatcher->m_bControlPressed) {
		// this is lazy behavior. match 2.2 here
		auto s = keyboardDispatcher->m_bShiftPressed;
		keyboardDispatcher->m_bShiftPressed = true;
		EditorUI::scrollWheel(-y, -x);

		keyboardDispatcher->m_bShiftPressed = s;
		return;
	}

	if (keyboardDispatcher->m_bAltPressed) {
		EditorUI::scrollWheel(x, y);
		return;
	}

	EditorUI::scrollWheel(y, x);
}
