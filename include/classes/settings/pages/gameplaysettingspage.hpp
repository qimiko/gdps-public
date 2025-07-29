#pragma once

#ifndef CLASSES_SETTINGS_GAMEPLAYSETTINGSPAGE_HPP
#define CLASSES_SETTINGS_GAMEPLAYSETTINGSPAGE_HPP

#include <Geode/Geode.hpp>

#include "base/game_variables.hpp"

#include "classes/settings/togglesettingspage.hpp"

class GameplaySettingsPage : public ToggleSettingsPage {
public:
    void createPage() override;

    CREATE_FUNC(GameplaySettingsPage); // NOLINT(modernize-use-auto)
};

#endif //CLASSES_SETTINGS_GAMEPLAYSETTINGSPAGE_HPP
