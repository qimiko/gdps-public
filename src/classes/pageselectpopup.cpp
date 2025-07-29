#include "classes/pageselectpopup.hpp"

void PageSelectPopup::keyBackClicked() {
	this->onClose(nullptr);
};

void PageSelectPopup::onClose(cocos2d::CCObject*) {
	auto selectedPage = this->m_selectedPage;
	auto delegate = this->m_delegate;

	this->m_pageInput->onClickTrackNode(false);

	this->setKeyboardEnabled(false);
	this->removeFromParentAndCleanup(true);

	if (delegate != nullptr) {
		delegate->onSelectPage(selectedPage);
	}
}

bool PageSelectPopup::init(PageSelectDelegate* delegate, int page, int maxPages) {
	if (!this->initWithColor({0, 0, 0, 75})) {
		return false;
	}

	constexpr auto width = 250.0f;
	constexpr auto height = 150.0f;

	m_delegate = delegate;
	m_selectedPage = page;
	m_initialPage = page;
	m_maxPage = maxPages;

	auto ccd = cocos2d::CCDirector::sharedDirector();
	ccd->getTouchDispatcher()->incrementForcePrio();
	this->setTouchEnabled(true);
	this->setKeypadEnabled(true);

	this->m_controlConnected = -1;
	this->m_scrollAction = -1;

	this->m_mainLayer = cocos2d::CCLayer::create();
	this->addChild(this->m_mainLayer);

	auto window_size = ccd->getWinSize();
	auto bg = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", {0.0f, 0.0f, 80.0f, 80.0f});
	bg->setContentSize(cocos2d::CCSize(width, height));
	bg->setPosition(window_size.width / 2, window_size.height / 2);

	this->m_mainLayer->addChild(bg, -2);

	this->m_buttonMenu = cocos2d::CCMenu::create();
	this->m_mainLayer->addChild(this->m_buttonMenu);
	m_buttonMenu->ignoreAnchorPointForPosition(false);

	auto close_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png");
	auto close_btn = CCMenuItemSpriteExtra::create(
		close_sprite,
		nullptr,
		this,
		static_cast<cocos2d::SEL_MenuHandler>(&PageSelectPopup::onClose)
	);

	this->m_buttonMenu->addChild(close_btn);
	close_btn->setPosition(
		(window_size.width - width) / 2 + 5,
		(window_size.height + height) / 2 - 5
	);

	auto title = cocos2d::CCLabelBMFont::create("Select Page", "goldFont.fnt");
	this->m_mainLayer->addChild(title);

	title->setPosition({
		window_size.width / 2,
		(window_size.height + height) / 2 - 20.0f
	});

	auto pageInput = CCTextInputNode::create(80.0f, 60.0f, "Page", "Thonburi", 24, "bigFont.fnt");
	pageInput->m_delegate = this;
	pageInput->m_maxLabelLength = 6;
	pageInput->setAllowedChars("0123456789");
	pageInput->setMaxLabelWidth(80.0f);
	pageInput->setMaxLabelScale(0.75f);
	pageInput->setLabelPlaceholderScale(0.75f);
	pageInput->setLabelPlaceholderColor({120, 170, 240});
	pageInput->setPosition(window_size.width / 2, window_size.height / 2);

	m_mainLayer->addChild(pageInput, 1);
	this->m_pageInput = pageInput;

	auto label_bg = cocos2d::extension::CCScale9Sprite::create("square02_small.png", cocos2d::CCRect(0.0f, 0.0f, 40.0f, 40.0f));
	label_bg->setContentSize({ 80.0f, 40.0f });
	label_bg->setOpacity(100);
	label_bg->setPosition(window_size.width / 2, window_size.height / 2);
	m_mainLayer->addChild(label_bg);

	auto next_sprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
	next_sprite->setScale(1.5f);
	this->m_nextBtn = CCMenuItemSpriteExtra::create(next_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&PageSelectPopup::onNextPage));

	m_buttonMenu->addChild(m_nextBtn);
	m_nextBtn->setPosition(
		window_size.width / 2 + 60.0,
		window_size.height / 2
	);

	auto prev_sprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
	prev_sprite->setScale(1.5f);
	this->m_prevBtn = CCMenuItemSpriteExtra::create(prev_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&PageSelectPopup::onPrevPage));

	m_buttonMenu->addChild(m_prevBtn);
	m_prevBtn->setPosition(
		(window_size.width) / 2 - 60.0,
		window_size.height / 2
	);

	auto first_sprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_leftBtn2_001.png");
	this->m_firstBtn = CCMenuItemSpriteExtra::create(first_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&PageSelectPopup::onFirstPage));

	m_buttonMenu->addChild(m_firstBtn);
	m_firstBtn->setPosition(
		(window_size.width) / 2 - 90.0,
		(window_size.height - height) / 2 + 25.0f
	);

	auto last_sprite = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn2_001.png");
	this->m_lastBtn = CCMenuItemSpriteExtra::create(last_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&PageSelectPopup::onLastPage));

	m_buttonMenu->addChild(m_lastBtn);
	m_lastBtn->setPosition(
		window_size.width / 2 + 90.0,
		(window_size.height - height) / 2 + 25.0f
	);

	this->m_pageInfo = cocos2d::CCLabelBMFont::create("Page x of x", "bigFont.fnt");
	m_pageInfo->setScale(0.6f);

	this->m_mainLayer->addChild(m_pageInfo);

	m_pageInfo->setPosition(
		(window_size.width) / 2,
		(window_size.height - height) / 2 + 25.0f
	);

	this->updateLabels();

	return true;
}

