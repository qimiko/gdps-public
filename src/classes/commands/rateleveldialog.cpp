#include "base/game_variables.hpp"

#include "classes/commands/rateleveldialog.hpp"

void RateLevelDialog::keyBackClicked() {
	this->onClose(nullptr);
};

void RateLevelDialog::onClose(cocos2d::CCObject*) {
	this->m_reasonInput->onClickTrackNode(false);
	this->setKeyboardEnabled(false);
	this->removeFromParentAndCleanup(true);
}

bool RateLevelDialog::init(GJGameLevel* level, bool isSend) {
	if (!this->initWithColor({0, 0, 0, 75})) {
		return false;
	}

	this->m_level = level;
	this->m_isSend = isSend;
	this->m_selectedStars = level->m_stars;

	if (level->m_featured == 184594917) {
		this->m_selectedRating = 2;
	} else if (level->m_featured > 0) {
		this->m_selectedRating = 1;
	} else if (!isSend && m_level->m_stars == 0 && m_level->m_ratingsSum != 0) {
		this->m_selectedRating = -1;

		if (m_level->m_autoLevel) {
			this->m_selectedStars = 1;
		} else if (m_level->m_demon) {
			this->m_selectedStars = 10;
		} else {
			auto currentRating = m_level->m_ratingsSum / m_level->m_ratings;
			switch (currentRating) {
				case 1:
					this->m_selectedStars = 2;
					break;
				case 2:
					this->m_selectedStars = 3;
					break;
				case 3:
					this->m_selectedStars = 4;
					break;
				case 4:
					this->m_selectedStars = 6;
					break;
				case 5:
					this->m_selectedStars = 8;
					break;
			}
		}
	} else {
		this->m_selectedRating = 0;
	}

	this->m_initialStars = m_selectedStars;
	this->m_initialRating = m_selectedRating;

	constexpr auto width = 400.0f;
	constexpr auto height = 280.0f;

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
	this->m_mainLayer->addChild(this->m_buttonMenu, 1);
	m_buttonMenu->ignoreAnchorPointForPosition(false);

	auto close_sprite = ButtonSprite::create("Cancel");
	auto close_btn = CCMenuItemSpriteExtra::create(
		close_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&RateLevelDialog::onClose)
	);

	this->m_buttonMenu->addChild(close_btn);
	close_btn->setPosition(
		window_size.width / 2 - 60.0f,
		(window_size.height - height) / 2 + 30.0f
	);

	auto info_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
	auto info_btn = CCMenuItemSpriteExtra::create(
		info_sprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&RateLevelDialog::onInfo)
	);

	this->m_buttonMenu->addChild(info_btn);
	info_btn->setPosition(
		(window_size.width + width) / 2 - 20.0f,
		(window_size.height + height) / 2 - 20.0f
	);

	if (!isSend) {
		m_sendIndicator = cocos2d::CCSprite::createWithSpriteFrameName("GJ_downloadsIcon_001.png");
		auto toggle_btn = CCMenuItemSpriteExtra::create(
			m_sendIndicator, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&RateLevelDialog::toggleSend)
		);

		this->m_buttonMenu->addChild(toggle_btn);
		toggle_btn->setPosition(
			(window_size.width + width) / 2 - 50.0f,
			(window_size.height + height) / 2 - 20.0f
		);

		m_sendIndicator->setColor({ 0xff, 0xff, 0xff });
	}

	auto submitSprite = ButtonSprite::create("Submit");
	auto submitBtn = CCMenuItemSpriteExtra::create(
		submitSprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&RateLevelDialog::onSubmit)
	);
	m_buttonMenu->addChild(submitBtn);
	submitBtn->setPosition(
		window_size.width / 2 + 60.0f,
		(window_size.height - height) / 2 + 30.0f
	);
	m_submitSprite = submitSprite;

	if (isSend) {
		m_title = cocos2d::CCLabelBMFont::create("Send Level", "bigFont.fnt");
	} else {
		m_title = cocos2d::CCLabelBMFont::create("Mod: Update Rating", "bigFont.fnt");
	}

	m_mainLayer->addChild(m_title);
	m_title->setPosition({window_size.width / 2, (window_size.height + height) / 2 - 22.0f});
	m_title->setScale(0.75f);

	auto leftPos = (window_size.width - width) / 2;
	auto rightPos = (window_size.width + width) / 2;
	auto bottomPos = (window_size.height - height) / 2;

	for (int i = 0; i <= 10; i++) {
		auto star = std::to_string(i);
		auto starSprite = ButtonSprite::create(star.c_str(), 20, 0, 0.5f, true, "bigFont.fnt", "GJ_button_01.png", 30.0f);
		auto starBtn = CCMenuItemSpriteExtra::create(starSprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&RateLevelDialog::onStar));

		starBtn->setTag(i);
		starSprite->setTag(i);

		m_starButtons.push_back(starSprite);
		m_buttonMenu->addChild(starBtn);

		auto yOffs = i > 5 ? -20.0f : 20.0f;
		auto xOffs = i > 5 ? 75.0f : 55.0f;

		starBtn->setPosition({leftPos + xOffs + (42.0f * (i % 6)), (window_size.height / 2) + yOffs + 50.0f});
	}

	auto difficultySprite = cocos2d::CCSprite::createWithSpriteFrameName("difficulty_auto_btn_001.png");
	auto featuredSprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png");

	m_difficultySprite = difficultySprite;
	m_ratingSprite = featuredSprite;

	auto difficultySize = difficultySprite->getContentSize();
	difficultySprite->addChild(featuredSprite, -1);
	featuredSprite->setPosition({difficultySize.width * 0.5f, difficultySize.height * 0.5f});
	difficultySprite->setScale(1.2f);

	auto difficultyBtn = CCMenuItemSpriteExtra::create(difficultySprite, nullptr, this, static_cast<cocos2d::SEL_MenuHandler>(&RateLevelDialog::onToggleRating));
	m_buttonMenu->addChild(difficultyBtn);

	auto noStarIndicator = cocos2d::CCLabelBMFont::create("(No-Star)", "bigFont.fnt");
	m_mainLayer->addChild(noStarIndicator);
	noStarIndicator->setPosition({rightPos - 68.0f, window_size.height / 2 + 10.0f});
	noStarIndicator->setVisible(false);
	noStarIndicator->setScale(0.5f);
	m_noStarIndicator = noStarIndicator;

	difficultyBtn->setPosition({rightPos - 70.0f, window_size.height / 2 + 50.0f});

	auto reasonBg = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0.0f, 0.0f, 80.0f, 80.0f});
	reasonBg->setOpacity(100);
	reasonBg->setContentSize({360.0f, 60.0f});
	reasonBg->setColor({0, 0, 0});

	m_mainLayer->addChild(reasonBg);
	reasonBg->setPosition({window_size.width / 2, bottomPos + 100.0f});

	auto reasonArea = TextArea::create("Enter reason", 300.0f, 0, {0.5f, 0.5f}, "chatFont.fnt", 20.0f);
	m_reasonArea = reasonArea;

	m_mainLayer->addChild(reasonArea, 2);

	reasonArea->setPosition({window_size.width / 2, bottomPos + 100.0f});

	auto reasonInput = CCTextInputNode::create(360.0f, 50.0f, "Enter reason", "Thonburi", 24, nullptr);
	m_reasonInput = reasonInput;

	reasonInput->m_filterSwearWords = true;
	reasonInput->m_delegate = this;
	reasonInput->m_maxLabelLength = 256;
	reasonInput->setAllowedChars(" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,-!?)(/*@\"");

	m_mainLayer->addChild(reasonInput);
	reasonInput->m_textField->setVisible(false);

	reasonInput->setPosition({window_size.width / 2, bottomPos + 100.0f});

	this->updateDifficulty();
	this->updateReason(m_reason);

	return true;
}

