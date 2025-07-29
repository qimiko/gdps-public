#include <string_view>

#include <Geode/Geode.hpp>
#include <Geode/modify/CCHttpClient.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/AppDelegate.hpp>
#include <Geode/modify/DS_Dictionary.hpp>
#include <Geode/modify/AccountRegisterLayer.hpp>
#include <Geode/modify/AccountLoginLayer.hpp>
#include <Geode/modify/UploadPopup.hpp>
#include <Geode/modify/GameLevelManager.hpp>
#include <Geode/modify/AccountHelpLayer.hpp>

#include <Geode/cocos/support/base64.h>

#include "base/platform_helper.hpp"
#include "base/config.hpp"
#include "base/game_variables.hpp"

#include "classes/loadingfailedhijacklayer.hpp"

struct CustomCCHttpClient : geode::Modify<CustomCCHttpClient, cocos2d::extension::CCHttpClient> {
	void send(cocos2d::extension::CCHttpRequest* x) {
		auto url = std::string(x->getUrl());

		constexpr std::string_view url_boomlings = "http://www.boomlings.com/database";
		constexpr std::string_view url_replacement = GDMOD_ENDPOINT_BASE_URL;

		if (auto host_pos = url.find(url_boomlings); host_pos != std::string_view::npos) {
			url.replace(host_pos, url_boomlings.length(), url_replacement);
			x->setUrl(url.c_str());

			x->setHeaders(gd::vector<gd::string>({
				gd::string(fmt::format("User-Agent: {}", Config::USER_AGENT))
      }));
		}

		cocos2d::extension::CCHttpClient::send(x);
	}
};

#ifdef GEODE_IS_ANDROID
struct SavePathGameManager : geode::Modify<SavePathGameManager, GameManager> {
	void reloadAllStep3() {
		auto file = cocos2d::CCFileUtils::sharedFileUtils();
		file->setAndroidPath(PlatformHelper::get_save_directory());

		GameManager::reloadAllStep3();
	}
};

struct FixLoadAppDelegate : geode::Modify<FixLoadAppDelegate, AppDelegate> {
	bool applicationDidFinishLaunching() {
		this->m_paused = false;

		auto file = cocos2d::CCFileUtils::sharedFileUtils();
		file->setAndroidPath(PlatformHelper::get_save_directory());

		file->addTexturePack({
			"gdps-pack",
			{PlatformHelper::get_textures_directory()}
		});

		this->setupGLView();

		cocos2d::CCDirector::sharedDirector()->setProjection(cocos2d::kCCDirectorProjection2D);
		CCContentManager::sharedManager();

		ObjectDecoder::sharedDecoder()->setDelegate(GJObjectDecoder::sharedDecoder());

		GameManager::sharedState();
		LocalLevelManager::sharedState();

		GameManager::sharedState()->loadVideoSettings();
		GameManager::sharedState()->startUpdate();

		this->m_enableFPSTricks = false;

		cocos2d::CCTexture2D::setDefaultAlphaPixelFormat(cocos2d::kCCTexture2DPixelFormat_RGBA8888);
		cocos2d::CCTexture2D::PVRImagesHavePremultipliedAlpha(false);
		cocos2d::CCDirector::sharedDirector()->setDepthTest(false);
		cocos2d::CCDirector::sharedDirector()->setAnimationInterval(1.0f / 60.0f);

		this->m_isIOS = false;

		// AdToolbox::setupAds() // stubbed on full version

		if (LoadingFailedHijackLayer::hasDestroyedState()) {
			auto loadingFailedLayer = LoadingFailedHijackLayer::scene();
			cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->setTargetPrio(-500);
			cocos2d::CCDirector::sharedDirector()->runWithScene(loadingFailedLayer);
			return true;
		}

		auto loadingLayer = LoadingLayer::scene(false);
		cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->setTargetPrio(-500);
		cocos2d::CCDirector::sharedDirector()->runWithScene(loadingLayer);

		return true;
	}

/*
	void applicationWillEnterForeground() {
		cocos2d::CCDirector::sharedDirector()->stopAnimation();
		cocos2d::CCDirector::sharedDirector()->resume();
		cocos2d::CCDirector::sharedDirector()->startAnimation();

		FMODAudioEngine::sharedEngine()->start();
		FMODAudioEngine::sharedEngine()->update(0.0f);

		if (PlatformToolbox::shouldResumeSound()) {
				this->resumeSound();
		}

		PlatformToolbox::onNativeResume();
		PlatformToolbox::onToggleKeyboard();

		cocos2d::CCDirector::sharedDirector()->getActionManager()
				->removeActionByTag(1, GameManager::sharedState());

		auto application_enter_sequence = cocos2d::CCSequence::create(
				cocos2d::CCDelayTime::create(0.0f),
				cocos2d::CCCallFunc::create(
					GameManager::sharedState(),
					static_cast<cocos2d::SEL_CallFunc>(&GameManager::applicationWillEnterForeground)
				)
		);

		application_enter_sequence->setTag(1);

		cocos2d::CCDirector::sharedDirector()->getActionManager()
			->addAction(
					application_enter_sequence,
					GameManager::sharedState(),
					false
		);

		FMODAudioEngine::sharedEngine()->update(0.0f);
	}
*/