void PageSelectPopup::updateLabels(bool skipInput) {
if (!skipInput) {
		auto pageLabel = fmt::format("{}", m_selectedPage + 1);

		m_updatingLabels = true;
		m_pageInput->setString(pageLabel.c_str());
		m_updatingLabels = false;
	}

	auto isLast = m_selectedPage < m_maxPage;
	m_nextBtn->setVisible(isLast);

	auto isFirst = m_selectedPage > 0;
	m_prevBtn->setVisible(isFirst);

	auto pageInfo = fmt::format("Page {} of {}", m_initialPage + 1, m_maxPage + 1);
	m_pageInfo->setString(pageInfo.c_str());
	m_pageInfo->limitLabelWidth(150.0f, 0.6f, 0.3f);
}

void PageSelectPopup::onNextPage(cocos2d::CCObject*) {
	if (m_selectedPage == m_maxPage) {
		return;
	}

	m_selectedPage++;
	this->updateLabels();
}

void PageSelectPopup::onPrevPage(cocos2d::CCObject*) {
	if (m_selectedPage == 0) {
		return;
	}

	m_selectedPage--;
	this->updateLabels();
}

void PageSelectPopup::onFirstPage(cocos2d::CCObject*) {
	m_selectedPage = 0;
	this->onClose(nullptr);
}

void PageSelectPopup::onLastPage(cocos2d::CCObject*) {
	m_selectedPage = m_maxPage;
	this->onClose(nullptr);
}

void PageSelectPopup::textInputClosed(CCTextInputNode* input) {
	if (m_updatingLabels) {
		return;
	}

	auto labelValue = input->getString();
	auto parsed = std::atoi(labelValue.c_str());

	if (parsed <= 0) {
		m_selectedPage = 0;
	} else if (parsed >= m_maxPage) {
		m_selectedPage = m_maxPage;
	} else {
		m_selectedPage = parsed - 1;
	}

	this->updateLabels();
}

void PageSelectPopup::textChanged(CCTextInputNode* input) {
	if (m_updatingLabels) {
		return;
	}

	auto labelValue = input->getString();
	auto parsed = std::atoi(labelValue.c_str());

	if (parsed <= 0) {
		m_selectedPage = 0;
	} else if (parsed >= m_maxPage) {
		m_selectedPage = m_maxPage;
	} else {
		m_selectedPage = parsed - 1;
	}

	this->updateLabels(true);
}