void RateLevelDialog::toggleSend(cocos2d::CCObject*) {
	this->m_isSend = !this->m_isSend;
	this->updateDifficulty();

	if (m_isSend) {
		m_title->setString("Send Level");
		m_sendIndicator->setColor({0xaa, 0xaa, 0xaa});
	} else {
		m_title->setString("Mod: Update Rating");
		m_sendIndicator->setColor({0xff, 0xff, 0xff});
	}
}

void RateLevelDialog::onInfo(cocos2d::CCObject*) {
	FLAlertLayer::create(nullptr,
		"Send Guidelines",
		"All sends are <co>publicly visible</c> in the 1.9 GDPS <cl>Discord server</c>.\n"
		"Please follow the ingame <cy>commenting rules</c> when sending a level.\n"
		"Abuse of the <cg>send system</c> may result in a <cr>permanent send ban</c>.",
		"OK", nullptr, 350.0f
	)->show();
}

void RateLevelDialog::updateDifficulty() {
	auto difficultyName = "difficulty_00_btn_001.png";
	switch (m_selectedStars) {
		case 1:
			difficultyName = "difficulty_auto_btn_001.png";
			break;
		case 2:
			difficultyName = "difficulty_01_btn_001.png";
			break;
		case 3:
			difficultyName = "difficulty_02_btn_001.png";
			break;
		case 4:
		case 5:
			difficultyName = "difficulty_03_btn_001.png";
			break;
		case 6:
		case 7:
			difficultyName = "difficulty_04_btn_001.png";
			break;
		case 8:
		case 9:
			difficultyName = "difficulty_05_btn_001.png";
			break;
		case 10:
			difficultyName = "difficulty_06_btn_001.png";
			break;
	}

	auto frameCache = cocos2d::CCSpriteFrameCache::sharedSpriteFrameCache();
	auto difficultyFrame = frameCache->spriteFrameByName(difficultyName);

	m_difficultySprite->setDisplayFrame(difficultyFrame);

	auto difficultySize = m_difficultySprite->getContentSize();
	m_ratingSprite->setPosition({difficultySize.width * 0.5f, difficultySize.height * 0.5f});

	if (m_selectedRating > 0) {
		auto ratingName = "GJ_featuredCoin_001.png";
		if (m_selectedRating >= 2 && !m_isSend) {
			ratingName = "superCoin.png"_spr;
		}

		auto ratingFrame = frameCache->spriteFrameByName(ratingName);

		m_ratingSprite->setDisplayFrame(ratingFrame);
		m_ratingSprite->setVisible(true);
	} else {
		m_ratingSprite->setVisible(false);
	}

	m_noStarIndicator->setVisible(m_selectedRating == -1);

	for (const auto& btn : m_starButtons) {
		if (btn->getTag() != m_selectedStars) {
			btn->updateBGImage("GJ_button_04.png");
		} else {
			btn->updateBGImage("GJ_button_01.png");
		}
	}

	if (this->hasMadeChanges()) {
		m_submitSprite->setColor({0xff, 0xff, 0xff});
	} else {
		m_submitSprite->setColor({0xa6, 0xa6, 0xa6});
	}
}

