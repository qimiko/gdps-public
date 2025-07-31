#pragma once

#ifndef CLASSES_EXTENSIONS_PLAYLAYEREXT_HPP
#define CLASSES_EXTENSIONS_PLAYLAYEREXT_HPP

#include <Geode/Geode.hpp>
#include <vector>

#include "classes/editorhitboxlayer.hpp"
#include "classes/speedhack/speedhacklifecyclewatcher.hpp"

class PlayLayerExt : public cocos2d::CCObject {
    bool isIllegitimate_{false};
    EditorHitboxLayer* hitboxLayer_{nullptr};
    bool isFalseCompletion_{false};
    cocos2d::CCSprite* timeIcon_{nullptr};
    cocos2d::CCLabelBMFont* timeLabel_{nullptr};
    cocos2d::CCLabelBMFont* cheatIndicator_{nullptr};
    std::vector<StartPosObject*> startPositions_{};
    std::uint32_t startPositionIdx_{0u};
    geode::Ref<SpeedhackLifecycleWatcher> speedhackWatcher_{};
    bool pausingSafe_{false};

    void updateIndicators();

public:
    PlayLayerExt() = default;

    bool getIsIllegitimate() const {
        return this->isIllegitimate_;
    }

    EditorHitboxLayer* getHitboxLayer() const {
        return this->hitboxLayer_;
    }

    void setHitboxLayer(EditorHitboxLayer* hitboxLayer) {
        this->hitboxLayer_ = hitboxLayer;
    }

    void setIsFalseCompletion(bool isFalseCompletion) {
        this->isFalseCompletion_ = isFalseCompletion;
    }

    bool getIsFalseCompletion() const {
        return this->isFalseCompletion_;
    }

    void setTimeIcon(cocos2d::CCSprite* icon) {
        this->timeIcon_ = icon;
    }

    void setTimeLabel(cocos2d::CCLabelBMFont* label) {
        this->timeLabel_ = label;
    }

    void setCheatIndicator(cocos2d::CCLabelBMFont* indicator) {
        this->cheatIndicator_ = indicator;
    }

    std::vector<StartPosObject*> getStartPositions() {
        return this->startPositions_;
    }

    void insertStartPosition(StartPosObject* obj) {
        this->startPositions_.push_back(obj);
    }

    std::uint32_t getStartPositionIdx() const {
        return this->startPositionIdx_;
    }

    void setStartPositionIdx(std::uint32_t value) {
        this->startPositionIdx_ = value;
    }

    void setSpeedhackWatcher(SpeedhackLifecycleWatcher* watcher) {
        this->speedhackWatcher_ = watcher;
    }

    SpeedhackLifecycleWatcher* getSpeedhackWatcher() const {
        return *this->speedhackWatcher_;
    }

    void setPausingSafe(bool v) {
        this->pausingSafe_ = v;
    }

    bool getPausingSafe() const {
        return this->pausingSafe_;
    }

    // determines if any modifications that are cheats are enabled, but does not set status
    bool determineCheatStatus();

    // "reset" cheat status, should be run on the beginning of an attempt
    void resetCheats();

    // "updates" cheat status, intended to run mid-attempt and can only enable cheat status
    void updateCheats();
};

#endif //CLASSES_EXTENSIONS_PLAYLAYEREXT_HPP
