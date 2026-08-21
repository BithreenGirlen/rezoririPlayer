cmake_minimum_required (VERSION 3.20)
project ("rezoriri")

set(TARGET_NAME "rezoriri")
set(LZ4_DIR "${CMAKE_CURRENT_LIST_DIR}/../deps")

add_executable (
	${TARGET_NAME}
	"main.cpp"
	"win_internet_utility.cpp" "win_internet_utility.h"
	"win_text.cpp" "win_text.h"
	"binary_utility.h"
	"static_string.h"
	"unity_bundle.cpp" "unity_bundle.h"
	"${LZ4_DIR}/lz4.c" "${LZ4_DIR}/lz4.h"
)

target_include_directories(
	${TARGET_NAME} PRIVATE
	${LZ4_DIR}
)

target_compile_definitions(
	${TARGET_NAME} PRIVATE
	UNICODE _UNICODE
)

if(MSVC)
	target_compile_definitions(${TARGET_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS)
	target_compile_options(${TARGET_NAME} PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/d1trimfile:${CMAKE_SOURCE_DIR}\\>")
	target_link_options(${TARGET_NAME} PRIVATE "/pdbaltpath:%_PDB%")
endif()
