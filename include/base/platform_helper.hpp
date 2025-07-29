#pragma once
#ifndef BASE_PLATFORM_HELPER_H
#define BASE_PLATFORM_HELPER_H

#include <string>

namespace PlatformHelper {
bool is_gd_installed();
void keep_screen_awake();
void remove_screen_awake();
void open_texture_picker();
void apply_classic_pack();
void wipe_textures_directory();
void export_level(std::string name, std::string export_string);
void import_level();
void loaded_to_menu();
std::string get_textures_directory();
bool is_controller_connected();
std::string get_secondary_assets_directory();
std::string get_save_directory();
bool is_launcher_build();
bool is_screen_restricted();
void toggle_is_screen_restricted();
void capture_cursor();
void release_cursor();
}

#endif