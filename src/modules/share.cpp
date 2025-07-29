#include <pugixml.hpp>

#include <Geode/Geode.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#include <Geode/cocos/support/base64.h>
#include <Geode/utils/base64.hpp>

#include "base/platform_helper.hpp"

struct ShareEditLevelLayer : geode::Modify<ShareEditLevelLayer, EditLevelLayer> {
	void onExport(cocos2d::CCObject*) {
		auto level = this->m_level;
		auto level_string = level->m_levelString;

		if (!level_string.empty()) {
			auto export_string = this->createLevelExportString();
			PlatformHelper::export_level(level->m_levelName, export_string);
		} else {
			FLAlertLayer::create(nullptr, "Empty", "You can't export an empty level.", "OK", nullptr, 300.0f)->show();
		}
	}

	std::string createLevelExportString() {
		auto level = this->m_level;

		// port of gdshare::exportLevel function at HJfod/GDShare-mod@38f00df3d1af115fb2ddca30b02d6acd12f89661/src/utils/gdshare.cpp#L150

		pugi::xml_document doc;
		auto data = doc.append_child("d");

		data.append_child("k").text().set("kCEK");
		data.append_child("i").text().set(4);

		data.append_child("k").text().set("k2");
		data.append_child("s").text().set(level->m_levelName.c_str());

		// gdshare double encodes description strings, and we have to replicate this
		auto desc = geode::utils::base64::encode(geode::utils::base64::encode(level->m_levelDesc.c_str()));

		data.append_child("k").text().set("k3");
		data.append_child("s").text().set(desc.c_str());

		data.append_child("k").text().set("k4");
		data.append_child("s").text().set(level->m_levelString.c_str());

		if (level->m_songID != 0) {
			data.append_child("k").text().set("k45");
			data.append_child("i").text().set(level->m_songID);
		} else {
			data.append_child("k").text().set("k8");
			data.append_child("i").text().set(level->m_audioTrack);
		}

		data.append_child("k").text().set("k13");
		data.append_child("t");

		data.append_child("k").text().set("k21");
		data.append_child("i").text().set(2);

		data.append_child("k").text().set("k50");
		data.append_child("i").text().set(24);

		std::ostringstream oss;
		doc.save(oss, "", pugi::format_raw | pugi::format_no_declaration);

		return oss.str();
	}

	bool init(GJGameLevel* lvl) {
		if (EditLevelLayer::init(lvl)) {
			auto share_btn = CCMenuItemSpriteExtra::create(
				geode::CircleButtonSprite::createWithSpriteFrameName(
					"exportIcon.png"_spr,
					0.85f,
					geode::CircleBaseColor::Green,
					geode::CircleBaseSize::Medium
				),
				nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&ShareEditLevelLayer::onExport)
			);

			auto menu = static_cast<cocos2d::CCMenu*>(this->getChildByID("level-actions-menu"));
			if (menu) {
				menu->addChild(share_btn);
				share_btn->setID("export-level-btn"_spr);
				menu->updateLayout();
			}

			return true;
		}

		return false;
	}
};

struct ShareLevelBrowserLayer : geode::Modify<ShareLevelBrowserLayer, LevelBrowserLayer> {
	void onImport(cocos2d::CCObject*) {
		PlatformHelper::import_level();
	}

	bool init(GJSearchObject* search) {
		if (!LevelBrowserLayer::init(search)) {
			return false;
		}

		if (search->m_searchType == SearchType::MyLevels) {
			auto import_button = CCMenuItemSpriteExtra::create(
				geode::CircleButtonSprite::createWithSpriteFrameName(
					"importIcon.png"_spr,
					0.85f,
					geode::CircleBaseColor::DarkPurple,
					geode::CircleBaseSize::Small
				),
				nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&ShareLevelBrowserLayer::onImport)
			);

			import_button->setID("import-level-btn"_spr);

			auto button_menu = reinterpret_cast<cocos2d::CCNode*>(
					this->getChildren()->objectAtIndex(9));

			if (geode::cast::typeinfo_cast<TextArea*>(button_menu) != nullptr) {
				// tap new offsets the amount of children by 1
				button_menu = reinterpret_cast<cocos2d::CCNode*>(
						this->getChildren()->objectAtIndex(10));
			}

			button_menu->addChild(import_button);
			import_button->setPositionY(50.0f);
		}

		return true;
	}
};
