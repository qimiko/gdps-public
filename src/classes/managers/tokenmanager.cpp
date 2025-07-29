#include "classes/managers/tokenmanager.hpp"

#include "base/config.hpp"

TokenManager::TokenManager() : cocos2d::CCObject() {}

TokenManager& TokenManager::get() {
	static TokenManager _managerInstance;
	return _managerInstance;
}

void TokenManager::load(DS_Dictionary* dictionary) {
	auto string_obj = dictionary->getStringForKey("GJAExt_003");
	if (!string_obj.empty()) {
		this->m_token = string_obj;
	}

	auto time_string_obj = dictionary->getStringForKey("GJAExt_004");
	if (!time_string_obj.empty()) {
		auto lst = std::strtoul(time_string_obj.c_str(), nullptr, 0);
		this->m_lastSessionTime = lst;
	}
}

void TokenManager::save(DS_Dictionary* dictionary) {
	if (this->m_token.empty()) {
		return;
	}

	dictionary->setStringForKey("GJAExt_003", this->m_token);

	auto session_time_obj = std::to_string(this->m_lastSessionTime);

	dictionary->setStringForKey("GJAExt_004", session_time_obj);
}

void TokenManager::deleteAuthKey() {
	if (this->m_isSendingReq) {
		return;
	}

	if (this->m_token.empty()) {
		if (auto delegate = this->m_sessionDelegate) {
			delegate->onSessionFailed("No token found.");
		}

		return;
	}

	this->m_listener.bind([this](geode::utils::web::WebTask::Event* e) {
		if (e->getProgress()) {
			return;
		}

		this->m_isSendingReq = false;

		if (auto res = e->getValue()) {
			auto jsonRes = res->json();
			if (!jsonRes) {
				geode::log::info("Invalid authkey response");
				if (auto delegate = this->m_sessionDelegate) {
					delegate->onSessionFailed("Invalid server response for deletion");
				}

				return;
			}

			auto json = jsonRes.unwrap();
			auto success = json.get<bool>("success").unwrapOrDefault();

			if (!success) {
				auto reason = json.get<std::string>("error").unwrapOr("Unknown error");
				geode::log::info("authkey deletion failed: {}", reason);

				if (auto delegate = this->m_sessionDelegate) {
					delegate->onSessionFailed(reason.c_str());
				}

				return;
			}

			this->m_token.clear();
			this->m_createdSession = false;
			this->m_lastSessionTime = 0;

			// silly response, but it's only used once so whatever
			if (auto delegate = this->m_sessionDelegate) {
				delegate->onSessionCreated();
			}
		}
	});

	auto req = geode::utils::web::WebRequest();

	auto reqData = fmt::format("authkey={}&mode=single", this->m_token);

	req.bodyString(reqData);
	req.userAgent(Config::USER_AGENT);

	this->m_isSendingReq = true;

	m_listener.setFilter(req.post(GDMOD_ENDPOINT_BASE_URL "/api/logout"));
}

void TokenManager::createAuthKey() {
	if (this->m_isSendingReq) {
		return;
	}

	this->m_listener.bind([this](geode::utils::web::WebTask::Event* e) {
		if (e->getProgress()) {
			return;
		}

		this->m_isSendingReq = false;

		if (auto res = e->getValue()) {
			auto jsonRes = res->json();
			if (!jsonRes) {
				geode::log::info("Invalid authkey response");
				if (auto delegate = this->m_sessionDelegate) {
					delegate->onSessionFailed("Invalid server response for authkey");
				}

				return;
			}

			auto json = jsonRes.unwrap();
			auto success = json.get<bool>("success").unwrapOrDefault();
			auto token = json.get<std::string>("authkey");

			if (!success || !token) {
				auto reason = json.get<std::string>("error").unwrapOr("Unknown error");
				geode::log::info("authkey creation failed: {}", reason);

				if (auto delegate = this->m_sessionDelegate) {
					delegate->onSessionFailed(reason.c_str());
				}

				return;
			}

			this->m_token = token.unwrap();

			this->createSession();
		}
	});

	geode::log::debug("sending authkey creation req");

	auto req = geode::utils::web::WebRequest();

	auto am = GJAccountManager::sharedState();
	auto reqData = fmt::format("username={}&password={}", am->m_username, am->m_password);

	req.bodyString(reqData);
	req.userAgent(Config::USER_AGENT);

	this->m_isSendingReq = true;

	m_listener.setFilter(req.post(GDMOD_ENDPOINT_BASE_URL "/api/login"));
}

void TokenManager::createSession() {
	if (this->m_isSendingReq) {
		return;
	}

	if (this->m_token.empty()) {
		return this->createAuthKey();
	}

	this->m_listener.bind([this](geode::utils::web::WebTask::Event* e) {
		if (e->getProgress()) {
			return;
		}

		this->m_isSendingReq = false;

		if (auto res = e->getValue()) {
			auto jsonRes = res->json();
			if (!jsonRes) {
				geode::log::info("session failed: invalid response");
				if (auto delegate = this->m_sessionDelegate) {
					delegate->onSessionFailed("Invalid server response");
				}

				return;
			}

			auto json = jsonRes.unwrap();
			auto success = json.get<bool>("success").unwrapOrDefault();
			if (!success) {
				auto reason = json.get<std::string>("error").unwrapOr("Unknown error");
				geode::log::info("session failed: {}", reason);

				if (auto delegate = this->m_sessionDelegate) {
					delegate->onSessionFailed(reason.c_str());
				}

				return;
			}

			this->m_createdSession = true;
			this->m_lastSessionTime = std::time(nullptr);

			if (auto delegate = this->m_sessionDelegate) {
				delegate->onSessionCreated();
			}
		}
	});

	geode::log::debug("sending session creation req");

	auto req = geode::utils::web::WebRequest();

	auto reqData = fmt::format("authkey={}", this->m_token);

	req.bodyString(reqData);
	req.userAgent(Config::USER_AGENT);

	this->m_isSendingReq = true;

	m_listener.setFilter(req.post(GDMOD_ENDPOINT_BASE_URL "/api/session"));
}

void TokenManager::setSessionDelegate(SessionDelegate* delegate) {
	this->m_sessionDelegate = delegate;
}

bool TokenManager::sendingRequest() const {
	return this->m_isSendingReq;
}

bool TokenManager::createdSession() const {
	return this->m_createdSession;
}
