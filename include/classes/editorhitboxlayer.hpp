#pragma once

#ifndef CLASSES_EDITORHITBOXLAYER_HPP
#define CLASSES_EDITORHITBOXLAYER_HPP

#include <array>
#include <Geode/Geode.hpp>

class EditorHitboxLayer : public cocos2d::CCLayer {
private:
    cocos2d::CCDrawNode* _hitboxNode{nullptr};
    bool _skipHitboxUpdates{false};

    void drawSlope(GameObject* obj, cocos2d::ccColor4F& color);
    void drawCircle(GameObject* obj, const cocos2d::ccColor4F& color);

    static constexpr float HITBOX_WIDTH = 0.5f;

public:
    virtual bool init() override;

    void beginUpdate();
    void drawObject(GameObject* object, int groupFilter);
    void drawPlayer(PlayerObject* player);
    void drawVerticalLine(float x);

    void setSkipHitboxUpdates(bool skip) {
        this->_skipHitboxUpdates = skip;
    }

    CREATE_FUNC(EditorHitboxLayer);
};

#endif //CLASSES_EDITORHITBOXLAYER_HPP
