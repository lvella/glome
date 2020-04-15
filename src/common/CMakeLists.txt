cmake_minimum_required(VERSION 3.10)

project(glome_common)

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
	camera.cpp
	camera.hpp
	color.cpp
	color.hpp
	controller_local.cpp
	controller_local.hpp
	controller_remote.cpp
	controller_remote.hpp
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
	octree.cpp
	octree.hpp
	options.cpp
	options.hpp
	particle_system.cpp
	particle_system.hpp
	projectile.cpp
	projectile.hpp
	random.cpp
	random.hpp
	randomcube.cpp
	randomcube.hpp
	renderer.cpp
	renderer.hpp
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
	supernova.cpp
	supernova.hpp
	textures.cpp
	textures.hpp
	thread_pool.cpp
	thread_pool.hpp
	vector2.hpp
	vector3.hpp
	vector4.cpp
	vector4.hpp
	world_dummy.cpp
	world_dummy.hpp
	world_local.hpp
)

target_include_directories(common PUBLIC
	"."
	"${glome_SOURCE_DIR}/external/concurrentqueue"
)

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(OPUSFILE REQUIRED IMPORTED_TARGET opusfile)
	pkg_check_modules(OPENAL REQUIRED IMPORTED_TARGET openal)

	target_link_libraries(common PUBLIC
		PkgConfig::OPUSFILE
	        PkgConfig::OPENAL
        )
endif()

find_package(Threads)
target_link_libraries(common PUBLIC ${CMAKE_THREAD_LIBS_INIT})
