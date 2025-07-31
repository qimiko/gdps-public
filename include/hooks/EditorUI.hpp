#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>

#include "classes/statusindicator.hpp"

struct CustomEditorUI : geode::Modify<CustomEditorUI, EditorUI> {
	struct Fields : ColorSelectDelegate {
		cocos2d::CCLabelBMFont* m_infoLabel{nullptr};
		bool m_updateInfoLabel{false};

		geode::Ref<GameObject> m_multiColorObject{};
		EditorUI* m_self{nullptr};

		StatusIndicator* m_statusIndicator{};

		void colorSelectClosed(ColorSelectPopup* popup) override;
	};

	// persistent clipboard
	void onCopy(cocos2d::CCObject* target);

	bool init(LevelEditorLayer *editor);

	cocos2d::CCMenu* get_menu_bar_for_index(uint32_t ind, uint32_t page);

	void setupCreateMenu();

	void updateInfoLabel(float);
	void scheduleInfoLabelUpdate();

	virtual void angleChanged(float angle) override;
	void selectObject(GameObject* object);
	void moveObject(GameObject* object, cocos2d::CCPoint to);
	void deselectObject(GameObject* object);
	void deselectAll();
	void selectObjects(cocos2d::CCArray* objects);
	void transformObjectCall(EditCommand command);

	bool multiSelectingColorTriggers();

	void editObject(cocos2d::CCObject* callback);

	bool editButtonUsable();

	void keyDown(cocos2d::enumKeyCodes key) override;
	void keyUp(cocos2d::enumKeyCodes key) override;
	void scrollWheel(float y, float x) override;
};
