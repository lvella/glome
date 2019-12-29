AM_CPPFLAGS = \
	-I$(top_srcdir)/src/sdl \
	$(SDL_CFLAGS) \
	$(OPENGL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(OPUS_CFLAGS) \
	$(AL_CFLAGS) \
	-I$(includedir)

noinst_LIBRARIES = libcommon.a

libcommon_a_SOURCES = \
	ai_controller.cpp \
	audio.cpp \
	audio_effect.cpp \
	audio_listener.cpp \
	audio_source.cpp \
	audio_world.cpp	\
	camera.cpp \
	color.cpp \
	controller_local.cpp \
	controller_remote.cpp \
	destroyer.cpp \
	drawable.cpp \
	dustfield.cpp \
	fire.cpp \
	game.cpp \
	math.cpp \
	matrix4.cpp \
	meridian.cpp \
	mesh.cpp \
	minimap.cpp \
	object.cpp \
	octree.cpp \
	options.cpp \
	particle_system.cpp \
	projectile.cpp \
	random.cpp \
	randomcube.cpp \
	renderer.cpp \
	shader.cpp \
	ship_controller.cpp \
	ship_stats.cpp \
	ship.cpp \
	simplexnoise1234.cpp \
	spaghetti.cpp \
	supernova.cpp \
	textures.cpp \
	vector4.cpp \
	world_dummy.cpp

flags:
	@echo $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
