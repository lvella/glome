cmake_minimum_required(VERSION 3.10)

project(glome_sdl)

option(FIND_SDL2 "Try to find SDL2" ON)
option(FIND_GLEW "Try to find GLEW" ON)

add_executable(glome
	input.cpp
	input.hpp
	kbinput.cpp
	kbinput.hpp
	jsinput.cpp
	jsinput.hpp
	sdl_main.cpp
)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(glome common ${OPENGL_LIBRARIES})

if(FIND_SDL2)
	find_package(SDL2 REQUIRED)
	target_include_directories(glome PUBLIC ${SDL2_INCLUDE_DIRS})
	target_link_libraries(glome ${SDL2_LIBRARIES})
endif()

if(FIND_GLEW)
	find_package(GLEW REQUIRED)
	target_include_directories(glome PUBLIC	${GLEW_INCLUDE_DIRS})
	target_link_libraries(glome ${GLEW_LIBRARIES})
endif()

separate_arguments(EXTRA_LD_FLAGS)
target_link_libraries(glome ${EXTRA_LD_FLAGS})

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
	target_compile_options(glome PRIVATE -g)
endif()

