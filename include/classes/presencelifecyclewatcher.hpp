#ifndef CLASSES_PRESENCELIFECYCLEWATCHER_HPP
#define CLASSES_PRESENCELIFECYCLEWATCHER_HPP

#include <Geode/Geode.hpp>

#include "classes/managers/discordmanager.hpp"

// hacky but allows me to avoid writing custom destructor code
class PresenceLifecycleWatcher : public cocos2d::CCNode {
public:
	virtual ~PresenceLifecycleWatcher() override {
		DiscordManager::get().decrementIdleCounter();
	}

	void begin() {
		DiscordManager::get().incrementIdleCounter();
	}

	CREATE_FUNC(PresenceLifecycleWatcher);
};

#endif
