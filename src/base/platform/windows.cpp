#include "base/platform_helper.hpp"

#include "classes/callbacks/levelimportcallback.hpp"

bool PlatformHelper::is_gd_installed() {
	// unimplemented
	return true;
}

void PlatformHelper::keep_screen_awake() { }

void PlatformHelper::remove_screen_awake() { }

void PlatformHelper::open_texture_picker() {
	// unimplemented
}

void PlatformHelper::apply_classic_pack() {
	// unimplemented
}

void PlatformHelper::wipe_textures_directory() {
	// unimplemented
}

void PlatformHelper::export_level(std::string name, std::string export_string) {
	using namespace geode::utils;

	if (name.empty()) {
		name = "Unnamed";
	}

	file::pick(file::PickMode::SaveFile, {
		.defaultPath = fmt::format("{}.gmd", name),
		.filters = {
			{"GD Level Files", {"*.gmd"}}
		},
	}).listen(
		[=](geode::Result<std::filesystem::path>* result) {
			if (!result->ok()) {
				return;
			}

			auto filename = result->unwrap();

			auto out = std::ofstream(filename, std::ofstream::out);

			out << export_string;

			out.close();

			geode::Loader::get()->queueInMainThread([=]() {
				auto message = fmt::format("Your level was exported to <co>{}</c>.", filename.filename().string());

				geode::createQuickPopup("Level Exported!", message, "OK", "Show", [filename](auto, bool btn2) {
					if (btn2) {
						auto savePath = filename.parent_path();
						geode::utils::file::openFolder(savePath);
					}
				});
			});
		}
	);
}

void PlatformHelper::import_level() {
	using namespace geode::utils;

	file::pick(file::PickMode::OpenFile, {
		.defaultPath = std::nullopt,
		.filters = {
			{"GD Level Files", {"*.gmd"}}
		},
	}).listen(
		[=](geode::Result<std::filesystem::path>* result) {
			if (!result->ok()) {
				return;
			}

			LevelImportCallback::importLevel(result->unwrap());
		}
	);
}

void PlatformHelper::loaded_to_menu() { }

std::string PlatformHelper::get_textures_directory() {
	// unimplemented
	return "";
}

bool PlatformHelper::is_controller_connected() {
	// unimplemented
	return false;
}

std::string PlatformHelper::get_secondary_assets_directory() {
	// unimplemented
	return "";
}

std::string PlatformHelper::get_save_directory() {
	// unimplemented
	return "";
}

bool PlatformHelper::is_launcher_build() {
	return false;
}

bool PlatformHelper::is_screen_restricted() {
	return false;
}

void PlatformHelper::toggle_is_screen_restricted() { }

void PlatformHelper::capture_cursor() {
	// unimplemented
}

void PlatformHelper::release_cursor() {
	// unimplemented
}
