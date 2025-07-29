#pragma once

#ifndef CLASSES_CREDITSLINE_HPP
#define CLASSES_CREDITSLINE_HPP

#include <Geode/Geode.hpp>
#include <unordered_map>

class CreditsLine : public cocos2d::CCNode, public FLAlertLayerProtocol {
	private:
	struct CreditName {
		std::string userName;
		int userId;
		int accountId;
	};

	class MorePopup : public geode::Popup<std::vector<CreditName>&>  {
		protected:
			bool setup(std::vector<CreditName>&) override;

		public:
			static MorePopup* create(std::vector<CreditName>&);
	};

	cocos2d::CCMenu* m_nameMenu{nullptr};
	std::vector<CreditName> m_nameList;

	virtual void FLAlert_Clicked(FLAlertLayer*, bool) override;

	int m_willOpenUser{-1};

	cocos2d::CCNode* createNameLabel(const CreditName& name, std::string nameOverride = "");

protected:
	virtual bool init() override;

	void onCreator(cocos2d::CCObject*);
	void onMore(cocos2d::CCObject*);

public:
	void addUser(const char* name, int userId, int accountId);

	void build();

	static CreditsLine* create();
};

#endif
