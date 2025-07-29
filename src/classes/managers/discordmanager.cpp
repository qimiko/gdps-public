#include "classes/managers/discordmanager.hpp"

#include <discord-rpc.hpp>

#include <chrono>
#include <fmt/chrono.h>

constexpr auto APPLICATION_ID = "497656410687930370";

DiscordManager::DiscordManager() {
	this->init();
}

DiscordManager::~DiscordManager() {
	this->close();
}

void DiscordManager::init() {
	if (m_connectionReady || m_initializingConnection) {
		return;
	}

	m_initializingConnection = true;

	discord::RPCManager::get()
		.setClientID(APPLICATION_ID)
		.onReady([this](const discord::User& user) {
			m_connectionReady = true;
			m_initializingConnection = false;

			geode::log::debug("discord rpc - connected");
		})
		.onDisconnected([this](int code, std::string_view msg) {
			m_connectionReady = false;
			m_initializingConnection = false;
		})
		.onErrored([this](int code, std::string_view msg) {
			m_connectionReady = false;
			m_initializingConnection = false;

			geode::log::warn("discord rpc - error: {} ({})", msg, code);
		})
		.initialize();
}

bool DiscordManager::status() const {
	return m_connectionReady;
}

void DiscordManager::close() {
	if (!m_connectionReady) {
		return;
	}

	discord::RPCManager::get().shutdown();

	m_connectionReady = false;
}

void DiscordManager::send(DiscordRichPresence& activity) {
/*
	geode::log::info("update Discord presence:\ndetails={}, state={}, startTimestamp={}, smallImageText={}, smallImageKey={}",
		activity.details,
		activity.state,
		std::chrono::sys_seconds{std::chrono::seconds{activity.startTimestamp}},
		activity.smallImageText,
		activity.smallImageKey
	);
*/

	discord::RPCManager::get().getPresence()
		.setDetails(activity.details)
		.setState(activity.state)
		.setStartTimestamp(activity.startTimestamp)
		.setSmallImageKey(activity.smallImageKey)
		.setSmallImageText(activity.smallImageText)
		.setStatusDisplayType(activity.swapDisplayType
				? discord::StatusDisplayType::Details
				: discord::StatusDisplayType::Name)
		.setLargeImageKey("logo")
		.setLargeImageText(GameManager::sharedState()->m_playerName.c_str())
		.refresh();
}

void DiscordManager::checkIdleCounter() {
	if (m_idleCounter == 0) {
		DiscordRichPresence presence {};
		presence.state = "Idle";

		this->send(presence);
	}
}

void DiscordManager::incrementIdleCounter() {
	m_idleCounter++;
}

void DiscordManager::decrementIdleCounter() {
	if (m_idleCounter > 0) {
		m_idleCounter--;
	}

	this->checkIdleCounter();
}

DiscordManager& DiscordManager::get() {
	static DiscordManager _managerInstance;
	return _managerInstance;
}