void RateLevelDialog::onStar(cocos2d::CCObject* target) {
	auto tag = static_cast<cocos2d::CCNode*>(target)->getTag();
	m_selectedStars = tag;

	if (tag == 0 && m_selectedRating == -1) {
		m_selectedRating = 0;
	}

	this->updateDifficulty();
}

void RateLevelDialog::onToggleRating(cocos2d::CCObject*) {
	if (!m_isSend && m_selectedRating >= 2) {
		if (m_selectedStars == 0) {
			m_selectedRating = 0;
		} else {
			m_selectedRating = -1;
		}
	} else if (m_isSend && m_selectedRating >= 1) {
		m_selectedRating = 0;
	} else {
		m_selectedRating++;
	}

	this->updateDifficulty();
}

void RateLevelDialog::updateReason(const std::string& reason) {
	if (reason.empty()) {
		m_reasonArea->setString("Enter reason");
		m_reasonArea->colorAllCharactersTo({0xc8, 0xc8, 0xc8});
	} else {
		m_reasonArea->setString(reason.c_str());
		m_reasonArea->colorAllCharactersTo({0xff, 0xff, 0xff});
	}
}

void RateLevelDialog::textChanged(CCTextInputNode* input) {
	this->updateReason(input->getString());
}

void RateLevelDialog::textInputClosed(CCTextInputNode* input) {
	m_reason = input->getString();
	this->updateReason(m_reason);
}

