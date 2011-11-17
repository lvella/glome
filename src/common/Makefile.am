INCLUDES = \
	-I$(top_srcdir)/src/sdl \
 	$(SDL_CFLAGS) \
	$(OPENGL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-I$(includedir)

noinst_LIBRARIES = libcommon.a

libcommon_a_SOURCES = \
	math.cpp \
	meridian.cpp \
	matrix4.cpp \
	projectile.cpp \
	shader.cpp \
	object.cpp \
	minimap.cpp \
	mesh.cpp \
	ship.cpp \
	client.cpp \
	randomcube.cpp \
	spaghetti.cpp \
	options.cpp \
	game.cpp \
	vector4.cpp \
	world.cpp \
	split_world.cpp \
	net_world.cpp \
	udp_server.cpp 

BUILT_SOURCES = \
	minimap_proj.glsl.hpp \
	world.vertex.glsl.hpp \
	world.fragment.glsl.hpp \
	projectile.glsl.hpp 

minimap_proj.glsl.hpp: minimap_proj.glsl
	${XXD} -i minimap_proj.glsl > minimap_proj.glsl.hpp

world.vertex.glsl.hpp: world.vertex.glsl
	${XXD} -i world.vertex.glsl > world.vertex.glsl.hpp

world.fragment.glsl.hpp: world.fragment.glsl
	${XXD} -i world.fragment.glsl > world.fragment.glsl.hpp

projectile.glsl.hpp: projectile.glsl
	${XXD} -i projectile.glsl > projectile.glsl.hpp

CLEANFILES = ${BUILT_SOURCES}
