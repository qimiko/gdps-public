#pragma once

#ifndef CLASSES_MANAGERS_DISCORDMANAGER_HPP
#define CLASSES_MANAGERS_DISCORDMANAGER_HPP

#include <Geode/Geode.hpp>

struct DiscordRichPresence {
	std::string details{};
	std::string state{};
	std::string smallImageText{};
	std::string smallImageKey{};
	std::uint64_t startTimestamp{};
	bool swapDisplayType{};
};

class DiscordManager {
	bool m_connectionReady{false};
	bool m_initializingConnection{false};
	int m_idleCounter{0};

	DiscordManager();

public:
	static DiscordManager& get();

	void init();
	void close();

	bool status() const;

	void send(DiscordRichPresence& activity);

	// when idle counter is 0, it's assumed that no overriding activities are happening
	// so the status should be idle
	void checkIdleCounter();
	void incrementIdleCounter();
	void decrementIdleCounter();

	~DiscordManager();

	DiscordManager(const DiscordManager&) = delete;
	DiscordManager(DiscordManager&&) = delete;
};

#endif