	static void onModify(auto& self) {
		if (!self.setHookPriority("AppDelegate::applicationDidFinishLaunching", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for applicationDidFinishLaunching");
		}
	}
};
#else

#include <Geode/modify/LoadingLayer.hpp>

// we can do a much less invasive hook on windows as we're not fixing anything
struct FixLoadLoadingLayer : geode::Modify<FixLoadLoadingLayer, LoadingLayer> {
	static LoadingLayer* create(bool reload) {
		if (LoadingFailedHijackLayer::hasDestroyedState() && !reload) {
			// spooky...
			return reinterpret_cast<LoadingLayer*>(LoadingFailedHijackLayer::create());
		}

		if (!reload) {
			cocos2d::CCTexture2D::setDefaultAlphaPixelFormat(cocos2d::kCCTexture2DPixelFormat_RGBA8888);
			cocos2d::CCTexture2D::PVRImagesHavePremultipliedAlpha(false);
		}

		return LoadingLayer::create(reload);
	}
};

#endif

std::string ZipUtils_decompressString2Wrap(unsigned char* data, bool use_decrypt, int size) {
	// this wrapper exists to remove all uses of strlen
	// strlen would cause a crash if placed at page end without null termination
	if (data == nullptr || size == 0) {
		geode::log::error("ds2_wrap:: nulld fail");
		LoadingFailedHijackLayer::setCode(100);
		return "";
	}

	// check if first byte invalid
	if (*data == 0x00) {
		geode::log::error("ds2_wrap:: dfb fail");
		LoadingFailedHijackLayer::setCode(101);
		return "";
	}

	if (use_decrypt) {
		// in-place xor. why not
		for (auto i = 0u; i < static_cast<std::uint32_t>(size); i++) {
				data[i] ^= 0x0B;
		}

		// this is the behavior of the original func
		// auto ds = std::string(reinterpret_cast<const char*>(data), size);
		// auto decrypted = cocos2d::ZipUtils::encryptDecrypt(ds, 0x0B);
		// decoded_length = cocos2d::base64Decode(data, size, &b64decoded);
	}

	unsigned char* b64decoded = nullptr;
	auto decoded_length = cocos2d::base64Decode(data, size, &b64decoded, true);

	if (decoded_length < 1) {
		delete[] b64decoded;

		geode::log::error("ds2_wrap:: dcl fail, dl {}", decoded_length);
		LoadingFailedHijackLayer::setCode(102);
		return "";
	} else {
		unsigned char* inflated = nullptr;

		auto inflated_length = cocos2d::ZipUtils::ccInflateMemory(b64decoded, decoded_length, &inflated);
		if (inflated_length < 1) {
			delete[] b64decoded;
			delete[] inflated;

			geode::log::error("ds2_wrap:: infll fail, il {}", inflated_length);
			LoadingFailedHijackLayer::setCode(103);
			return "";
		}

		auto output_str = std::string(reinterpret_cast<char*>(inflated), inflated_length);

		delete[] b64decoded;
		delete[] inflated;

		return output_str;
	}
}

struct SaveFixDSDictionary : geode::Modify<SaveFixDSDictionary, DS_Dictionary> {
	bool loadRootSubDictFromCompressedFile(const char* filename) {
		if (LoadingFailedHijackLayer::hasDestroyedState()) {
			// ignore load if state already broken
			return false;
		}

#ifdef GEODE_IS_ANDROID
		// FileOperation isn't a thing on windows
		auto fpath = std::string(FileOperation::getFilePath()) + filename;
#else
		auto fpath = geode::dirs::getSaveDir() / filename;
#endif

			geode::log::debug("loading save file {}", filename);

			auto ifs = std::ifstream(fpath);
			if (!ifs) {
				return false;
			}

			auto data = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	/*
			spdlog::get("global")->info(
				"lrsdfcf({}):: ifstream size {}, fb {:#04x} lb {:#04x}",
				filename, data.size(), data[0x00], data[data.size() - 1]);
	*/

			// detect empty save files and warn about corruption
			if (data.empty()) {
				LoadingFailedHijackLayer::setCode(100);
				LoadingFailedHijackLayer::destroyGameState();
				return false;
			}

			// handle unencoded data
			if (this->loadRootSubDictFromString(data)) {
				return true;
			}

			auto dfile = cocos2d::ZipUtils::decompressString(data, true);


			if (this->loadRootSubDictFromString(dfile)) {
				return true;
			}

			// this means our save data is not functional in some way
			// destroy state and show corruption layer
			LoadingFailedHijackLayer::destroyGameState();

			return false;
	}

