#pragma once
#ifndef GAME_VARIABLES_HPP
#define GAME_VARIABLES_HPP

namespace GameVariable {
	// 00xx - base game variables
	constexpr auto EDITOR_FOLLOW_PLAYER = "0001";
	constexpr auto FLIP_TWO_PLAYER_CONTROLS = "0010";
	constexpr auto LIMIT_CONTROLS = "0011";
	constexpr auto INCREASED_UNDO = "0013";
	constexpr auto DISABLE_EXPLOSION_SHAKE = "0014";
	constexpr auto FLIP_PAUSE = "0015";
	constexpr auto NO_SONG_LIMIT = "0018";
	constexpr auto LOAD_SONGS_MEMORY = "0019";
	constexpr auto HIGHER_AUDIO_QUALITY = "0022";
	constexpr auto SMOOTH_FIX = "0023";
	constexpr auto SHOW_CURSOR = "0024";
	constexpr auto AUTOMATIC_RETRY = "0026";
	constexpr auto AUTOMATIC_CHECKPOINTS = "0027";
	constexpr auto DISABLE_THUMBSTICK = "0028";
	constexpr auto VERTICAL_SYNC = "0030";
	constexpr auto CHANGE_SONG_LOCATION = "0033";
	constexpr auto SMALL_EDITOR_STEP = "0035";

	// 30xx - variables not set in the settings ui
	constexpr auto BYPASS_VERIFY = "3001";
	constexpr auto ALL_ICON_BYPASS = "3002";
	constexpr auto IS_MODERATOR = "3003";
	constexpr auto SHOWN_RATE_DIALOG = "3004";
	constexpr auto SECRET_COMPLETED = "3005";

	constexpr auto ICON_GLOW = "3097";
	constexpr auto SHOW_PERCENTAGE = "3098";
	constexpr auto BYPASS_PACKAGE_CHECK = "3099";

	// 40xx - variables set in settings ui, mod settings
	constexpr auto SHOW_TEXTURE_SETTINGS = "4001";
	constexpr auto OVERLAY_SPEEDHACK_ENABLED = "4002";

	constexpr auto DISABLE_MENU_MUSIC = "4077";
	constexpr auto SHOW_INFO_LABEL = "4078";
	constexpr auto DISABLE_RICH_PRESENCE = "4079";
	constexpr auto ENABLE_FPS_BYPASS = "4080";
	constexpr auto HIDE_PRACTICE_UI = "4081";
	constexpr auto DISABLE_SESSION = "4082";
	constexpr auto SHOW_PRACTICE_HITBOXES = "4083";
	constexpr auto SHOW_HITBOXES_ON_DEATH = "4084";
	constexpr auto SHOW_EDITOR_HITBOXES = "4085";
	constexpr auto ADVANCED_COLOR_SELECT = "4086";
	constexpr auto COPY_HACK = "4087";
	constexpr auto KEEP_AWAKE = "4088";
	constexpr auto ACCURATE_PERCENTAGE = "4089";
	constexpr auto AUTOMATIC_SESSION_LEGACY = "4090";
	constexpr auto SHOW_UNUSED_OBJS = "4091";
	constexpr auto PERSISTENT_CLIPBOARD = "4092";
	constexpr auto SPEED_CONTROLS = "4093";
	constexpr auto REPLAY_CONTROLS = "4094";
	constexpr auto FAST_PRACTICE_RESET = "4095";
	constexpr auto SHOW_RESTART = "4096";
	constexpr auto UNLOCK_ALL = "4097";
	constexpr auto PRACTICE_MUSIC = "4098";
	constexpr auto IGNORE_DAMAGE = "4099";

	// 50xx - mod int variables
	constexpr auto OVERLAY_MODIFIER = "5093";
	constexpr auto OVERLAY_KEYBIND = "5094";
	constexpr auto OVERLAY_SCALE = "5095";
	constexpr auto SPEED_INTERVAL = "5097";
	constexpr auto FPS_BYPASS = "5098";
	constexpr auto SPEED_INTERVAL_LEGACY = "5099";
}

#endif
