#include "classes/settings/pages/graphicssettingspage.hpp"

void GraphicsSettingsPage::createPage() {
	const float width = this->_dimensions.width;
	const float height = this->_dimensions.height;

	auto title = cocos2d::CCLabelBMFont::create("Graphics", "bigFont.fnt");
	this->addChild(title);

	title->setPosition(width / 2, height - 10.0f);
	title->setScale(0.75f);

	this->addToggle(
		"Limit Aspect Ratio\n(req. restart)",
		PlatformHelper::is_screen_restricted(),
		static_cast<cocos2d::SEL_MenuHandler>(&GraphicsSettingsPage::onLimitAspectRatio),
		"Limits the game window to a maximum aspect ratio of 16:9.");

	auto descLabel = cocos2d::CCLabelBMFont::create("Quality:", "goldFont.fnt");
	this->addChild(descLabel);

	descLabel->setScale(0.75f);
	descLabel->setPosition(7.0f, (height / 2) + 10.0f);
	descLabel->setAnchorPoint({ 0.0f, 0.5f });

	auto ccd = cocos2d::CCDirector::sharedDirector();
	this->_selected_quality = ccd->getLoadedTextureQuality();
	if (static_cast<int>(this->_selected_quality) > 3) {
		this->_selected_quality = cocos2d::TextureQuality::kTextureQualityHigh;
	}

	auto apply_sprite = ButtonSprite::create("Apply", 80, 0, 1.0f, false);

	this->_apply_button = CCMenuItemSpriteExtra::create(apply_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&GraphicsSettingsPage::onApply));

	this->_internal_menu->addChild(this->_apply_button);
	this->_menu_objects.push_back(this->_apply_button);
	_apply_button->setPosition((-_window_dimensions.width / 2) + (width / 2) + 87.0f, -(_window_dimensions.height / 2) + 35.0f);

	this->_quality_label = cocos2d::CCLabelBMFont::create(labelForQuality(this->_selected_quality), "bigFont.fnt");
	this->addChild(this->_quality_label);
	this->_quality_label->setPosition(width / 2, (height / 2) - 38.0f);
	this->_quality_label->setScale(1.0f);

	auto sprite_center = cocos2d::CCPoint((-_window_dimensions.width / 2) + (width / 2), 0.0f);

	auto next_sprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
	next_sprite->setScale(2.0f);
	auto next_btn = CCMenuItemSpriteExtra::create(next_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&GraphicsSettingsPage::onNext));
	this->_internal_menu->addChild(next_btn);
	this->_menu_objects.push_back(next_btn);

	next_btn->setPosition(sprite_center.x + 85.0f + 13.0f, sprite_center.y - 39.0f);

	auto prev_sprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
	prev_sprite->setScale(2.0f);
	auto prev_btn = CCMenuItemSpriteExtra::create(prev_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&GraphicsSettingsPage::onPrev));
	this->_internal_menu->addChild(prev_btn);
	this->_menu_objects.push_back(prev_btn);

	prev_btn->setPosition(sprite_center.x - 85.0f + 13.0f, sprite_center.y - 39.0f);

	auto textures_sprite = ButtonSprite::create("Textures", 380, 0, 1.0f, false);
	auto textures_btn = CCMenuItemSpriteExtra::create(textures_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&GraphicsSettingsPage::onTextures));
	this->_internal_menu->addChild(textures_btn);
	this->_menu_objects.push_back(textures_btn);

	textures_btn->setPosition((-_window_dimensions.width / 2) + (width / 2) - 37.0f, -(_window_dimensions.height / 2) + 35.0f);
}

void GraphicsSettingsPage::onLimitAspectRatio(cocos2d::CCObject*) {
	PlatformHelper::toggle_is_screen_restricted();
}

void GraphicsSettingsPage::onPrev(cocos2d::CCObject*)
{
	auto btn = static_cast<int32_t>(this->_selected_quality);
	btn--;
	this->updateToggles(btn);
}

void GraphicsSettingsPage::onNext(cocos2d::CCObject*)
{
	auto btn = static_cast<int32_t>(this->_selected_quality);
	btn++;
	this->updateToggles(btn);
}

void GraphicsSettingsPage::onApply(cocos2d::CCObject*)
{
	auto gm = GameManager::sharedState();
	gm->m_texQuality = this->_selected_quality;

	auto ccd = cocos2d::CCDirector::sharedDirector();

	auto c_quality = ccd->getLoadedTextureQuality();

	ccd->updateContentScale(this->_selected_quality);

	if (ccd->getLoadedTextureQuality() != c_quality) {
		gm->reloadAll(false, false, true);
	} else {
		FLAlertLayer::create(nullptr, "Info", "No changes were made to graphics quality.", "OK", nullptr)->show();
	}
}

void GraphicsSettingsPage::onTextures(cocos2d::CCObject*)
{
	TextureSettingsPopup::create()->show();
}

const char* GraphicsSettingsPage::labelForQuality(cocos2d::TextureQuality quality)
{
	switch (quality) {
		case cocos2d::kTextureQualityLow:
			return "Low";
		case cocos2d::kTextureQualityMedium:
			return "Medium";
		case cocos2d::kTextureQualityHigh:
		default:
			return "High";
	}
}

void GraphicsSettingsPage::updateToggles(uint32_t btn)
{
	auto ccd = cocos2d::CCDirector::sharedDirector();

	if (btn > 3) {
		btn = 1;
	}

	if (btn < 1) {
		btn = 3;
	}

	this->_selected_quality = static_cast<cocos2d::TextureQuality>(btn);

	auto c_quality = ccd->getLoadedTextureQuality();

	if (this->_selected_quality != c_quality) {
		_apply_button->setColor({ 0xFF, 0xFF, 0xFF });
	} else {
		_apply_button->setColor({ 0xA6, 0xA6, 0xA6 });
	}

	this->_quality_label->setString(labelForQuality(this->_selected_quality));
}