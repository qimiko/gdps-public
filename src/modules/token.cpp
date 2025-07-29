#include <Geode/Geode.hpp>
#include <Geode/modify/GJAccountManager.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/GameManager.hpp>

#include <string>

#include "base/game_variables.hpp"

#include "classes/managers/tokenmanager.hpp"
#include "classes/sessionwidget.hpp"

struct TokenGJAccountManager : geode::Modify<TokenGJAccountManager, GJAccountManager> {
	void dataLoaded(DS_Dictionary* dictionary) {
		GJAccountManager::dataLoaded(dictionary);

		TokenManager::get().load(dictionary);
	}

	void saveCustomData(DS_Dictionary* dictionary) {
		TokenManager::get().save(dictionary);
	}

#ifndef GEODE_IS_WINDOWS
	void encodeDataTo(DS_Dictionary* dictionary) {
		GJAccountManager::encodeDataTo(dictionary);

		this->saveCustomData(dictionary);
	}
#endif

	void unlinkFromAccount() {
		TokenManager::get().deleteAuthKey();

		GJAccountManager::unlinkFromAccount();
	}
};

#ifdef GEODE_IS_WINDOWS
// GJAccountManager::encodeDataTo is inlined on windows
struct TokenGameManager : geode::Modify<TokenGameManager, GameManager> {
	void encodeDataTo(DS_Dictionary* dictionary) {
		GameManager::encodeDataTo(dictionary);

		static_cast<TokenGJAccountManager*>(GJAccountManager::sharedState())
			->saveCustomData(dictionary);
	}
};
#endif

struct TokenCreatorLayer : geode::Modify<TokenCreatorLayer, CreatorLayer> {
	bool init() {
		auto gm = GameManager::sharedState();
		auto create_session = !gm->getGameVariable(GameVariable::DISABLE_SESSION);
		auto am = GJAccountManager::sharedState();

		if (PlatformToolbox::isNetworkAvailable() && create_session && am->m_accountID) {
			auto& tokenManager = TokenManager::get();
			if (!tokenManager.createdSession()) {
				TokenManager::get().createSession();
			}
		}

		if (CreatorLayer::init()) {
			if (am->m_accountID != 0) {
				// only create session widget for registered accounts
				auto status = SessionWidget::create();
				this->addChild(status);

				auto win_size = cocos2d::CCDirector::sharedDirector()->getWinSize();
				status->setPosition({ win_size.width / 2.0f, 22.0f });
			}

			return true;
		}

		return false;
	}
};
