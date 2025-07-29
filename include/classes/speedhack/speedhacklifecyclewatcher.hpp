//
// Created by dslkjkld on 1/29/23.
//

#ifndef CLASSES_SPEEDHACK_SPEEDHACKLIFECYCLEWATCHER_HPP
#define CLASSES_SPEEDHACK_SPEEDHACKLIFECYCLEWATCHER_HPP

#include <Geode/Geode.hpp>

#include "classes/speedhack/speedhackmanagercard.hpp"

// hacky but allows me to avoid writing custom destructor code
class SpeedhackLifecycleWatcher : public cocos2d::CCNode {
public:
    ~SpeedhackLifecycleWatcher() override {
        SpeedhackManagerCard::decrementLifetime();
    }

    void beginSpeedhack() {
        SpeedhackManagerCard::incrementLifetime();
    }

    CREATE_FUNC(SpeedhackLifecycleWatcher);
};

#endif //CLASSES_SPEEDHACK_SPEEDHACKLIFECYCLEWATCHER_HPP
