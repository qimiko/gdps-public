#include "classes/commands/commanddispatcher.hpp"

#include "base/config.hpp"

namespace {
	std::string encodeUrlParam(std::string_view str) {
		std::ostringstream oss;
		oss.fill('0');
		oss << std::hex;

		for (auto c : str) {
			if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
				oss << c;
				continue;
			}

			oss << std::uppercase << '%' << std::setw(2) << static_cast<int>(static_cast<std::uint8_t>(c)) << std::nouppercase;
		}

		return oss.str();
	}
}

void CommandDispatcher::queueCommand(const std::string& command) {
	this->m_queuedComments.push(command);
}

std::size_t CommandDispatcher::commandCount() const {
	return this->m_queuedComments.size();
}

void CommandDispatcher::onFinished() {
	m_executing = false;
	this->m_uploadRequest.reset();

	if (m_delegate != nullptr) {
		m_delegate->onDispatchSuccess(this);
	}
}

void CommandDispatcher::onFailure() {
	m_executing = false;

	this->m_queuedComments = {};
	this->m_uploadRequest.reset();

	if (m_delegate != nullptr) {
		m_delegate->onDispatchFailure(this);
	}
}

void CommandDispatcher::nextCommand() {
	if (this->m_queuedComments.empty()) {
		this->onFinished();
		return;
	}

	auto nextCommand = this->m_queuedComments.front();
	this->m_queuedComments.pop();

	auto gm = GameManager::sharedState();
	auto am = GJAccountManager::sharedState();

	auto req = web::WebRequest();
	auto bodyStr = fmt::format("udid={}&accountID={}&userName={}&levelID={}&comment={}&secret=Wmfd2893gb7",
		gm->m_playerUDID, am->m_accountID, gm->m_playerName, this->m_levelId, encodeUrlParam(nextCommand));

	req.bodyString(bodyStr);
	req.userAgent(Config::USER_AGENT);

	auto task = req.post(GDMOD_ENDPOINT_BASE_URL "/uploadGJComment19.php");
	m_uploadListener.setFilter(task);

	m_uploadRequest = req;
}

void CommandDispatcher::beginExecution() {
	if (m_executing) {
		return;
	}

	// we have to use our own request because gd hardcodes the banned flalertlayer
	m_uploadListener.bind([this](web::WebTask::Event* e) {
		if (!this->m_executing) {
			return;
		}

		if (web::WebResponse* res = e->getValue()) {
			auto resultResult = res->string();
			if (!resultResult) {
				this->onFailure();
				return;
			}

			auto resultResultResult = resultResult.unwrap();

			// 1 is successful, but that means it didn't do the command
			if (resultResultResult == "-10") {
				this->nextCommand();
			} else {
				this->onFailure();
			}
		} else if (e->isCancelled()) {
			this->onFailure();
		}
	});

	m_executing = true;
	this->nextCommand();
}

void CommandDispatcher::cancelExecution() {
	if (!m_executing) {
		return;
	}

	m_queuedComments = {};
	m_uploadRequest.reset();
}

void CommandDispatcher::setDelegate(CommandDispatcherProtocol* protocol) {
	this->m_delegate = protocol;
}
