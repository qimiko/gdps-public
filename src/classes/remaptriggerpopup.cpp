#include "classes/remaptriggerpopup.hpp"

void RemapTriggerPopup::keyBackClicked() {
	this->onClose(nullptr);
};

void RemapTriggerPopup::onClose(cocos2d::CCObject*) {
	remapObjects();

	if (m_parentPopup != nullptr) {
		m_parentPopup->closeColorSelect(nullptr);

		if (m_editorUi != nullptr) {
			m_editorUi->editObject(nullptr);
		}
	}

	this->setKeyboardEnabled(false);
	this->removeFromParentAndCleanup(true);
}

void RemapTriggerPopup::remapObject(GameObject* object) {
	if (auto selection = static_cast<cocos2d::CCArray*>(object->getUserObject("multi-selection"_spr)); selection != nullptr) {
		for (const auto& object : geode::cocos::CCArrayExt<GameObject*>(selection)) {
			remapObject(object);
		}

		return;
	}

	object->m_objectID = m_selected;

	auto frameName = static_cast<cocos2d::CCString*>(ObjectToolbox::sharedState()->m_keyToFrameDict->objectForKey(m_selected));
	if (frameName == nullptr) {
		return;
	}

	auto frame = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName->getCString());
	object->setDisplayFrame(frame);
}

void RemapTriggerPopup::remapObjects() {
	if (!m_selectionDirty) {
		return;
	}

	remapObject(m_gameObject);
	m_selectionDirty = false;
}

cocos2d::CCPoint RemapTriggerPopup::getNextButtonPos() {
	auto window_size = cocos2d::CCDirector::sharedDirector()->getWinSize();
	auto left = (window_size.width - m_width) / 2 + 45.0f;
	auto center = window_size.height/2;

	auto buttonIdx = m_buttons.size();

	if (buttonIdx >= 5) {
		return { left + (buttonIdx - 5) * 75.0f + 18.0f, center - 25.0f };
	} else {
		return { left + buttonIdx * 65.0f, center + 25.0f };
	}
}

void RemapTriggerPopup::addButton(const char* title, int id) {
	auto buttonIdx = m_buttons.size();
	auto buttonSize = buttonIdx >= 5 ? 50 : 40;

	auto sprite = ButtonSprite::create(title, buttonSize, 0, 0.5f, true, "bigFont.fnt", "GJ_button_04.png", 30.0f);
	auto button = CCMenuItemSpriteExtra::create(
		sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&RemapTriggerPopup::onSelect)
	);

	m_buttonMenu->addChild(button);

	sprite->setTag(id);
	button->setTag(id);

	auto pos = this->getNextButtonPos();
	button->setPosition(pos);

	m_buttons.push_back(sprite);
}

void RemapTriggerPopup::onSelect(cocos2d::CCObject* button) {
	auto tag = static_cast<cocos2d::CCNode*>(button)->getTag();
	this->onSelectId(tag);
}

void RemapTriggerPopup::onSelectId(int id) {
	m_selected = id;
	m_selectionDirty = true;

	for (const auto& btn : m_buttons) {
		if (btn->getTag() != id) {
			btn->updateBGImage("GJ_button_04.png");
		} else {
			btn->updateBGImage("GJ_button_02.png");
		}
	}
}

bool RemapTriggerPopup::init(GameObject* object, ColorSelectPopup* popup) {
	if (!this->initWithColor({0, 0, 0, 75})) {
		return false;
	}

	m_gameObject = object;
	m_parentPopup = popup;

	auto ccd = cocos2d::CCDirector::sharedDirector();
	ccd->getTouchDispatcher()->incrementForcePrio();
	this->setTouchEnabled(true);
	this->setKeypadEnabled(true);

	this->m_controlConnected = -1;
	this->m_scrollAction = -1;

	this->m_mainLayer = cocos2d::CCLayer::create();
	this->addChild(this->m_mainLayer);

	auto window_size = ccd->getWinSize();
	auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", {0.0f, 0.0f, 80.0f, 80.0f});
	bg->setContentSize(cocos2d::CCSize(m_width, m_height));
	bg->setPosition(window_size.width / 2, window_size.height / 2);

	this->m_mainLayer->addChild(bg, -2);

	this->m_buttonMenu = cocos2d::CCMenu::create();
	this->m_mainLayer->addChild(this->m_buttonMenu);
	m_buttonMenu->ignoreAnchorPointForPosition(false);

	auto close_sprite = ButtonSprite::create("OK");
	auto close_btn = CCMenuItemSpriteExtra::create(
		close_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&RemapTriggerPopup::onClose)
	);

	this->m_buttonMenu->addChild(close_btn);
	close_btn->setPosition(
		window_size.width / 2,
		(window_size.height - m_height) / 2 + 25.0f
	);

	auto title = cocos2d::CCLabelBMFont::create("Remap Color", "bigFont.fnt");
	this->m_mainLayer->addChild(title);

	title->setPosition({
		window_size.width / 2,
		(window_size.height + m_height) / 2 - 25.0f
	});

	this->addButton("BG", 29);
	this->addButton("G", 30);
	this->addButton("L", 104);
	this->addButton("Obj", 105);
	this->addButton("3DL", 744);
	this->addButton("Col1", 221);
	this->addButton("Col2", 717);
	this->addButton("Col3", 718);
	this->addButton("Col4", 743);

	this->onSelectId(m_gameObject->m_objectID);
	m_selectionDirty = false;

	return true;
}