void RateLevelDialog::textInputOpened(CCTextInputNode* input) {
	m_reasonArea->setString(input->getString().c_str());
}

void RateLevelDialog::onDispatchSuccess(CommandDispatcher*) {
	m_uploadAction->showSuccessMessage("Success! Rating submitted.");
	m_commandDispatcher.reset();
}

void RateLevelDialog::onDispatchFailure(CommandDispatcher*) {
	m_uploadAction->showFailMessage("Execution failed.");
	m_commandDispatcher.reset();
}

bool RateLevelDialog::hasMadeChanges() const {
	return m_selectedStars != m_initialStars || m_selectedRating != m_initialRating;
}

const char* RateLevelDialog::currentDifficultyName() const {
	switch (m_selectedStars) {
		default:
		case 0:
			return "na";
		case 1:
			return "auto";
		case 2:
			return "easy";
		case 3:
			return "normal";
		case 4:
		case 5:
			return "hard";
		case 6:
		case 7:
			return "harder";
		case 8:
		case 9:
			return "insane";
		case 10:
			return "demon";
	}
}

void RateLevelDialog::onSubmit(cocos2d::CCObject*) {
	if (m_commandDispatcher) {
		return;
	}

	// determine if the user actually changed any parameters
	if (!this->hasMadeChanges()) {
		return;
	}

	m_reason = m_reasonInput->getString();

	m_uploadAction = UploadActionPopup::create("Submitting rating...");
	m_uploadAction->setDelegate(this);
	m_uploadAction->show();

	auto currentRating = 0;
	if (m_level->m_featured == 184594917) {
		currentRating = 2;
	} else if (m_level->m_featured > 0) {
		currentRating = 1;
	}

	if (m_isSend) {
		auto rating = std::min(m_selectedRating, 1);

		auto& dispatcher = m_commandDispatcher.emplace(m_level->m_levelID);
		dispatcher.setDelegate(this);

		auto command = fmt::format("!send {} {} {} {}", currentDifficultyName(), m_selectedStars, rating, m_reason);
		dispatcher.queueCommand(command);

		dispatcher.beginExecution();

		return;
	}

	// determine what we actually changed
	auto& dispatcher = m_commandDispatcher.emplace(m_level->m_levelID);
	dispatcher.setDelegate(this);

	auto isUnfeaturing = currentRating >= 1 && m_selectedRating <= 0;

	if (m_selectedStars != m_initialStars || isUnfeaturing) {
		auto rating = std::clamp(m_selectedRating, 0, 1);

		auto trueStars = m_selectedRating < 0 ? 0 : m_selectedStars;
		auto command = fmt::format("!rate {} {} {} {}", currentDifficultyName(), trueStars, rating, m_reason);
		dispatcher.queueCommand(command);

		// this command can set feature status, but not revoke epic
		if (currentRating <= 1) {
			currentRating = rating;
		}
	}

	if (currentRating != m_selectedRating) {
		if (currentRating < 1 && m_selectedRating >= 1) {
			// we want to feature
			dispatcher.queueCommand("!feature");
		}

		if (currentRating < 2 && m_selectedRating == 2) {
			dispatcher.queueCommand("!epic");
		}

		// only possible unrank is an unsuper
		if (currentRating == 2 && m_selectedRating < 2) {
			dispatcher.queueCommand("!unepic");
		}
	}

	dispatcher.beginExecution();
}

void RateLevelDialog::onClosePopup(UploadActionPopup* popup) {
	if (m_commandDispatcher) {
		m_commandDispatcher->cancelExecution();
		m_commandDispatcher.reset();
	}

	this->m_uploadAction = nullptr;

	if (popup->getSuccessful()) {
		this->onClose(nullptr);
	}

	popup->closePopup();
}
