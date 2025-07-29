#pragma once

#ifndef CLASSES_REMAPTRIGGERPOPUP_HPP
#define CLASSES_REMAPTRIGGERPOPUP_HPP

#include <Geode/Geode.hpp>

#include <vector>

class RemapTriggerPopup : public FLAlertLayer {
private:
	std::vector<ButtonSprite*> m_buttons{};

	constexpr static float m_width = 350.0f;
	constexpr static float m_height = 200.0f;

	int m_selected{0};
	GameObject* m_gameObject{nullptr};
	ColorSelectPopup* m_parentPopup{nullptr};
	bool m_selectionDirty{false};

	EditorUI* m_editorUi{nullptr};

	bool init(GameObject* object, ColorSelectPopup* popup);
	virtual void keyBackClicked() override;

	void remapObject(GameObject*);
	void remapObjects();

	void onClose(cocos2d::CCObject*);
	void onSelect(cocos2d::CCObject*);

	void onSelectId(int id);

	void addButton(const char* title, int id);

	cocos2d::CCPoint getNextButtonPos();

public:
	static RemapTriggerPopup* create(GameObject* object, ColorSelectPopup* popup) {
		auto pRet = new RemapTriggerPopup();
		if (pRet && pRet->init(object, popup)) {
			pRet->autorelease();
			return pRet;
		} else {
			delete pRet;
			pRet = nullptr;
			return nullptr;
		}
	}

	void setEditorUI(EditorUI* ui) {
		this->m_editorUi = ui;
	}
};

#endif // CLASSES_REMAPTRIGGERPOPUP_HPP
