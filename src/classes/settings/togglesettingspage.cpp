#include "classes/settings/togglesettingspage.hpp"

void ToggleSettingsPage::addToggle(const char* name, bool default_on, cocos2d::SEL_MenuHandler callback, const char* desc)
{
	if (_pages.empty() || _toggle_count >= MAX_TOGGLE_PER_PAGE) {
		_pages.emplace_back();

		auto obj_layer = cocos2d::CCLayer::create();
		this->addChild(obj_layer);
		_pages.back().push_back(obj_layer);

		_toggle_count = 0;

		if (_pages.size() > 1 && !_has_page_counter) {
			auto next_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
			auto next_btn = CCMenuItemSpriteExtra::create(
					next_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onPageNext));

			this->_internal_menu->addChild(next_btn);
			this->_menu_objects.push_back(next_btn);

			next_btn->setPosition(-(_window_dimensions.width / 2) - 21.0f, 0.0f);

			_has_page_counter = true;
		}
	}

	auto& current_page = _pages.back();
	auto obj_layer = current_page.front();

	auto pos = cocos2d::CCPoint(20.0f, this->getNextToggleY());

	auto toggle = GameToolbox::createToggleButton(name, callback, default_on, this->_internal_menu, pos, this, obj_layer, 0.8f, 0.5f, 205.0f, cocos2d::CCPoint(8.0f, 0.0f), "bigFont.fnt", false);
	toggle->setSizeMult(1.0f);

	// the toggle is placed very strangely relative to objects, thanks rob?
	toggle->setPosition(-(_window_dimensions.width / 2) + 32.0f, this->getNextToggleMenuY());
	this->_menu_objects.push_back(toggle);
	current_page.push_back(toggle);
	this->_toggle_count++;

	if (_pages.size() > 1) {
		toggle->setVisible(false);
	}

	if (desc)
	{
		auto info_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
		info_sprite->setScale(0.5f);
		auto info_btn = CCMenuItemSpriteExtra::create(info_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&ToggleSettingsPage::onInfoText));
		info_btn->setSizeMult(2.0f);

		auto toggle_pos = toggle->getPosition();

		this->_internal_menu->addChild(info_btn);
		this->_menu_objects.push_back(info_btn);
		current_page.push_back(info_btn);
		if (_pages.size() > 1) {
			info_btn->setVisible(false);
		}

		info_btn->setPosition(toggle_pos.x - 18.0f, toggle_pos.y + 16.0f);

		auto desc_str = std::string(desc);

		auto desc_identifier = static_cast<uint32_t>(std::hash<std::string>{}(desc_str));
		this->_info_text.insert({ desc_identifier, desc_str });

		info_btn->setTag(desc_identifier);
		info_btn->setSizeMult(2.0f);
	}
}

void ToggleSettingsPage::addSpacer(int inc) {
	this->_toggle_count += inc;
}

float ToggleSettingsPage::getNextToggleY() {
	return _dimensions.height - (this->_toggle_count * TOGGLE_SPACING) - 54.0f;
}

float ToggleSettingsPage::getNextToggleMenuY() {
	return (_window_dimensions.height / 2) - (static_cast<float>(this->_toggle_count + 1) * TOGGLE_SPACING) - 23.0f;
}

void ToggleSettingsPage::onPageNext(cocos2d::CCObject*) {
	auto oldPage = _pages.at(_current_page);

	for (const auto& i : oldPage) {
		i->setVisible(false);
	}

	if (_pages.size() > _current_page + 1) {
		_current_page++;
	} else {
		_current_page = 0;
	}

	auto newPage = _pages.at(_current_page);

	for (const auto& i : newPage) {
		i->setVisible(true);
	}
}

void ToggleSettingsPage::onInfoText(cocos2d::CCObject* target) {
	auto n_target = geode::cast::typeinfo_cast<cocos2d::CCNode*>(target);
	if (n_target == nullptr) {
		return;
	}

	auto identifier = n_target->getTag();
	auto desc = _info_text[identifier];

	FLAlertLayer::create(nullptr, "Info", desc.c_str(), "OK", nullptr, 300.0f)->show();
}

void ToggleSettingsPage::onToggleVisibility(bool visible) {
	SettingsPage::onToggleVisibility(visible);

	_current_page = 0;
	// hide all other pages
	if (visible && !_pages.empty()) {
		bool has_first = false;
		for (const auto& p : _pages) {
			if (!has_first) {
				has_first = true;
				// enable visibility of first page only
				for (const auto& i : p) {
					i->setVisible(visible);
				}
				continue;
			}
			for (const auto& i : p) {
				i->setVisible(false);
			}
		}
	}
}
