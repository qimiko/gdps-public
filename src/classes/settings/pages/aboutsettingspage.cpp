#include "classes/settings/pages/aboutsettingspage.hpp"

#include <Geode/ui/GeodeUI.hpp>

// someone forgot to export this and it's too late to fix it
namespace {
std::string VersionTag_toSuffixString(const geode::VersionTag& tag) {
	std::string res = "";
	switch (tag.value) {
		case geode::VersionTag::Alpha: res += "-alpha"; break;
		case geode::VersionTag::Beta: res += "-beta"; break;
		case geode::VersionTag::Prerelease: res += "-prerelease"; break;
	}
	if (tag.number) {
		res += "." + std::to_string(tag.number.value());
	}
	return res;
}
}

void AboutSettingsPage::createPage() {
	const float width = this->_dimensions.width;
	const float height = this->_dimensions.height;

	auto internal_layer = cocos2d::CCLayer::create();
	this->addChild(internal_layer);

	auto version = geode::Mod::get()->getVersion();

	auto versionString = fmt::format("1.9 GDPS u{}.{}", version.getMajor(), version.getMinor());

	auto tag = version.getTag();
	if (version.getPatch() != 0 || tag) {
		versionString += fmt::format(".{}", version.getPatch());
	}

	if (tag) {
		versionString += VersionTag_toSuffixString(*tag);
	}

	auto title = cocos2d::CCLabelBMFont::create(versionString.c_str(), "bigFont.fnt");
	internal_layer->addChild(title);
	title->setPosition(width / 2, height - 15.0f);
	title->limitLabelWidth(width - 30.0f, 0.75f, 0.5f);

/*
	// code for position testing

	auto h_top = cocos2d::CCLabelBMFont::create("-T-", "bigFont.fnt");
	internal_layer->addChild(h_top, 3);
	h_top->setPosition(width / 2, height);
	h_top->setOpacity(127);

	auto h_left = cocos2d::CCLabelBMFont::create("-L-", "bigFont.fnt");
	internal_layer->addChild(h_left, 3);
	h_left->setPosition(0.0f, height / 2);
	h_left->setOpacity(127);
	h_left->setRotation(270.0f);

	auto h_bottom = cocos2d::CCLabelBMFont::create("-B-", "bigFont.fnt");
	internal_layer->addChild(h_bottom, 3);
	h_bottom->setPosition(width / 2, 0.0f);
	h_bottom->setOpacity(127);
	h_bottom->setRotation(180.0f);

	auto h_right = cocos2d::CCLabelBMFont::create("-R-", "bigFont.fnt");
	internal_layer->addChild(h_right, 3);
	h_right->setPosition(width, height / 2);
	h_right->setOpacity(127);
	h_right->setRotation(90.0f);
*/

	auto version_string = cocos2d::CCString::createWithFormat(
		"%s-%s",
		CMakeConfiguration::BRANCH, CMakeConfiguration::HASH);

	auto label = cocos2d::CCLabelBMFont::create(version_string->getCString(), "chatFont.fnt");
	internal_layer->addChild(label);
	label->setColor({  0xBB, 0xBB, 0xBB });

	label->setAlignment(cocos2d::CCTextAlignment::kCCTextAlignmentLeft);
	label->setAnchorPoint(cocos2d::CCPoint(0.0f, 0.0f));
	label->setScale(0.5f);
	label->setPosition(cocos2d::CCPoint(0.0f, 20.0f));

	auto penguin = cocos2d::CCSprite::create("splash.png"_spr);

	auto penguin_offset_x = -2.0f;
	auto penguin_offset_y = 10.0f;

	internal_layer->addChild(penguin);

	penguin->setPosition({ (width / 2) + penguin_offset_x, (height / 2) + penguin_offset_y });
	penguin->setScale(1.0f);

	auto c_top = cocos2d::CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
	auto c_left = cocos2d::CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
	auto c_bottom = cocos2d::CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
	c_bottom->setFlipY(true);

	auto c_right = cocos2d::CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
	c_right->setFlipX(true);

	internal_layer->addChild(c_top, 2);
	internal_layer->addChild(c_bottom, 2);
	internal_layer->addChild(c_left, 1);
	internal_layer->addChild(c_right, 1);

	c_top->setPositionX((width / 2) + penguin_offset_x);
	c_top->setPositionY((height / 2) + 68.0f + penguin_offset_y);
	c_top->setScaleX(0.78f);

	c_bottom->setPositionX((width / 2) + penguin_offset_x);
	c_bottom->setPositionY((height / 2) - 68.0f + penguin_offset_y);
	c_bottom->setScaleX(0.78f);

	c_left->setPositionX((width / 2) - 136.0f + penguin_offset_x);
	c_left->setPositionY((height / 2) + penguin_offset_y);
	c_left->setScaleY(4.4f);

	c_right->setPositionX((width / 2) + 136.0f + penguin_offset_x);
	c_right->setPositionY((height / 2) + penguin_offset_y);
	c_right->setScaleY(4.4f);

	auto mods_sprite = ButtonSprite::create(
		"Mods", 220, 0, 0.7f, false, "bigFont.fnt", "GJ_button_05.png", 30.0f);

	auto mods_button = CCMenuItemSpriteExtra::create(
		mods_sprite, nullptr, this,
		static_cast<cocos2d::SEL_MenuHandler>(&AboutSettingsPage::onMods));

	this->_internal_menu->addChild(mods_button);
	this->_menu_objects.push_back(mods_button);

	mods_button->setPosition((-_window_dimensions.width / 2) + width - 30.0f, -(_window_dimensions.height / 2) + 40.0f);

	auto source_sprite = ButtonSprite::create(
		"Source", 220, 0, 0.5f, false, "bigFont.fnt", "GJ_button_04.png", 25.0f);

	auto source_button = CCMenuItemSpriteExtra::create(
		source_sprite, nullptr, this,
		static_cast<cocos2d::SEL_MenuHandler>(&AboutSettingsPage::onSource));

	this->_internal_menu->addChild(source_button);
	this->_menu_objects.push_back(source_button);

	source_button->setPosition((-_window_dimensions.width / 2) + width - 120.0f, -(_window_dimensions.height / 2) + 40.0f);
}

void AboutSettingsPage::onMods(cocos2d::CCObject*) {
	geode::openModsList();
}

void AboutSettingsPage::onSource(cocos2d::CCObject*)
{
	cocos2d::CCApplication::sharedApplication()->openURL(Config::SOURCE_URL);
}
