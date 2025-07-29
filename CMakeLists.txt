cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(aurav2)
set(aurav2_VERSION 9.0.0)
set(GEODE_MOD_ID zmx.aura)

option(GDMOD_ENABLE_DEV_OVERLAY "Enable ImGui and logging constants" OFF)

set(GDMOD_SOURCES
  src/classes/advancedcolorselectwidget.cpp
  src/classes/loadingfailedhijacklayer.cpp
  src/classes/editorhitboxlayer.cpp
  src/classes/sessionwidget.cpp
  src/classes/creditsnode.cpp
  src/classes/creditsline.cpp
  src/classes/creditspopup.cpp
  src/classes/gameplaysettingspopup.cpp
  src/classes/uploadactionpopup.cpp
  src/classes/pageselectpopup.cpp
  src/classes/remaptriggerpopup.cpp
  src/classes/mainlevelinfopopup.cpp
  src/classes/statusindicator.cpp
  src/classes/callbacks/levelimportcallback.cpp
  src/classes/managers/outfitmanager.cpp
  src/classes/managers/overlaymanager.cpp
  src/classes/managers/tokenmanager.cpp
  src/classes/managers/secretmanager.cpp
  src/classes/speedhack/speedhackmanagercard.cpp
  src/classes/settings/settingspage.cpp
  src/classes/settings/togglesettingspage.cpp
  src/classes/settings/settingspopup.cpp
  src/classes/settings/texturesettingspopup.cpp
  src/classes/settings/pages/aboutsettingspage.cpp
  src/classes/settings/pages/gameplaysettingspage.cpp
  src/classes/settings/pages/interfacesettingspage.cpp
  src/classes/settings/pages/tweakssettingspage.cpp
  src/classes/settings/pages/editorsettingspage.cpp
  src/classes/settings/pages/songssettingspage.cpp
  src/classes/settings/pages/graphicssettingspage.cpp
  src/classes/commands/rateleveldialog.cpp
  src/classes/commands/commanddispatcher.cpp
  src/classes/extensions/playlayerext.cpp
  src/modules/default.cpp
  src/modules/levels.cpp
  src/modules/hacks.cpp
  src/modules/hacks/speedhack.cpp
  src/modules/limits.cpp
  src/modules/textures.cpp
  src/modules/badges.cpp
  src/modules/search.cpp
  src/modules/settings.cpp
  src/modules/urls.cpp
  src/modules/share.cpp
  src/modules/icons.cpp
  src/modules/editor.cpp
  src/modules/token.cpp
  src/modules/overlay.cpp
  src/modules/physics.cpp
  src/modules/web.cpp
  src/overlay/code.cpp
  src/overlay/items.cpp
  src/overlay/menu.cpp
)

if (ANDROID)
  set(GDMOD_SOURCES ${GDMOD_SOURCES}
    src/base/platform/android.cpp
    src/classes/controlleroverlay.cpp
    src/classes/managers/controllermanager.cpp
    src/classes/callbacks/showloadingcirclecallback.cpp
    src/classes/callbacks/reloadgamecallback.cpp
    src/classes/callbacks/inputcallback.cpp
    src/modules/input.cpp
  )
elseif(WIN32)
  set(GDMOD_SOURCES ${GDMOD_SOURCES}
    src/base/platform/windows.cpp
    src/classes/managers/discordmanager.cpp
    src/modules/presence.cpp
  )
else()
  message(FATAL_ERROR "Unsupported platform :(")
endif()

if (GDMOD_ENABLE_DEV_OVERLAY)
  set(GDMOD_SOURCES ${GDMOD_SOURCES} src/modules/overlay-dev.cpp)
endif()

add_library(aurav2 SHARED ${GDMOD_SOURCES})

# set_property(TARGET aurav2 PROPERTY CXX_CLANG_TIDY "${ANDROID_TOOLCHAIN_ROOT}/bin/clang-tidy${ANDROID_TOOLCHAIN_SUFFIX}")

set(GDMOD_INCLUDE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(aurav2 PRIVATE ${GDMOD_INCLUDE})

find_package(Git)
execute_process(COMMAND
  "${GIT_EXECUTABLE}" describe --always --abbrev=7 --dirty=+ --exclude \\*
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_SHA1
  ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND
  "${GIT_EXECUTABLE}" symbolic-ref -q --short HEAD
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULTS_VARIABLE GIT_BRANCH_RESULT)

if (GIT_BRANCH_RESULT AND NOT GIT_BRANCH_RESULT EQUAL 0)
  execute_process(COMMAND
    "${GIT_EXECUTABLE}" describe --tags --exact-match
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE GIT_BRANCH
    ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/base/cmake_config.hpp.in" "${CMAKE_CURRENT_SOURCE_DIR}/include/base/cmake_config.hpp" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources/about.md.in" "${CMAKE_CURRENT_SOURCE_DIR}/resources/about.md" @ONLY)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resources/mod.json.in" "${CMAKE_CURRENT_SOURCE_DIR}/resources/mod.json" @ONLY)
target_compile_definitions(${PROJECT_NAME} PRIVATE GEODE_MOD_ID="${GEODE_MOD_ID}")

include(cmake/CPM.cmake)

CPMAddPackage("gh:fmtlib/fmt#11.2.0")
target_link_libraries(aurav2 fmt)

CPMAddPackage("gh:zeux/pugixml@1.15")
target_link_libraries(aurav2 pugixml)

CPMAddPackage(
  NAME gd-imgui-cocos
  GITHUB_REPOSITORY qimiko/gd-imgui-cocos
  GIT_TAG 083e6072e80f55572c07412619ce414b09ea57ee
  OPTIONS "ENABLE_FREETYPE ON"
)
target_link_libraries(aurav2 imgui-cocos)

if (WIN32)
  CPMAddPackage("gh:qimiko/discord-presence#9ea90322a20a43b02e1f7d630e0e6b24a26329c5")
  target_link_libraries(aurav2 discord-rpc)
endif()

if (GDMOD_ENABLE_DEV_OVERLAY)
  target_compile_definitions(aurav2 PRIVATE -DGDMOD_ENABLE_LOGGING)
endif()

if (GDMOD_SOURCE_URL)
  target_compile_definitions(aurav2 PRIVATE "-DGDMOD_SOURCE_URL=\"${GDMOD_SOURCE_URL}\"")
endif()

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

set(GDMOD_RESOURCES_PATH ${CMAKE_BINARY_DIR}/resources CACHE PATH "Output directory for resources")

package_geode_resources_now(
  ${PROJECT_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/resources
  ${GDMOD_RESOURCES_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/include/base/resources.hpp
)

target_link_libraries(${PROJECT_NAME} geode-sdk)