	// disable save compression
	bool saveRootSubDictToCompressedFile(const char* filename) {
		// disable saving if we're in a no save zone
		if (LoadingFailedHijackLayer::hasDestroyedState()) {
			return true;
		}

		return this->saveRootSubDictToFile(filename);
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("DS_Dictionary::loadRootSubDictFromCompressedFile", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for loadRootSubDictFromCompressedFile");
		}

		if (!self.setHookPriority("DS_Dictionary::saveRootSubDictToCompressedFile", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for saveRootSubDictToCompressedFile");
		}
	}
};

struct CustomAccountRegisterLayer : geode::Modify<CustomAccountRegisterLayer, AccountRegisterLayer> {
	void registerAccountFinished() {
		auto f = FLAlertLayer::create(
			this, "Created",
			"Thank you for registering. You may <cg>log into</c> your 1.9 GDPS account.",
			"OK", nullptr, 320.0f
		);
		f->setTag(1);
		f->show();

		this->m_loadingCircle->setVisible(false);
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("AccountRegisterLayer::registerAccountFinished", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for registerAccountFinished");
		}
	}
};

struct CustomUploadPopup : geode::Modify<CustomUploadPopup, UploadPopup> {
	void onSessionInfo(cocos2d::CCObject*) {
		FLAlertLayer::create(
			nullptr, "Session Info",
			"All <cy>registered users</c> must have an <cg>active session</c> to upload levels. Sessions can be created in the <co>online levels menu</c> or online, in the <cl>tools page</c>.",
			"OK", nullptr, 350.0f
		)->show();
	}

	void levelUploadFailed(GJGameLevel* lvl) {
		UploadPopup::levelUploadFailed(lvl);

		this->m_textArea->setString("Upload Failed. Please try again later.\nDid you make a session?");

		auto infoSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
		infoSprite->setScale(0.6f);
		auto infoBtn = CCMenuItemSpriteExtra::create(
			infoSprite, nullptr, this,
			static_cast<cocos2d::SEL_MenuHandler>(&CustomUploadPopup::onSessionInfo)
		);
		infoBtn->setSizeMult(1.6f);

		this->m_buttonMenu->addChild(infoBtn);

		auto sz = cocos2d::CCDirector::sharedDirector()->getWinSize();
		infoBtn->setPosition(142.0, -17.0f);
	}
};

struct CustomGameLevelManager : geode::Modify<CustomGameLevelManager, GameLevelManager> {
	void onUploadCommentCompleted(gd::string response, gd::string key) {
		this->m_activeUploads->removeObjectForKey(key);

		auto level = this->levelIDFromCommentKey(key.c_str());

		if (response == "-1") {
			// generic failure

			if (this->m_commentUploadDelegate != nullptr) {
				this->m_commentUploadDelegate->commentUploadFailed(level);
			}

			this->resetTimerForKey(this->getPostCommentKey(level));
			return;
		}

		if (response == "-10") {
			// comment ban
			if (this->m_commentUploadDelegate != nullptr) {
				this->m_commentUploadDelegate->commentUploadFailed(level);
			}

			// reset comment timer for mods because it's annoying
			if (GameManager::sharedState()->getGameVariable(GameVariable::IS_MODERATOR)) {
				this->resetTimerForKey(this->getCommentKey(level, 0));
			}

			FLAlertLayer::create(nullptr, "Success!", "Your <cg>command</c> was successful.\nPlease ask the <cy>#support channel</c> if you have any questions.", "OK", nullptr)->show();
			return;
		}

		// success! (this means anything not -1 or -10 is successful. idk why he did it like that...)
		this->resetTimerForKey(this->getCommentKey(level, 0));

		if (this->m_commentUploadDelegate != nullptr) {
			this->m_commentUploadDelegate->commentUploadFinished(level);
		}
	}

