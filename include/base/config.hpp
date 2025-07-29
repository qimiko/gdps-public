#pragma once
#ifndef CONFIG_HPP
#define CONFIG_HPP

#ifndef GDMOD_SOURCE_URL
#define GDMOD_SOURCE_URL "https://github.com/qimiko/gdps-public"
#endif

#ifndef GDMOD_ENDPOINT_BASE_URL
#define GDMOD_ENDPOINT_BASE_URL "https://absolllute.com/gdps/gdapi"
#endif

#include <Geode/Geode.hpp>
#include <fmt/format.h>
#include "cmake_config.hpp"

namespace Config {
constexpr auto SOURCE_URL = GDMOD_SOURCE_URL;
constexpr auto USER_AGENT = "1.9 GDPS/" CMAKE_CONFIG_VERSION " (" GEODE_PLATFORM_SHORT_IDENTIFIER ")";
}

#endif