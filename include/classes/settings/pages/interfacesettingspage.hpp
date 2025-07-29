#pragma once

#ifndef CLASSES_SETTINGS_INTERFACESETTINGSPAGE_HPP
#define CLASSES_SETTINGS_INTERFACESETTINGSPAGE_HPP

#include <Geode/Geode.hpp>

#include "base/game_variables.hpp"

#include "classes/settings/togglesettingspage.hpp"

class InterfaceSettingsPage : public ToggleSettingsPage {
public:
    void createPage() override;

    CREATE_FUNC(InterfaceSettingsPage); // NOLINT(modernize-use-auto)
};

#endif //CLASSES_SETTINGS_INTERFACESETTINGSPAGE_HPP
