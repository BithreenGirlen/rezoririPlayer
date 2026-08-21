cmake_minimum_required(VERSION 3.20)
project(rezoririPlayer)

set(TARGET_NAME "rezoririPlayer")

set(DEPS_DIR "${CMAKE_CURRENT_LIST_DIR}/../deps")
set(IMGUI_DIR "${DEPS_DIR}/imgui-1.92.7")
set(RLIMGUI_DIR "${DEPS_DIR}/rlImGui-Raylib_6_0")
set(SPINE_GENERIC_RUNTIME_DIR "${DEPS_DIR}/spine-cpp-4.2")

set(RAYLIB_SPINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib-spine")
set(RAYLIB_IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/raylib-imgui")

# Copy setup file
if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/setting.txt")
	file(COPY "${DEPS_DIR}/setting.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

add_executable(
	${TARGET_NAME}
	"main.cpp"
	"raylib_input.cpp" "raylib_input.h"
	"raylib_main_window.cpp" "raylib_main_window.h"
	"raylib_handle.h"
	"raylib_filesystem_utility.cpp" "raylib_filesystem_utility.h"
	"raylib_render_target_scope.cpp" "raylib_render_target_scope.h"
	"raylib_text_drawer.cpp" "raylib_text_drawer.h"
	"raylib_clock.cpp" "raylib_clock.h"
	"path_utility.h"
	"text_utility.h"
	"adv.h"
	"rezoriri.cpp" "rezoriri.h"
	"rezoriri_scene_crafter.cpp" "rezoriri_scene_crafter.h"
	"json_minimal.cpp" "json_minimal.h"
	"windows/win_dialogue.cpp" "windows/win_dialogue.h"
	"../rezoriri/win_text.cpp" "../rezoriri/win_text.h"
	"windows/mf_media_player.cpp" "windows/mf_media_player.h"
	"${RAYLIB_SPINE_DIR}/raylib_spine_drawable.cpp" "${RAYLIB_SPINE_DIR}/raylib_spine_drawable.h"
	"${RAYLIB_SPINE_DIR}/raylib_spine_texture_loader.cpp" "${RAYLIB_SPINE_DIR}/raylib_spine_texture_loader.h"
	"${RAYLIB_SPINE_DIR}/raylib_spine_extension.cpp" "${RAYLIB_SPINE_DIR}/raylib_spine_extension.h"
	"${RAYLIB_SPINE_DIR}/raylib_spine_player.cpp" "${RAYLIB_SPINE_DIR}/raylib_spine_player.h"
	"${RAYLIB_SPINE_DIR}/spine_loader.cpp" "${RAYLIB_SPINE_DIR}/spine_loader.h"
	"${RAYLIB_SPINE_DIR}/spine_player.cpp" "${RAYLIB_SPINE_DIR}/spine_player.h"
	"${RAYLIB_SPINE_DIR}/spine_file_verifier.cpp" "${RAYLIB_SPINE_DIR}/spine_file_verifier.h"
	"${IMGUI_DIR}/imconfig.h"
	"${IMGUI_DIR}/imgui.cpp" "${IMGUI_DIR}/imgui.h"
	"${IMGUI_DIR}/imgui_demo.cpp"
	"${IMGUI_DIR}/imgui_draw.cpp"
	"${IMGUI_DIR}/imgui_internal.h"
	"${IMGUI_DIR}/imgui_tables.cpp"
	"${IMGUI_DIR}/imgui_widgets.cpp"
	"${RLIMGUI_DIR}/rlImGui.cpp" "${RLIMGUI_DIR}/rlImGui.h"
	"${RAYLIB_IMGUI_DIR}/raylib_imgui_init.cpp" "${RAYLIB_IMGUI_DIR}/raylib_imgui_init.h"
	)

target_include_directories(
	${TARGET_NAME} PRIVATE
	"${SPINE_GENERIC_RUNTIME_DIR}/include"
	"${IMGUI_DIR}"
	"${RLIMGUI_DIR}"
)

target_link_libraries(${TARGET_NAME} PRIVATE
	"spine-cpp-4.2"
	"raylib"
)

target_compile_definitions(
	${TARGET_NAME} PRIVATE
	UNICODE _UNICODE
	SPINE_42
	NO_FONT_AWESOME
)

if(MSVC)
	target_compile_definitions(${TARGET_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS)
	target_compile_options(${TARGET_NAME} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/d1trimfile:${CMAKE_SOURCE_DIR}\\>")
	target_link_options(${TARGET_NAME} PRIVATE "/pdbaltpath:%_PDB%")
endif()
