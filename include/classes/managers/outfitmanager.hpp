#pragma once

#ifndef CLASSES_MANAGERS_OUTFITMANAGER_HPP
#define CLASSES_MANAGERS_OUTFITMANAGER_HPP

#include <array>
#include <optional>

#include <Geode/Geode.hpp>

class OutfitManager {
public:
	static OutfitManager& get_manager();
	cocos2d::ccColor3B get_color(int idx);

	int vanilla_color_for_pos(int pos);
	int color_for_pos(int pos);
	int max_for_type(IconType type);
	int vanilla_max_for_type(IconType type);

	bool is_custom(IconType type, int id);

	int max_colors();

private:
	geode::Result<cocos2d::ccColor3B> get_color_internal(int idx);
	geode::Result<int> color_for_pos_internal(int idx);

	std::optional<matjson::Value> _outfits;

	void init();

	OutfitManager();
};

#endif //CLASSES_MANAGERS_OUTFITMANAGER_HPP
