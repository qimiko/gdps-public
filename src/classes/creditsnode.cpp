#include "classes/creditsnode.hpp"
#include <fmt/format.h>

bool CreditsNode::init(const char* title) {
	auto noTitle = title == nullptr;
	if (!noTitle) {
		auto tlabel = cocos2d::CCLabelBMFont::create(title, "bigFont.fnt");
		this->addChild(tlabel);
		tlabel->setScale(0.6f);
	}

	m_noTitle = noTitle;

	m_names = cocos2d::CCMenu::create();
	this->addChild(m_names);

	if (noTitle) {
		m_names->setContentSize({ 200.0f, 100.0f });
	} else {
		m_names->setContentSize({ 108.0f, 150.0f });
	}
	m_names->ignoreAnchorPointForPosition(false);
	m_names->setAnchorPoint(noTitle
		? cocos2d::CCPoint { 0.5f, 0.5f }
		: cocos2d::CCPoint { 0.0f, 1.0f }
	);

	if (noTitle) {
		m_names->setLayout(
			geode::RowLayout::create()
				->setAutoScale(false)
				->setAxisAlignment(geode::AxisAlignment::Center)
				->setCrossAxisAlignment(geode::AxisAlignment::Start)
				->setGrowCrossAxis(true)
				->setGap(10.0f)
		);
	} else {
		m_names->setLayout(
			geode::ColumnLayout::create()
				->setAutoScale(false)
				->setAxisAlignment(geode::AxisAlignment::End)
				->setCrossAxisAlignment(geode::AxisAlignment::Start)
				->setGrowCrossAxis(true)
				->setCrossAxisReverse(true)
				->setAxisReverse(true)
		);
	}

	if (noTitle) {
		m_names->setPosition(0.0f, 0.0f);
	} else {
		m_names->setPosition(-53.0f, -15.0f);
	}

	auto prio = cocos2d::CCDirector::sharedDirector()->getTouchDispatcher()->getTargetPrio();
	m_names->setTouchPriority(prio - 1);

	return true;
}

void CreditsNode::FLAlert_Clicked(FLAlertLayer*, bool clicked) {
	if (clicked) {
		auto search = GJSearchObject::create(SearchType::UsersLevels, std::to_string(m_willOpenUser));
		auto scene = LevelBrowserLayer::scene(search);

		auto transition = cocos2d::CCTransitionFade::create(0.5f, scene);
		cocos2d::CCDirector::sharedDirector()->replaceScene(transition);
	}
};

void CreditsNode::addUser(const char* name, int userId, int accountId) {
	auto label = cocos2d::CCLabelBMFont::create(name, "goldFont.fnt");
	label->setScale(0.75f);

	if (userId == -1) {
		m_names->addChild(label);
		label->setColor({0x5a, 0xff, 0xff});

		m_names->updateLayout();

		if (!m_noTitle) {
			auto menuWidth = m_names->getScaledContentWidth();
			if (menuWidth < 110.0f) {
				m_names->setPositionX(-menuWidth / 2);
			}
		}

		return;
	}

	auto nameStr = GameLevelManager::sharedState()->userNameForID(userId);
	if (nameStr == "-") {
		auto userResponse = fmt::format("{}:{}:{}", userId, name, accountId);
		GameLevelManager::sharedState()->storeUserNames(userResponse);
	}

	auto btn = CCMenuItemSpriteExtra::create(
		label, nullptr, this,
		static_cast<cocos2d::SEL_MenuHandler>(&CreditsNode::onCreator)
	);

	m_names->addChild(btn);

	btn->setTag(userId);

	m_names->updateLayout();

	// we want to center based on the first column only 
	// i couldn't think of a better way to do this
	if (!m_noTitle) {
		auto menuWidth = m_names->getScaledContentWidth();
		if (menuWidth < 110.0f) {
			m_names->setPositionX(-menuWidth / 2);
		}
	}
}

void CreditsNode::onCreator(cocos2d::CCObject* target) {
	if (s_disableOnline) {
		return;
	}

	auto userId = static_cast<cocos2d::CCNode*>(target)->getTag();
	m_willOpenUser = userId;

	auto name = GameLevelManager::sharedState()->userNameForID(userId);

	auto query = fmt::format("View levels created by <cg>{}</c>?", name);

	FLAlertLayer::create(this, "More", query.c_str(), "NO", "YES", 300.0f)->show();
}

bool CreditsNode::s_disableOnline = false;
