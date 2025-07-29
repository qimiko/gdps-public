cmake_minimum_required(VERSION 3.5)
project(ImGui)

include(../../CPM.cmake)
CPMAddPackage(
	NAME imgui
	GITHUB_REPOSITORY ocornut/imgui
	VERSION 1.91.4
	DOWNLOAD_ONLY YES
)

if (imgui_ADDED)
	add_library(ImGui STATIC
		${imgui_SOURCE_DIR}/imgui_tables.cpp
		${imgui_SOURCE_DIR}/imgui_widgets.cpp
		${imgui_SOURCE_DIR}/imgui_draw.cpp
		${imgui_SOURCE_DIR}/imgui_demo.cpp
		${imgui_SOURCE_DIR}/imgui.cpp

		${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
	)

	target_include_directories(ImGui PUBLIC
		${imgui_SOURCE_DIR}
	)

	if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
		target_compile_definitions(ImGui PRIVATE -DIMGUI_IMPL_OPENGL_ES2)
		target_link_libraries(ImGui GLESv2)
		target_link_libraries(ImGui android)
	endif()
endif()
