#pragma once

#ifndef CLASSES_MANAGERS_TOKENMANAGER_HPP
#define CLASSES_MANAGERS_TOKENMANAGER_HPP

#include <ctime>

#include <Geode/Geode.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/web.hpp>

#include "classes/delegates/sessiondelegate.hpp"

class TokenManager : public cocos2d::CCObject {
  std::string m_token{};
  bool m_createdSession{false};
  std::time_t m_lastSessionTime{0};

  bool m_isSendingReq{false};
	SessionDelegate* m_sessionDelegate{nullptr};

	geode::EventListener<geode::utils::web::WebTask> m_listener{};

	void createAuthKey();

	TokenManager();

public:
	void init();

	static TokenManager& get();

	void load(DS_Dictionary* dictionary);
	void save(DS_Dictionary* dictionary);

	void setSessionDelegate(SessionDelegate*);

	void createSession();
	void deleteAuthKey();

	bool sendingRequest() const;
	bool createdSession() const;

	TokenManager(const TokenManager&) = delete;
	TokenManager(TokenManager&&) = delete;
};

#endif