	static void onModify(auto& self) {
		if (!self.setHookPriority("GameLevelManager::onUploadCommentCompleted", geode::Priority::Replace)) {
			geode::log::warn("failed to set hook priority for onUploadCommentCompleted");
		}
	}
};

struct CustomAccountHelpLayer : geode::Modify<CustomAccountHelpLayer, AccountHelpLayer> {
	void FLAlert_Clicked(FLAlertLayer* layer, bool btn) override {
		if (layer->getTag() == 5 && btn) {
			cocos2d::CCApplication::sharedApplication()->openURL(GDMOD_ENDPOINT_BASE_URL "/tools/");
			return;
		}

		AccountHelpLayer::FLAlert_Clicked(layer, btn);
	}
};

struct CustomAccountLoginLayer : geode::Modify<CustomAccountLoginLayer, AccountLoginLayer> {
	struct Fields {
		bool m_inSuccessPopup{false};
	};

	virtual void keyBackClicked() override {
		if (m_fields->m_inSuccessPopup) {
			return;
		}

		AccountLoginLayer::keyBackClicked();
	}

	virtual void loginAccountFinished(int account, int user) override {
		m_fields->m_inSuccessPopup = true;
		AccountLoginLayer::loginAccountFinished(account, user);
	}
};

// adding this to the bindings creates too many issues. might be the worst looking code in the mod :sob: i'm so sorry
// disables the whitespace for save data. from my tests, there's a ~33% size reduction so it's worth it

gd::string (* GEODE_WINDOWS(__thiscall) pugi_save_string_original)(pugi::xml_document*, const char*, unsigned int, pugi::xml_encoding) = nullptr;
gd::string pugi_save_string(pugi::xml_document* self, const char* indent, unsigned int flags, pugi::xml_encoding encoding) {
	return pugi_save_string_original(self, indent, pugi::format_raw, encoding);
}

bool (* GEODE_WINDOWS(__thiscall) pugi_save_file_original)(pugi::xml_document*, const char*,const char*, unsigned int, pugi::xml_encoding) = nullptr;
bool pugi_save_file(pugi::xml_document* self, const char* fname, const char* indent, unsigned int flags, pugi::xml_encoding encoding) {
	return pugi_save_file_original(self, fname, indent, pugi::format_raw, encoding);
}

$execute {
	auto stringAddr =
#if defined(GEODE_IS_WINDOWS)
	GetProcAddress(
		reinterpret_cast<HMODULE>(geode::base::getCocos()),
		"?save_string@xml_document@pugi@@QBE?AV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@PBDIW4xml_encoding@2@@Z"
	);
#elif defined(GEODE_IS_ANDROID)
	dlsym(dlopen("libcocos2dcpp.so", RTLD_NOW), "_ZNK4pugi12xml_document11save_stringEPKcjNS_12xml_encodingE");
#else
	nullptr;
#endif

	if (stringAddr == nullptr) {
		geode::log::warn("failed to hook save_string - fn not found");
		return;
	}

	auto stringRes = geode::Mod::get()->hook(
		reinterpret_cast<void*>(stringAddr),
		&pugi_save_string,
		"pugi::xml_document::save_string"
		GEODE_WINDOWS(, tulip::hook::TulipConvention::Thiscall)
	);
	if (!stringRes) {
		geode::log::warn("failed to hook save_string - {}", stringRes.unwrapErr());
		return;
	}

	pugi_save_string_original = reinterpret_cast<decltype(pugi_save_string_original)>(stringAddr);

	auto fileAddr =
#if defined(GEODE_IS_WINDOWS)
	GetProcAddress(
		reinterpret_cast<HMODULE>(geode::base::getCocos()),
		"?save_file@xml_document@pugi@@QBE_NPBD0IW4xml_encoding@2@@Z"
	);
#elif defined(GEODE_IS_ANDROID)
	dlsym(dlopen("libcocos2dcpp.so", RTLD_NOW), "_ZNK4pugi12xml_document9save_fileEPKcS2_jNS_12xml_encodingE");
#else
	nullptr;
#endif

	if (fileAddr == nullptr) {
		geode::log::warn("failed to hook save_file - fn not found");
		return;
	}

	auto fileRes = geode::Mod::get()->hook(
		reinterpret_cast<void*>(fileAddr),
		&pugi_save_file,
		"pugi::xml_document::save_file"
		GEODE_WINDOWS(, tulip::hook::TulipConvention::Thiscall)
	);
	if (!fileRes) {
		geode::log::warn("failed to hook save_file - {}", fileRes.unwrapErr());
		return;
	}

	pugi_save_file_original = reinterpret_cast<decltype(pugi_save_file_original)>(fileAddr);
}

#ifdef GEODE_IS_WINDOWS
$execute {
	// disables saving when entering the background, a known source of lag
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x28e02),
		{0x90, 0x90, 0x90}
	);
}
#endif
