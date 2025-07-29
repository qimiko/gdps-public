#include "classes/settings/settingspopup.hpp"

bool SettingsPopup::init(int page)
{
	if (this->initWithColor({ 0, 0, 0, 75 })) {
		constexpr auto width = 400.0f;
		constexpr auto height = 280.0f;

		auto ccd = cocos2d::CCDirector::sharedDirector();
		ccd->getTouchDispatcher()->incrementForcePrio();
		this->setTouchEnabled(true);
		this->setKeypadEnabled(true);

		this->m_controlConnected = -1;
		this->m_scrollAction = -1;

		this->m_mainLayer = cocos2d::CCLayer::create();
		this->addChild(this->m_mainLayer);

		auto window_size = ccd->getWinSize();
		auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", { 0.0f, 0.0f, 80.0f, 80.0f });
		bg->setContentSize(cocos2d::CCSize(width, height));
		bg->setPosition(window_size.width / 2, window_size.height / 2);

		this->m_mainLayer->addChild(bg, -2);

		this->m_buttonMenu = cocos2d::CCMenu::create();
		this->m_mainLayer->addChild(this->m_buttonMenu, 3);

		auto close_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
		auto close_btn = CCMenuItemSpriteExtra::create(
			close_sprite,
			nullptr,
			this,
			static_cast<cocos2d::SEL_MenuHandler>(&SettingsPopup::onClose)
		);

		this->m_buttonMenu->addChild(close_btn);
		close_btn->setPosition(-((width / 2) - 5), (height / 2) - 5);

		auto tabs_bg = cocos2d::extension::CCScale9Sprite::create("square02_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
		tabs_bg->setOpacity(75);
		tabs_bg->setContentSize(cocos2d::CCSize(100.0f, height - 20.0f));
		tabs_bg->setPosition(cocos2d::CCPoint(((window_size.width + width) / 2.0f) - 60.0f, window_size.height / 2));

		this->m_mainLayer->addChild(tabs_bg, -1);

		const std::vector<SettingsItem> items {
			{ "Gameplay", "gameplaySettingsBadge.png"_spr, 0.37f, GameplaySettingsPage::create() },
			{ "Interface", "interfaceSettingsBadge.png"_spr, 0.36f, InterfaceSettingsPage::create() },
			{ "Editor", "editorSettingsBadge.png"_spr, 0.5f, EditorSettingsPage::create() },
			{ "Songs", "songsSettingsBadge.png"_spr, 0.5f, SongsSettingsPage::create() },
			{ "Tweaks", "tweaksSettingsBadge.png"_spr, 0.47f, TweaksSettingsPage::create() },
#ifdef GEODE_IS_ANDROID
			{ "Graphics", "graphicsSettingsBadge.png"_spr, 0.36f, GraphicsSettingsPage::create() },
#endif
			{ "About", "aboutSettingsBadge.png"_spr, 0.5f, AboutSettingsPage::create() }
		};

		auto i = 0u;
		this->buttons_ = cocos2d::CCArray::create();
		this->layers_ = cocos2d::CCArray::create();

		this->buttons_->retain();
		this->layers_->retain();

		for (const auto& item : items) {
			auto toggle_sprite = SearchButton::create("GJ_longBtn01_001.png", item.title, item.size, item.icon);
			auto toggle_on_sprite = SearchButton::create("GJ_longBtn02_001.png", item.title, item.size, item.icon);

			auto toggle = CCMenuItemToggler::create(toggle_sprite, toggle_on_sprite, this, static_cast<cocos2d::SEL_MenuHandler>(&SettingsPopup::onToggleLayer));

			this->m_buttonMenu->addChild(toggle);
			toggle->setPosition((width / 2) - 60.0f, (height / 2) - 32.0f - (i * 36.0f));
			toggle->setTag(i);

			item.page->setDimensions(cocos2d::CCSize(width - 130.0f, height - 30.0f));
			item.page->setWindowDimensions(cocos2d::CCSize(width, height));
			item.page->setInternalMenu(this->m_buttonMenu);
			item.page->createPage();

			this->m_mainLayer->addChild(item.page, 2);
			item.page->setPosition({ ((window_size.width - width) / 2.0f) + 13.0f, ((window_size.height - height) / 2.0f) + 17.0f });
			item.page->setVisible(false);

			this->buttons_->addObject(toggle);
			this->layers_->addObject(item.page);

			i++;
		}

		if (page == -1) {
			this->toggleLayer(i - 1);
		} else {
			this->toggleLayer(page);
		}

		return true;
	}

	return false;
}

void SettingsPopup::keyBackClicked()
{
	this->onClose(nullptr);
};

void SettingsPopup::onClose(cocos2d::CCObject*)
{
	this->setKeyboardEnabled(false);
	this->removeFromParentAndCleanup(true);

	for (auto i = 0u; i < this->layers_->count(); i++) {
		auto selected_layer = dynamic_cast<SettingsPage*>(this->layers_->objectAtIndex(i));
		if (selected_layer == nullptr) {
			continue;
		}

		selected_layer->onClose();
	}

	this->buttons_->release();
	this->layers_->release();
}

void SettingsPopup::onToggleLayer(cocos2d::CCObject* target)
{
	auto n_target = geode::cast::typeinfo_cast<cocos2d::CCNode*>(target);
	if (n_target == nullptr) {
		return;
	}

	this->toggleLayer(n_target->getTag());
}

void SettingsPopup::toggleLayer(uint32_t layer)
{
	if (this->layers_ == nullptr) {
		return;
	}

	if (this->buttons_ == nullptr) {
		return;
	}

	for (auto i = 0u; i < this->buttons_->count(); i++) {
		auto current_button = geode::cast::typeinfo_cast<CCMenuItemToggler*>(this->buttons_->objectAtIndex(i));
		if (current_button == nullptr) {
			continue;
		}

		if (i != layer) {
			current_button->toggle(false);
			current_button->m_notClickable = false;
		} else {
			if (!current_button->m_toggled) {
				current_button->toggle(true);
				current_button->m_notClickable = true;
			}
		}
	}

	for (auto i = 0u; i < this->layers_->count(); i++) {
		auto selected_layer = dynamic_cast<SettingsPage*>(this->layers_->objectAtIndex(i));
		if (selected_layer == nullptr) {
			continue;
		}

		auto visibility = i == layer;
		selected_layer->setVisible(visibility);
		selected_layer->onToggleVisibility(visibility);
	}
}
