#include <Geode/Geode.hpp>
#include <Geode/modify/MoreSearchLayer.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

#include <vector>

#include "classes/extensions/gjsearchobjectext.hpp"
#include "classes/pageselectpopup.hpp"

struct SearchMoreSearchLayer : geode::Modify<SearchMoreSearchLayer, MoreSearchLayer> {
	void onNoReupload(cocos2d::CCObject* /* target */) {
		auto glm = GameLevelManager::sharedState();

		auto no_reupload = glm->getBoolForKey("noreupload_filter");
		glm->setBoolForKey(!no_reupload, "noreupload_filter");
	}

	void onSuper(cocos2d::CCObject* /* target */) {
		auto glm = GameLevelManager::sharedState();

		auto super_filter = glm->getBoolForKey("super_filter");
		glm->setBoolForKey(!super_filter, "super_filter");
	}


	bool init() {
		if (!MoreSearchLayer::init()) {
			return false;
		}

		auto toggle_menu = this->m_buttonMenu;

		auto glm = GameLevelManager::sharedState();
		auto filter_noreupload_toggled = glm->getBoolForKey("noreupload_filter");
		auto filter_supered_toggled = glm->getBoolForKey("super_filter");

		/*
		this->createToggleButton(
			"No Reupload",
			static_cast<cocos2d::SEL_MenuHandler>(&SearchMoreSearchLayer::onNoReupload),
			!filter_noreupload_toggled,
			toggle_menu,
			cocos2d::CCPoint(125.0f, 80.0f));
		*/

		auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
		float x = winSize.width * 0.5f - 140.0f;
		float y = winSize.height * 0.5f + 70.0f;

		this->createToggleButton(
			"Super",
			static_cast<cocos2d::SEL_MenuHandler>(&SearchMoreSearchLayer::onSuper),
			!filter_supered_toggled, toggle_menu, cocos2d::CCPoint(x, y - (50.0f * 3)));

		std::vector<cocos2d::CCArray*> song_objects = {
			this->m_songTabNodes, this->m_originalSongNodes, this->m_customSongNodes
		};

		for (const auto& song_object : song_objects) {
			for (auto i = 0u; i < song_object->count(); i++) {
				auto node = reinterpret_cast<cocos2d::CCNode*>(song_object->objectAtIndex(i));

				auto obj_position = node->getPosition();
				obj_position.x += 75.0f;

				node->setPosition(obj_position);
			}
		}

		m_enterSongID->m_maxLabelLength = 9;

		return true;
	}
};

struct SearchLevelSearchLayer : geode::Modify<SearchLevelSearchLayer, LevelSearchLayer> {
	void clearFilters() {
		LevelSearchLayer::clearFilters();

		auto glm = GameLevelManager::sharedState();
		glm->setBoolForKey(false, "noreupload_filter");
		glm->setBoolForKey(false, "super_filter");

		return;
	}

	GJSearchObject* getSearchObject(SearchType type, gd::string query) {
		auto search_object = LevelSearchLayer::getSearchObject(type, query);
		auto filter_supered_toggled = GameLevelManager::sharedState()->getBoolForKey("super_filter");

		if (filter_supered_toggled) {
			std::string searchQuery = search_object->m_searchQuery;
			search_object->m_searchQuery = searchQuery + "&epic=1";
		}

		return search_object;
	}
};

class PageSelectButton : public cocos2d::CCSprite, public PageSelectDelegate {
private:
	LevelBrowserLayer* m_currentBrowser{};
	cocos2d::CCLabelBMFont* m_pageLabel{nullptr};

public:
	void updatePageSelectBtn() {
		auto itemCount = m_currentBrowser->m_itemCount;

		auto pageCount = std::max((itemCount - 1) / 10, 0);
		auto pageStr = fmt::format("{}", m_currentBrowser->m_searchObject->m_page + 1);

		// reset the scale
		m_pageLabel->setScale(1.0f);

		m_pageLabel->setString(pageStr.c_str());
		m_pageLabel->limitLabelWidth(23.0f, 0.9f, 0.0f);

		this->setVisible(pageCount > 0);
	}

private:
	bool init(LevelBrowserLayer* currentBrowser) {
		if (!cocos2d::CCSprite::initWithFile("GJ_button_04.png")) {
			return false;
		}

		this->setScale(0.75f);

		m_pageLabel = cocos2d::CCLabelBMFont::create("1", "bigFont.fnt");
		this->addChild(m_pageLabel);

		auto currentScale = this->getContentSize();
		m_pageLabel->setPosition({currentScale.width / 2, currentScale.height / 2 + 2.0f});

		m_currentBrowser = currentBrowser;
		return true;
	}

public:
	static PageSelectButton* create(LevelBrowserLayer* currentBrowser) {
		auto pRet = new PageSelectButton();
		if (pRet && pRet->init(currentBrowser)) {
			pRet->autorelease();
			return pRet;
		} else {
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}

	virtual void onSelectPage(int selected) override {
		if (m_currentBrowser->m_searchObject->m_page == selected) {
			return;
		}

		GameManager::sharedState()->m_lastLevelID = 0;

		auto nextPageObject = m_currentBrowser->m_searchObject->getNextPageObject();
		nextPageObject->m_page = selected;

		m_currentBrowser->loadPage(nextPageObject);
	}

	void onPageSelectBtn(cocos2d::CCObject*) {
		auto pageCount = std::max((m_currentBrowser->m_itemCount - 1) / 10, 0);
		PageSelectPopup::create(this, m_currentBrowser->m_searchObject->m_page, pageCount)->show();
	}
};

struct SearchLevelBrowserLayer : geode::Modify<SearchLevelBrowserLayer, LevelBrowserLayer> {
	struct Fields {
		PageSelectButton* m_selectButton{nullptr};
	};

