//
// Created by dslkjkld on 1/29/23.
//

#ifndef CLASSES_SPEEDHACK_SPEEDHACKLIFECYCLEWATCHER_HPP
#define CLASSES_SPEEDHACK_SPEEDHACKLIFECYCLEWATCHER_HPP

#include <Geode/Geode.hpp>

#include "classes/speedhack/speedhackmanagercard.hpp"

// hacky but allows me to avoid writing custom destructor code
class SpeedhackLifecycleWatcher : public cocos2d::CCNode {
    bool m_hasIncremented{false};

public:
    void endSpeedhack() {
        if (m_hasIncremented) {
            SpeedhackManagerCard::decrementLifetime();
            m_hasIncremented = false;
        }
    }

    ~SpeedhackLifecycleWatcher() override {
        this->endSpeedhack();
    }

    void beginSpeedhack() {
        if (!m_hasIncremented) {
            SpeedhackManagerCard::incrementLifetime();
            m_hasIncremented = true;
        }
    }

    CREATE_FUNC(SpeedhackLifecycleWatcher);
};

#endif //CLASSES_SPEEDHACK_SPEEDHACKLIFECYCLEWATCHER_HPP
