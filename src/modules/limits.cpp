#include <Geode/Geode.hpp>
#include <Geode/modify/SongOptionsLayer.hpp>

struct ExtendedSongOptionsLayer : geode::Modify<ExtendedSongOptionsLayer, SongOptionsLayer> {
	bool init(LevelSettingsObject* settings) {
		if (!SongOptionsLayer::init(settings)) {
			return false;
		}

		this->m_offsetInput->m_maxLabelLength = 5;

		auto songOffset = settings->m_songOffset;
		if (songOffset > 0) {
			this->m_offsetInput->setString(fmt::format("{:.2f}", songOffset));
		}

		return true;
	}
};

$execute {
	std::vector<uint8_t> max_length = { 0x4B, 0xEA, 0x60, 0x60 };

#if defined(GEODE_IS_ANDROID)
		// editor length patches
	// original constant = 0x476A6000 => 60000.0f
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1EB0C4),
		max_length
	);

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1E37CC),
		max_length
	);

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1DB058),
		max_length
	);

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1DAC24),
		max_length
	);

	// 0x476A7E00
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1E39B8),
		max_length
	);

	// object count patches
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1EB0EC),
		{0x04, 0xE0}
	);

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x1EBB9C),
		{0xC3, 0xE7}
	);
#elif defined(GEODE_IS_WINDOWS)
	// original constant = 0x476A6000 => 60000.0f
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x4b45c),
		max_length
	);

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x4b52a),
		max_length
	);

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x14a74c),
		max_length
	);

	// 0x476A7E00
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x4c9f9),
		max_length
	);

	// object count
	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x476b7),
		{0xeb, 0x0b}
	);

	(void)geode::Mod::get()->patch(
		reinterpret_cast<void*>(geode::base::get() + 0x48eda),
		{0xeb, 0x0a}
	);
#else
#error Missing patches (editor length bypass)
#endif
}
