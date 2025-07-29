#pragma once

#ifndef CLASSES_SETTINGS_EDITORSETTINGSPAGE_HPP
#define CLASSES_SETTINGS_EDITORSETTINGSPAGE_HPP

#include <Geode/Geode.hpp>

#include "base/game_variables.hpp"

#include "classes/settings/togglesettingspage.hpp"

class EditorSettingsPage : public ToggleSettingsPage {
public:
    void createPage() override;

    CREATE_FUNC(EditorSettingsPage); // NOLINT(modernize-use-auto)
};

#endif //CLASSES_SETTINGS_EDITORSETTINGSPAGE_HPP
