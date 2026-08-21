cmake_minimum_required (VERSION 3.20)
project ("rezoriri_deps")

# "rezoriri" depends on LZ4
# "rezoriri_player" depends on raylib, spine-cpp, Dear-imgui and rlImGui

set(LZ4_VERSION "1.10.0")
set(LZ4_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})

set(RAYLIB_VERSION "6.0")
set(RAYLIB_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/raylib-${RAYLIB_VERSION}")

set(SPINE_VERSION "4.2")
set(SPINE_RUNTIME_LANGUAGE "spine-cpp")
set(SPINE_RUNTIME_DIR ${CMAKE_CURRENT_LIST_DIR}/${SPINE_RUNTIME_LANGUAGE}-${SPINE_VERSION})

set(IMGUI_VERSION "1.92.7")
set(IMGUI_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/imgui-${IMGUI_VERSION}")

set(RLIMGUI_VERSION "Raylib_6_0")
set(RLIMGUI_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/rlImGui-${RLIMGUI_VERSION}")

set(TEMP_DIR ${CMAKE_CURRENT_BINARY_DIR}/temp)

function(download_and_extract HOST_URL ARCHIVE_NAME)
	if (NOT EXISTS "${TEMP_DIR}/${ARCHIVE_NAME}.zip")
		file(DOWNLOAD "${HOST_URL}" "${TEMP_DIR}/${ARCHIVE_NAME}.zip" SHOW_PROGRESS)
	endif()

	if (NOT EXISTS "${TEMP_DIR}/${ARCHIVE_NAME}")
		file(ARCHIVE_EXTRACT INPUT "${TEMP_DIR}/${ARCHIVE_NAME}.zip" DESTINATION "${TEMP_DIR}")
	endif()
endfunction()

# Download LZ4
if (NOT EXISTS "${LZ4_SOURCE_DIR}/lz4.c" OR NOT EXISTS "${LZ4_SOURCE_DIR}/lz4.h")
	download_and_extract("https://github.com/lz4/lz4/archive/refs/tags/v${LZ4_VERSION}.zip" "lz4-${LZ4_VERSION}")
	
	file(COPY "${TEMP_DIR}/lz4-${LZ4_VERSION}/lib/lz4.c" DESTINATION "${LZ4_SOURCE_DIR}")
	file(COPY "${TEMP_DIR}/lz4-${LZ4_VERSION}/lib/lz4.h" DESTINATION "${LZ4_SOURCE_DIR}")
endif()

# Download Raylib
if (NOT EXISTS "${RAYLIB_SOURCE_DIR}")
	set (RAYLIB_FOLDER_NAME "raylib-${RAYLIB_VERSION}")
	download_and_extract("https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.zip" "${RAYLIB_FOLDER_NAME}")
	# Huge project files for examples are bothersome
	file(COPY "${TEMP_DIR}/${RAYLIB_FOLDER_NAME}/"
		DESTINATION "${RAYLIB_SOURCE_DIR}"
		PATTERN ".github" EXCLUDE
		PATTERN "projects" EXCLUDE
		PATTERN "examples" EXCLUDE
		PATTERN "tools" EXCLUDE
		)
endif()

# Download Spine generic runtime
if (NOT EXISTS "${SPINE_RUNTIME_DIR}/src")
	set (SPINE_RUNTIME_URL "https://github.com/EsotericSoftware/spine-runtimes/archive/refs/heads/${SPINE_VERSION}.zip")
	set (SPINE_FOLDER_NAME "spine-runtimes-${SPINE_VERSION}")
	download_and_extract("${SPINE_RUNTIME_URL}" "${SPINE_FOLDER_NAME}")

	file(COPY "${TEMP_DIR}/${SPINE_FOLDER_NAME}/${SPINE_RUNTIME_LANGUAGE}/${SPINE_RUNTIME_LANGUAGE}/src" DESTINATION "${SPINE_RUNTIME_DIR}")
	file(COPY "${TEMP_DIR}/${SPINE_FOLDER_NAME}/${SPINE_RUNTIME_LANGUAGE}/${SPINE_RUNTIME_LANGUAGE}/include" DESTINATION "${SPINE_RUNTIME_DIR}")
endif()

# Download Dear-ImGui
if (NOT EXISTS "${IMGUI_SOURCE_DIR}")
	set (IMGUI_FOLDER_NAME "imgui-${IMGUI_VERSION}")
		download_and_extract("https://github.com/ocornut/imgui/archive/refs/tags/v${IMGUI_VERSION}.zip" "${IMGUI_FOLDER_NAME}")
	# Exclude project files
	file(COPY "${TEMP_DIR}/${IMGUI_FOLDER_NAME}/"
		DESTINATION "${IMGUI_SOURCE_DIR}"
		FILES_MATCHING 
			PATTERN "*.cpp"
			PATTERN "*.h"
			PATTERN ".github" EXCLUDE
			PATTERN "backends" EXCLUDE
			PATTERN "docs" EXCLUDE
			PATTERN "examples" EXCLUDE
			PATTERN "misc" EXCLUDE
		)

endif()

# Download rlImGui
if (NOT EXISTS "${RLIMGUI_SOURCE_DIR}")
	set (RLIMGUI_FOLDER_NAME "rlImGui-${RLIMGUI_VERSION}")
	download_and_extract("https://github.com/raylib-extras/rlImGui/archive/refs/tags/${RLIMGUI_VERSION}.zip" "${RLIMGUI_FOLDER_NAME}")
	
	file(COPY "${TEMP_DIR}/${RLIMGUI_FOLDER_NAME}/" DESTINATION "${RLIMGUI_SOURCE_DIR}")
endif()

# Delete temporary files
file(REMOVE_RECURSE "${TEMP_DIR}")

# Build Spine generic runtime as static library
add_subdirectory ("${SPINE_RUNTIME_DIR}")

# Build Raylib
add_subdirectory ("${RAYLIB_SOURCE_DIR}")