	void onHallOfFame(cocos2d::CCObject*) {
		// all this mess is to get it to return to the previous search object
		// (featured)
		GameManager::sharedState()->m_lastScene2 = LastGameScene::PreviousSearch;

		auto current_search = this->m_searchObject;

		GameLevelManager::sharedState()->m_lastSearchKey2 = current_search->getKey();

		auto hof_search = GJSearchObject::create(SearchType::HallOfFame);
		auto browser_scene = LevelBrowserLayer::scene(hof_search);

		auto fade_scene = cocos2d::CCTransitionFade::create(0.5f, browser_scene);
		cocos2d::CCDirector::sharedDirector()->replaceScene(fade_scene);
	}

	void updateLevelsLabel() {
		LevelBrowserLayer::updateLevelsLabel();

		if (m_fields->m_selectButton != nullptr) {
			m_fields->m_selectButton->setVisible(true);
			m_fields->m_selectButton->updatePageSelectBtn();
		}
	}

	void loadPage(GJSearchObject* page) {
		if (m_fields->m_selectButton != nullptr) {
			m_fields->m_selectButton->setVisible(false);
		}

		LevelBrowserLayer::loadPage(page);
	}

	void onRefresh(cocos2d::CCObject*) {
		auto key = this->m_searchObject->getKey();

		GameLevelManager::sharedState()->resetTimerForKey(key);

		this->loadPage(this->m_searchObject);
	}

	bool init(GJSearchObject* search) {
		if (!LevelBrowserLayer::init(search)) {
			return false;
		}

		cocos2d::CCMenu* br_menu = nullptr;
		auto search_type = search->m_searchType;

		// all of the things that could probably support refreshing
		if (search_type == SearchType::Featured ||
			search_type == SearchType::Downloaded ||
			search_type == SearchType::MostLiked ||
			search_type == SearchType::Trending ||
			search_type == SearchType::Recent ||
			search_type == SearchType::UsersLevels ||
			search_type == SearchType::Featured ||
			search_type == SearchType::Magic ||
			search_type == SearchType::Sends ||
			search_type == SearchType::HallOfFame ||
			search_type == SearchType::Search
		) {
			auto refresh_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png");
			auto refresh_btn = CCMenuItemSpriteExtra::create(
				refresh_sprite, nullptr, this,
				static_cast<cocos2d::SEL_MenuHandler>(&SearchLevelBrowserLayer::onRefresh)
			);

			refresh_btn->setID("refresh-btn"_spr);

			br_menu = cocos2d::CCMenu::createWithItem(refresh_btn);
			this->addChild(br_menu, 1);

			auto director = cocos2d::CCDirector::sharedDirector();

			auto pos_x = director->getScreenRight() - 26.0f;
			auto pos_y = director->getScreenBottom() + 26.0f;

			br_menu->setPosition(pos_x, pos_y);
			br_menu->setID("bottom-right-menu"_spr);
		}

		if (search->m_searchType == SearchType::Featured) {
			auto hof_sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_achBtn_001.png");
			hof_sprite->setScale(0.75f);

			auto hof_button = CCMenuItemSpriteExtra::create(
					hof_sprite, nullptr, this,
					static_cast<cocos2d::SEL_MenuHandler>(&SearchLevelBrowserLayer::onHallOfFame));

			br_menu->addChild(hof_button);
			hof_button->setPositionX(2.0f);
			hof_button->setPositionY(52.0f);
		}

		auto tr_menu = cocos2d::CCMenu::create();
		this->addChild(tr_menu, 1);

		auto director = cocos2d::CCDirector::sharedDirector();

		auto winSize = director->getWinSize();

		auto pos_x = director->getScreenRight() - 22.0f;
		auto pos_y = winSize.height / 2 + 122.0f;

		tr_menu->setPosition(pos_x, pos_y);

		auto selectBtn = PageSelectButton::create(this);
		auto pageBtn = CCMenuItemSpriteExtra::create(selectBtn, nullptr, selectBtn, static_cast<cocos2d::SEL_MenuHandler>(&PageSelectButton::onPageSelectBtn));
		pageBtn->setID("page-select-btn"_spr);

		tr_menu->addChild(pageBtn);

		// i think the ccmenuitemspriteextra forces it to visible
		selectBtn->updatePageSelectBtn();

		m_fields->m_selectButton = selectBtn;

		return true;
	}
};
