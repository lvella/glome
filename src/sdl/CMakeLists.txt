cmake_minimum_required(VERSION 3.10)

project(glome_sdl)

add_executable(glome
	input.cpp
	input.hpp
	kbinput.cpp
	kbinput.hpp
	jsinput.cpp
	jsinput.hpp
	sdl_main.cpp
)

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

target_include_directories(glome PUBLIC
	${GLEW_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
)

target_link_libraries(glome
	common
    	${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES}
	${SDL2_LIBRARIES}
)
