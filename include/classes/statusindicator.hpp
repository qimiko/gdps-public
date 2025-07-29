#pragma once

#ifndef CLASSES_STATUSINDICATOR_HPP
#define CLASSES_STATUSINDICATOR_HPP

#include <Geode/Geode.hpp>
#include <string_view>

class StatusIndicator : public cocos2d::CCNode {
private:
	cocos2d::CCLabelBMFont* m_statusLabel{nullptr};
	cocos2d::extension::CCScale9Sprite* m_background{nullptr};

	void setFadeToNode(cocos2d::CCNode*);

public:
	virtual bool init() override;

	void setLabel(const char* label);
	void show();


	CREATE_FUNC(StatusIndicator);
};

#endif //CLASSES_STATUSINDICATOR_HPP
