cmake_minimum_required(VERSION 3.10)

project(glome_common)

option(USE_PKG_CONFIG "Try to find pkg-config, which can be used to find OpenAL and Opusfile" ON)
option(FIND_OPENAL "If using pkg-config, try to find OpenAL" ON)
option(FIND_OPUSFILE "If using pkg-config, try to find Opusfile" ON)
option(FIND_OPENVR "If using pkg-config, try to find OpenVR" ON)

add_library(common STATIC
	ai_controller.cpp
	ai_controller.hpp
	audio.cpp
	audio.hpp
	audio_effect.cpp
	audio_effect.hpp
	audio_listener.cpp
	audio_listener.hpp
	audio_sound.hpp
	audio_source.cpp
	audio_source.hpp
	audio_world.cpp
	audio_world.hpp
	buffer_object.hpp
	camera.cpp
	camera.hpp
	collidable.hpp
	color.cpp
	color.hpp
	data_file.cpp
	data_file.hpp
	destroyer.cpp
	destroyer.hpp
	drawable.cpp
	drawable.hpp
	dustfield.cpp
	dustfield.hpp
	fire.cpp
	fire.hpp
	game.cpp
	game.hpp
	initialization.hpp
	initialization.cpp
	make_array.hpp
	math.cpp
	math.hpp
	matrix4.cpp
	matrix4.hpp
	meridian.cpp
	meridian.hpp
	mesh.cpp
	mesh.hpp
	minimap.cpp
	minimap.hpp
	object.cpp
	object.hpp
	object_adder.hpp
	octree.cpp
	octree.hpp
	options.cpp
	options.hpp
	particle_system.cpp
	particle_system.hpp
	profiling.cpp
	profiling.hpp
	projectile.cpp
	projectile.hpp
	qrot.cpp
	qrot.hpp
	random.cpp
	random.hpp
	renderer.cpp
	renderer.hpp
	renderer_vr.cpp
	renderer_vr.hpp
	rot_dir.hpp
	shader.cpp
	shader.hpp
	ship.cpp
	ship.hpp
	ship_controller.cpp
	ship_controller.hpp
	ship_stats.cpp
	ship_stats.hpp
	simplexnoise1234.cpp
	simplexnoise1234.hpp
	spaghetti.cpp
	spaghetti.hpp
	spaghetti_fragment.cpp
	spaghetti_fragment.hpp
	sub_object.cpp
	sub_object.hpp
	supernova.cpp
	supernova.hpp
	textures.cpp
	textures.hpp
	thread_pool.cpp
	thread_pool.hpp
	utils.hpp
	vector2.hpp
	vector3.hpp
	vector4.cpp
	vector4.hpp
	vol_sphere.cpp
	vol_sphere.hpp
	world.cpp
	world.hpp
	world_dummy.cpp
	world_dummy.hpp
	frustum.cpp
	frustum.hpp
)

target_include_directories(common PUBLIC
	"."
	"${glome_SOURCE_DIR}/external/concurrentqueue"
)

if(USE_PKG_CONFIG)
	find_package(PkgConfig REQUIRED)

	if(FIND_OPUSFILE)
		pkg_check_modules(OPUSFILE REQUIRED IMPORTED_TARGET opusfile)
		target_link_libraries(common PUBLIC PkgConfig::OPUSFILE)
	endif()

	if(FIND_OPENAL)
		pkg_check_modules(OPENAL REQUIRED IMPORTED_TARGET openal)
		target_link_libraries(common PUBLIC PkgConfig::OPENAL)
	endif()

	if(FIND_OPENVR)
		pkg_check_modules(OPENVR REQUIRED IMPORTED_TARGET openvr)
		target_link_libraries(common PUBLIC PkgConfig::OPENVR)
		target_include_directories(common PUBLIC PkgConfig::OPENVR)
	endif()
endif()

find_package(Threads)
target_link_libraries(common PUBLIC ${CMAKE_THREAD_LIBS_INIT})
