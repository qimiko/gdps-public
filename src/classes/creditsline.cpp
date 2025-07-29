#include "classes/creditsline.hpp"
#include "classes/creditsnode.hpp"

CreditsLine* CreditsLine::create() {
	auto pRet = new CreditsLine();
	if (pRet && pRet->init()) {
		pRet->autorelease();
		return pRet;
	} else {
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

void CreditsLine::addUser(const char* name, int userId, int accountId) {
	m_nameList.emplace_back(name, userId, accountId);
}

cocos2d::CCNode* CreditsLine::createNameLabel(const CreditName& name, std::string nameOverride) {
	auto nameText = nameOverride.empty() ? name.userName : nameOverride;
	auto label = cocos2d::CCLabelBMFont::create(nameText.c_str(), "goldFont.fnt");

	if (name.userId == -1) {
		label->setColor({0x5a, 0xff, 0xff});
		return label;
	}

	auto nameStr = GameLevelManager::sharedState()->userNameForID(name.userId);
	if (nameStr == "-") {
		auto userResponse = fmt::format("{}:{}:{}", name.userId, name.userName, name.accountId);
		GameLevelManager::sharedState()->storeUserNames(userResponse);
	}

	auto btn = CCMenuItemSpriteExtra::create(
		label, nullptr, this,
		static_cast<cocos2d::SEL_MenuHandler>(&CreditsLine::onCreator)
	);

	btn->setTag(name.userId);

	return btn;
}

void CreditsLine::build() {
	m_nameMenu->removeAllChildren();

	if (m_nameList.size() == 1) {
		auto nameLabel = createNameLabel(m_nameList[0], fmt::format("By {}", m_nameList[0].userName));
		m_nameMenu->addChild(nameLabel);
		m_nameMenu->updateLayout();
	}

	if (m_nameList.size() == 2) {
		m_nameMenu->addChild(cocos2d::CCLabelBMFont::create("By ", "goldFont.fnt"));
		m_nameMenu->addChild(createNameLabel(m_nameList[0]));
		m_nameMenu->addChild(cocos2d::CCLabelBMFont::create(" and ", "goldFont.fnt"));
		m_nameMenu->addChild(createNameLabel(m_nameList[1]));

		m_nameMenu->updateLayout();
	}

	if (m_nameList.size() == 3) {
		m_nameMenu->addChild(cocos2d::CCLabelBMFont::create("By ", "goldFont.fnt"));
		m_nameMenu->addChild(createNameLabel(m_nameList[0], fmt::format("{}, ", m_nameList[0].userName)));
		m_nameMenu->addChild(createNameLabel(m_nameList[1]));
		m_nameMenu->addChild(cocos2d::CCLabelBMFont::create(" and ", "goldFont.fnt"));
		m_nameMenu->addChild(createNameLabel(m_nameList[2]));

		m_nameMenu->updateLayout();
	}

	if (m_nameList.size() > 3) {
		m_nameMenu->addChild(cocos2d::CCLabelBMFont::create("By ", "goldFont.fnt"));
		m_nameMenu->addChild(createNameLabel(m_nameList[0], fmt::format("{}, ", m_nameList[0].userName)));
		m_nameMenu->addChild(createNameLabel(m_nameList[1]));
		m_nameMenu->addChild(cocos2d::CCLabelBMFont::create(" and ", "goldFont.fnt"));

		auto moreLabel = cocos2d::CCLabelBMFont::create("more", "goldFont.fnt");
		auto btn = CCMenuItemSpriteExtra::create(
			moreLabel, nullptr, this,
			static_cast<cocos2d::SEL_MenuHandler>(&CreditsLine::onMore)
		);
		m_nameMenu->addChild(btn);

		m_nameMenu->updateLayout();
	}
}

void CreditsLine::FLAlert_Clicked(FLAlertLayer*, bool clicked) {
	if (clicked) {
		auto search = GJSearchObject::create(SearchType::UsersLevels, std::to_string(m_willOpenUser));
		auto scene = LevelBrowserLayer::scene(search);

		auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
		cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
	}
};

void CreditsLine::onCreator(cocos2d::CCObject* target) {
	if (CreditsNode::s_disableOnline) {
		return;
	}

	auto userId = static_cast<cocos2d::CCNode*>(target)->getTag();
	m_willOpenUser = userId;

	auto name = GameLevelManager::sharedState()->userNameForID(userId);

	auto query = fmt::format("View levels created by <cg>{}</c>?", name);

	FLAlertLayer::create(this, "More", query.c_str(), "NO", "YES", 300.0f)->show();
}

void CreditsLine::onMore(cocos2d::CCObject*) {
	CreditsLine::MorePopup::create(m_nameList)->show();
}

bool CreditsLine::init() {
	if (!cocos2d::CCNode::init()) {
		return false;
	}

	m_nameMenu = cocos2d::CCMenu::create();
	m_nameMenu->ignoreAnchorPointForPosition(false);
	m_nameMenu->setLayout(geode::RowLayout::create()
		->setGap(0)
	);
	this->addChild(m_nameMenu);

	auto prio = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->getTargetPrio();
	m_nameMenu->setTouchPriority(prio - 1);

	m_nameMenu->setPosition(0.0f, 0.0f);

	return true;
}

bool CreditsLine::MorePopup::setup(std::vector<CreditName>& names) {
	this->setTitle("Credits", "bigFont.fnt");

	auto creditsNode = CreditsNode::create(nullptr);
	m_mainLayer->addChildAtPosition(creditsNode, geode::Anchor::Center, {0.0f, -10.0f});

	for (const auto& name : names) {
		creditsNode->addUser(name.userName.c_str(), name.userId, name.accountId);
	}

	return true;
}

CreditsLine::MorePopup* CreditsLine::MorePopup::create(std::vector<CreditName>& names) {
	auto pRet = new MorePopup();
	if (pRet && pRet->initAnchored(240.0f, 200.0f, names, "square02.png"_spr)) {
		pRet->autorelease();
		return pRet;
	} else {
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}
