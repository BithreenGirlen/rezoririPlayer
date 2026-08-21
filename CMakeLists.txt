cmake_minimum_required (VERSION 3.20)
project ("rezoriri")

set(CMAKE_CXX_STANDARD 20)

# Download dependencies
add_subdirectory ("deps")

# Build something
add_subdirectory ("rezoriri")

# Build player
add_subdirectory ("rezoriri_player")
