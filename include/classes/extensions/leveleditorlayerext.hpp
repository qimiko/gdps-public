#pragma once

#ifndef CLASSES_EXTENSIONS_LEVELEDITORLAYEREXT_HPP
#define CLASSES_EXTENSIONS_LEVELEDITORLAYEREXT_HPP

#include <Geode/Geode.hpp>
#include "classes/editorhitboxlayer.hpp"

class LevelEditorLayerExt : public cocos2d::CCObject {
private:
    bool _hitboxesEnabled{false};
    bool _hasShownPlayer{false};
    EditorHitboxLayer* _hitboxLayer{nullptr};

public:
    bool getHitboxesEnabled() const { return this->_hitboxesEnabled; };
    void setHitboxesEnabled(bool enabled) { this->_hitboxesEnabled = enabled; }

    bool getHasShownPlayer() const { return this->_hasShownPlayer; };
    void setHasShownPlayer(bool shown) { this->_hasShownPlayer = shown; }

    EditorHitboxLayer* getHitboxLayer() const { return this->_hitboxLayer; }
    void setHitboxLayer(EditorHitboxLayer* node) { this->_hitboxLayer = node; }
};

#endif //CLASSES_EXTENSIONS_LEVELEDITORLAYEREXT_HPP
