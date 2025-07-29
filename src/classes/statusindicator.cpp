#include "classes/statusindicator.hpp"

bool StatusIndicator::init() {
	if (!cocos2d::CCNode::init()) {
		return false;
	}

	auto background = cocos2d::extension::CCScale9Sprite::create("square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
	this->addChild(background);
	background->setContentSize({ 250.0f, 40.0f });
	background->setPosition(0.0f, 0.0f);
	background->setAnchorPoint({0.5f, 0.5f});
	background->setOpacity(0);
	m_background = background;

	auto label = cocos2d::CCLabelBMFont::create("", "chatFont.fnt");
	this->addChild(label);
	label->setAnchorPoint({0.5f, 0.5f});
	label->setPosition(0.0f, 0.0f);
	label->setOpacity(0);
	m_statusLabel = label;

	return true;
}

void StatusIndicator::setLabel(const char* label) {
	m_statusLabel->setString(label);
	auto contentSize = m_statusLabel->getScaledContentSize();

	constexpr auto padding = 10.0f;

	m_background->setContentSize({
		contentSize.width + padding,
		contentSize.height + padding
	});
}

void StatusIndicator::setFadeToNode(cocos2d::CCNode* node) {
	node->stopActionByTag(1);
	auto fadeAction = cocos2d::CCSequence::createWithTwoActions(
		cocos2d::CCDelayTime::create(0.5f),
		cocos2d::CCFadeTo::create(0.5f, 0)
	);
	fadeAction->setTag(1);

	node->runAction(fadeAction);
}

void StatusIndicator::show() {
	m_statusLabel->setOpacity(255);
	m_background->setOpacity(170);

	this->setFadeToNode(m_statusLabel);
	this->setFadeToNode(m_background);
}
