#include <regex>

#include <pugixml.hpp>

#include <Geode/utils/base64.hpp>

#include "classes/callbacks/levelimportcallback.hpp"

void LevelImportCallback::goToScene() {
	if (toLevel_ == nullptr) {
		return;
	}

	auto editor_scene = EditLevelLayer::scene(reinterpret_cast<GJGameLevel*>(toLevel_));
	auto fade_scene = cocos2d::CCTransitionFade::create(0.5f, editor_scene);

	if (this->isNewLevel_) {
		auto warning_popup = FLAlertLayer::create(
			nullptr, "Info",
			"This level was created on a <cy>newer version</c> of the game.\n<cr>Some issues may occur during gameplay.</c>",
			"OK", nullptr);
		warning_popup->setTargetScene(editor_scene);
		warning_popup->show();
	}

	cocos2d::CCDirector::sharedDirector()->replaceScene(fade_scene);
}

void LevelImportCallback::onLevelImported(cocos2d::CCNode*) {
	goToScene();
}

void LevelImportCallback::FLAlert_Clicked(FLAlertLayer*, bool) {
	goToScene();
}

void importLevelInner(pugi::xml_document&& doc) {

	// plist parsing, tinyxml style
	// there's no validation on this at all. enjoy

	auto document = doc.child("d");
	if (!document) {
		geode::Loader::get()->queueInMainThread([]() {
			FLAlertLayer::create(
				nullptr,
				"Import Failed",
				"The latest version of gmd files is not supported. "
				"Only levels exported from the 1.9 client can be imported.",
				"OK",
				nullptr
			)->show();
		});
		return;
	}

	auto level = GameLevelManager::sharedState()->createNewLevel();
	GameLevelManager::sharedState()->m_createdNewLevel = true;

	auto is_new_level = false;
	auto is_valid_level = false;

	for (auto elem = document.first_child(); elem; elem = elem.next_sibling()) {
		if (strcmp(elem.name(), "k") == 0) {
			auto key = elem.text().get();

			elem = elem.next_sibling();
			if (!elem) {
				break;
			}

			auto value = elem.text();
			if (!value) {
				continue;
			}

			// what nice parsing we have
			if (strcmp(key, "kCEK") == 0) {
				auto obj_type = value.as_int();
				if (obj_type == 4) {
					is_valid_level = true;
				} else {
					break;
				}
			} else if (strcmp(key, "k2") == 0) {
				level->m_levelName = value.get();
			} else if (strcmp(key, "k3") == 0) {
				// undo the double encode
				auto unencodedDesc = value.get();
				auto desc = geode::utils::base64::decodeString(unencodedDesc).andThen([](const auto& d) {
					return geode::utils::base64::decodeString(d);
				}).unwrapOr(unencodedDesc);
				level->m_levelDesc = desc;
			} else if (strcmp(key, "k4") == 0) {
				level->m_levelString = value.get();
			} else if (strcmp(key, "k8") == 0) {
				level->m_audioTrack = value.as_int();
			} else if (strcmp(key, "k45") == 0) {
				level->m_songID = value.as_int();
			} else if (strcmp(key, "k50") == 0) {
				if (value.as_int() > 24) {
					is_new_level = true;
				}
			}
		}
	}

	if (!is_valid_level) {
		geode::Loader::get()->queueInMainThread([]() {
			FLAlertLayer::create(
				nullptr,
				"Import Failed",
				"A valid level was not provided for import.",
				"OK",
				nullptr
			)->show();
		});
		return;
	}

	level->m_levelType = GJLevelType::Editor;

	auto callback = LevelImportCallback::create();

	callback->setIsNewLevel(is_new_level);
	callback->setToLevel(level);

	auto cc_func = cocos2d::CCCallFuncN::create(callback, static_cast<cocos2d::SEL_CallFuncN>(&LevelImportCallback::onLevelImported));

	// this avoids a very yummy crash
	auto cc_sequence = cocos2d::CCSequence::createWithTwoActions(
		cocos2d::CCDelayTime::create(0.0f),
		cc_func
	);

	auto action_manager = cocos2d::CCDirector::sharedDirector()->getActionManager();
	action_manager->addAction(cc_sequence, callback, false);
}

void LevelImportCallback::importLevelBytes(const std::span<const std::uint8_t> data) {
	// cocos's imperfect base64 encode can leak garbage into the level string/description
	// our old perfect xml code would happily spit that garbage into gmd files
	// history says it's our problem if it doesn't import
	std::string str{data.begin(), data.end()};

	std::regex nullMatch{"\\0.*?<"};
	auto fixedData = std::regex_replace(str, nullMatch, "<");

	pugi::xml_document doc;
	if (auto result = doc.load_buffer(fixedData.data(), fixedData.size()); !result) {
		auto fmtMessage = fmt::format("A valid file was not provided for import: {}", result.description());
		geode::Loader::get()->queueInMainThread([=]() {
			FLAlertLayer::create(
				nullptr,
				"Import Failed",
				fmtMessage.c_str(),
				"OK",
				nullptr
			)->show();
		});
		return;
	}

	importLevelInner(std::move(doc));
}

void LevelImportCallback::importLevel(const std::filesystem::path path) {
	auto data = geode::utils::file::readBinary(path);
	if (!data) {
		auto fmtMessage = fmt::format("Unable to access file: {}", data.unwrapErr());
		geode::Loader::get()->queueInMainThread([=]() {
			FLAlertLayer::create(
				nullptr,
				"Import Failed",
				fmtMessage.c_str(),
				"OK",
				nullptr
			)->show();
		});
		return;
	}

	auto bytes = data.unwrap();
	importLevelBytes(bytes);
}

