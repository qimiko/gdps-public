#pragma once

#ifndef CLASSES_PAGESELECTPOPUP_HPP
#define CLASSES_PAGESELECTPOPUP_HPP

#include <Geode/Geode.hpp>

class PageSelectDelegate {
public:
	virtual void onSelectPage(int page) {};
};

class PageSelectPopup : public FLAlertLayer, TextInputDelegate {
private:
	PageSelectDelegate* m_delegate{nullptr};
	CCTextInputNode* m_pageInput{nullptr};
	CCMenuItemSpriteExtra* m_prevBtn{nullptr};
	CCMenuItemSpriteExtra* m_nextBtn{nullptr};
	CCMenuItemSpriteExtra* m_firstBtn{nullptr};
	CCMenuItemSpriteExtra* m_lastBtn{nullptr};
	cocos2d::CCLabelBMFont* m_pageInfo{nullptr};
	int m_maxPage{0};
	int m_selectedPage{0};
	int m_initialPage{0};

	bool m_updatingLabels{false};

	bool init(PageSelectDelegate* delegate, int page, int maxPages);
	virtual void keyBackClicked() override;

	void onFirstPage(cocos2d::CCObject*);
	void onLastPage(cocos2d::CCObject*);

	void onNextPage(cocos2d::CCObject*);
	void onPrevPage(cocos2d::CCObject*);

	void updateLabels(bool skipInput = false);

	virtual void textInputClosed(CCTextInputNode*) override;
	virtual void textChanged(CCTextInputNode*) override;

	void onClose(cocos2d::CCObject*);

public:
	static PageSelectPopup* create(PageSelectDelegate* delegate, int page, int maxPages) {
		auto pRet = new PageSelectPopup();
		if (pRet && pRet->init(delegate, page, maxPages)) {
			pRet->autorelease();
			return pRet;
		} else {
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}
};

#endif // CLASSES_PAGESELECTPOPUP_HPP
