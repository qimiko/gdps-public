#pragma once

#ifndef CLASSES_COMMANDS_COMMANDDISPATCHER_HPP
#define CLASSES_COMMANDS_RATELEVELDIALOG_HPP

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

#include <queue>
#include <optional>

using namespace geode::prelude;

class CommandDispatcherProtocol;

class CommandDispatcher {
	std::queue<std::string> m_queuedComments{};
	CommandDispatcherProtocol* m_delegate{nullptr};
	EventListener<web::WebTask> m_uploadListener{};
	std::optional<web::WebRequest> m_uploadRequest{};
	bool m_executing{false};
	int m_levelId{};

	void nextCommand();

	void onFinished();
	void onFailure();

public:
	CommandDispatcher(int levelId) : m_levelId{levelId} {}

	void queueCommand(const std::string&);
	std::size_t commandCount() const;

	void setDelegate(CommandDispatcherProtocol*);

	void beginExecution();
	void cancelExecution();
};

class CommandDispatcherProtocol {
public:
	virtual void onDispatchSuccess(CommandDispatcher*) {}
	virtual void onDispatchFailure(CommandDispatcher*) {}
};

#endif
