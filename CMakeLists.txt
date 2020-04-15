cmake_minimum_required(VERSION 3.10)

# set the project name
project(glome)

option(STATS_TUNING "Enable ship statistics tunning")
set(DATA_DIR "data" CACHE STRING "Path to data directory")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(src/config.hpp.in src/config.hpp)

add_subdirectory(src/common)
add_subdirectory(src/sdl)

target_include_directories(common PUBLIC
	"${PROJECT_BINARY_DIR}/src"
	"${PROJECT_SOURCE_DIR}/src/sdl"
	$<TARGET_PROPERTY:glome,INCLUDE_DIRECTORIES>
)
