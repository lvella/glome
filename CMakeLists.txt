cmake_minimum_required(VERSION 3.10)

# set the project name
project(glome)

# options
option(STATS_TUNING "Enable ship statistics tunning")
set(DATA_DIR "data" CACHE STRING "Path to data directory")
set(EXTRA_CXX_FLAGS "" CACHE STRING "Extra C++ flags used during compilation")
set(EXTRA_LD_FLAGS "" CACHE STRING "Extra flags used during linking")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

list(INSERT CMAKE_CXX_FLAGS 0 "${EXTRA_CXX_FLAGS}")

configure_file(src/config.hpp.in src/config.hpp)

add_subdirectory(src/common)
add_subdirectory(src/sdl)

include(CheckIPOSupported)
check_ipo_supported(RESULT HAS_IPO)
if(HAS_IPO)
  set_property(TARGET common PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  set_property(TARGET glome PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

target_include_directories(common PUBLIC
	"${PROJECT_BINARY_DIR}/src"
	"${PROJECT_SOURCE_DIR}/src/sdl"
	$<TARGET_PROPERTY:glome,INCLUDE_DIRECTORIES>
)
