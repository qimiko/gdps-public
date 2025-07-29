#include "classes/managers/outfitmanager.hpp"

OutfitManager& OutfitManager::get_manager() {
	static OutfitManager _manager_instance;
	return _manager_instance;
}

geode::Result<cocos2d::ccColor3B> OutfitManager::get_color_internal(int idx) {
	if (!_outfits) {
		return geode::Err("Outfits not defined");
	}

	GEODE_UNWRAP_INTO(auto colors, _outfits->get<std::vector<matjson::Value>>("playerColors"));
	if (colors.size() < static_cast<std::size_t>(idx)) {
		return geode::Err("Color index out of range");
	}

	GEODE_UNWRAP_INTO(auto colorInt, colors.at(idx).asInt());
	return geode::Ok<cocos2d::ccColor3B>({
		static_cast<GLubyte>(((colorInt >> 16) & 0xFF)),
		static_cast<GLubyte>(((colorInt >> 8) & 0xFF)),
		static_cast<GLubyte>((colorInt & 0xFF))
	});
}

cocos2d::ccColor3B OutfitManager::get_color(int idx) {
	return this->get_color_internal(idx)
		.unwrapOr<cocos2d::ccColor3B>({0xFF, 0xFF, 0xFF});
}

int OutfitManager::vanilla_color_for_pos(int pos) {
	switch (pos) {
	case 4:
		return 16;
	case 5:
		return 4;
	case 6:
		return 5;
	case 7:
		return 6;
	case 8:
		return 13;
	case 9:
		return 7;
	case 10:
		return 8;
	case 11:
		return 9;
	case 12:
		return 19;
	case 13:
		return 10;
	case 15:
		return 11;
	case 16:
		return 12;
	case 19:
		return 15;
	default:
		return pos;
	}
}

geode::Result<int> OutfitManager::color_for_pos_internal(int pos) {
	if (!_outfits) {
		return geode::Err("Outfits not defined");
	}

	GEODE_UNWRAP_INTO(auto colors, _outfits->get<std::vector<matjson::Value>>("colorDispOrder"));
	if (static_cast<std::uint32_t>(pos) >= colors.size()) {
		return geode::Err("Color position out of range");
	}

	return colors.at(pos).as<int>();
}

int OutfitManager::color_for_pos(int pos) {
	return this->color_for_pos_internal(pos)
		.unwrapOr(vanilla_color_for_pos(pos));
}

int OutfitManager::vanilla_max_for_type(IconType type) {
	switch (type) {
		case IconType::Cube:
			return 48;
		case IconType::Ship:
			return 18;
		case IconType::Ball:
			return 10;
		case IconType::Ufo:
			return 10;
		case IconType::Special:
			return 2;
		case IconType::Wave:
			return 1;
		default:
			return 0;
	}
}

bool OutfitManager::is_custom(IconType type, int id) {
	return id > vanilla_max_for_type(type);
}

int OutfitManager::max_for_type(IconType type) {
	if (!_outfits) {
		return vanilla_max_for_type(type);
	}

	const char* key = nullptr;
	switch (type) {
		case IconType::Cube:
			key = "playerMaxId";
			break;
		case IconType::Ship:
			key = "shipMaxId";
			break;
		case IconType::Ball:
			key = "ballMaxId";
			break;
		case IconType::Ufo:
			key = "birdMaxId";
			break;
		case IconType::Special:
			key = "specialMaxId";
			break;
		case IconType::Wave:
			key = "dartMaxId";
			break;
		default:
			return 0;
	}

	if (key == nullptr) {
		return 1;
	}

	auto maxStr = _outfits->get<int>(key);
	if (!maxStr) {
		return 1;
	}

	return std::max(maxStr.unwrap(), 1);
}

int OutfitManager::max_colors() {
	if (!_outfits) {
		return 23;
	}

	auto maxColorStr = _outfits->get<int>("colorMaxDispId");
	if (!maxColorStr) {
		return 23;
	}

	return maxColorStr.unwrap();
}

void OutfitManager::init() {
	auto res = geode::utils::file::readJsonFromResources("outfits.json"_spr);
	if (!res) {
		geode::log::warn("Failed to read player outfits: {}", res.unwrapErr());
		return;
	}

	_outfits = res.unwrap();
}

OutfitManager::OutfitManager() {
	init();
};
