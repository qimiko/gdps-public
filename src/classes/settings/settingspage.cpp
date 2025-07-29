#include "classes/settings/settingspage.hpp"

void SettingsPage::setDimensions(const cocos2d::CCSize& dimensions) {
	this->_dimensions = dimensions;
}

void SettingsPage::setWindowDimensions(const cocos2d::CCSize& dimensions) {
	this->_window_dimensions = dimensions;
}

void SettingsPage::setInternalMenu(cocos2d::CCMenu* menu) {
	this->_internal_menu = menu;
}

void SettingsPage::onToggleVisibility(bool visible) {
	for (const auto& object : this->_menu_objects) {
		object->setVisible(visible);
	}
}
