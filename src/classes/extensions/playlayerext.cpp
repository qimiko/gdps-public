#include "classes/extensions/playlayerext.hpp"

#include <fmt/format.h>

#include "base/game_variables.hpp"
#include "classes/speedhack/speedhackmanagercard.hpp"

bool PlayLayerExt::determineCheatStatus() {
	if (GameManager::sharedState()->getGameVariable(GameVariable::IGNORE_DAMAGE)) {
		return true;
	}

	if (SpeedhackManagerCard::isSpeedhackActive()) {
		return true;
	}

	if (this->isFalseCompletion_) {
		return true;
	}

	return false;
}

void PlayLayerExt::updateIndicators() {
	if (timeLabel_ && timeIcon_) {
		auto speedhack_interval = SpeedhackManagerCard::getSpeedhackInterval();
		auto speedhack_string = fmt::format("{:.2f}x", speedhack_interval);

		timeLabel_->setString(speedhack_string.c_str());

		auto speedhack_active = SpeedhackManagerCard::isSpeedhackActive();
		timeLabel_->setVisible(speedhack_active);
		timeIcon_->setVisible(speedhack_active);
	}

	if (cheatIndicator_) {
		auto cheat_status = this->determineCheatStatus();
		if (cheat_status) {
			cheatIndicator_->setColor({0xff, 0x00, 0x00});
			cheatIndicator_->setVisible(true);
		} else if (isIllegitimate_) {
			cheatIndicator_->setColor({0xff, 0xaa, 0x00});
		} else {
			cheatIndicator_->setColor({0x00, 0xff, 0x00});
			cheatIndicator_->setVisible(false);
		}
	}
}

void PlayLayerExt::resetCheats() {
	this->isIllegitimate_ = this->determineCheatStatus();
	this->updateIndicators();
}

void PlayLayerExt::updateCheats() {
	this->isIllegitimate_ = this->isIllegitimate_ || this->determineCheatStatus();
	this->updateIndicators();
}
