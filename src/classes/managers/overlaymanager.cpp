#include <Geode/Geode.hpp>

#include <array>

#include "classes/managers/overlaymanager.hpp"
#include "base/game_variables.hpp"

constexpr float DPI_NORMAL_FACTOR = 432.0f;

OverlayManager& OverlayManager::get() {
	static OverlayManager _manager_instance;
	return _manager_instance;
}

void OverlayManager::registerOverlay(std::function<void()> overlay) {
	this->_overlays.push_back(overlay);
}

void OverlayManager::render() const {
	for (const auto& overlay : this->_overlays) {
		overlay();
	}
}

float OverlayManager::scaleFactor() const {
	return this->_scaleFactor;
}

ImFont* OverlayManager::getFont(Overlay::Font id) const {
	return _fonts.at(id).at(_fontScale);
}

void OverlayManager::setupFont(Overlay::Font id, const char* fontPath, float fontScale) {
	// we keep the font data alive ourselves
	if (!_fontData.contains(fontPath)) {
		unsigned long size = 0;
		auto full_path = cocos2d::CCFileUtils::sharedFileUtils()->fullPathForFilename(fontPath, false);
		auto font_buffer = cocos2d::CCFileUtils::sharedFileUtils()->getFileData(full_path.c_str(), "rb", &size);

		if (font_buffer == nullptr || size <= 0) {
			return;
		}

		_fontData[fontPath] = std::vector(font_buffer, font_buffer + size);
	}

	auto& data = _fontData[fontPath];

	ImGuiIO& io = ImGui::GetIO();

	constexpr auto fontSizes = std::to_array<std::pair<Overlay::FontScale, float>>({
		{ Overlay::FontScale::Small, 0.75f },
		{ Overlay::FontScale::Base, 1.0f },
		{ Overlay::FontScale::Medium, 1.25f },
		{ Overlay::FontScale::Large, 1.5f },
		{ Overlay::FontScale::ExtraLarge, 2.0f },
	});

	for (const auto& [type, scale] : fontSizes) {
		ImFontConfig fontCfg;
		fontCfg.FontDataOwnedByAtlas = false;

		_fonts[id][type] = io.Fonts->AddFontFromMemoryTTF(data.data(), static_cast<int>(data.size()), fontScale * scale, &fontCfg);
	}
}

void OverlayManager::setup() {
#ifdef GEODE_IS_ANDROID
	// default font with increased size. just like the example (wow)
	auto dpi = static_cast<float>(cocos2d::CCDevice::getDPI());
	auto scaleFactor = dpi / DPI_NORMAL_FACTOR;
#else
	auto scaleFactor = 0.6f;
#endif

	_scaleFactor = scaleFactor;

	auto fontSize = 20.0f;

	this->setupFont(Overlay::Font::Monospace, "FiraMono-Regular.ttf"_spr, fontSize);
	this->setupFont(Overlay::Font::SansSerif, "muli-sb.ttf"_spr, fontSize);

#ifdef GEODE_IS_ANDROID
	ImGui::GetStyle().TouchExtraPadding = ImVec2(5.0f * 3.5f, 5.0f * 3.5f);
#endif

	_initialized = true;
}

void OverlayManager::setScale(float scale) {
	_scale = scale;

	// this is dumb code but it does make the menus look better (very mildly)
	if (scale > 1.5f) {
		ImGui::GetIO().FontGlobalScale = scale / 2.0f;
		_fontScale = Overlay::FontScale::ExtraLarge;
	} else if (scale > 1.25f) {
		ImGui::GetIO().FontGlobalScale = scale / 1.5f;
		_fontScale = Overlay::FontScale::Large;
	} else if (scale > 1.0f) {
		ImGui::GetIO().FontGlobalScale = scale / 1.25f;
		_fontScale = Overlay::FontScale::Medium;
	} else if (scale > 0.75f) {
		ImGui::GetIO().FontGlobalScale = scale;
		_fontScale = Overlay::FontScale::Base;
	} else {
		ImGui::GetIO().FontGlobalScale = scale / 0.75f;
		_fontScale = Overlay::FontScale::Small;
	}
}

float OverlayManager::scale() const {
	return _scale;
}

bool OverlayManager::initialized() const {
	return _initialized;
}

const char* OverlayManager::getKeybindName() {
	auto currentKey = static_cast<cocos2d::enumKeyCodes>(GameManager::sharedState()->getIntGameVariable(GameVariable::OVERLAY_KEYBIND));
	if (currentKey == cocos2d::enumKeyCodes::KEY_None) {
		currentKey = cocos2d::enumKeyCodes::KEY_Tab;
	}

	if (currentKey == cocos2d::enumKeyCodes::KEY_C) {
		return "C";
	}

	auto keyString = cocos2d::CCDirector::sharedDirector()->getKeyboardDispatcher()->keyToString(currentKey);
	if (keyString == nullptr) {
		geode::log::info("using unknown keybind: {}", static_cast<int>(currentKey));
		return "Unknown";
	}

	return keyString;
}
