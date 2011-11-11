INCLUDES = \
	-I$(top_srcdir)/sdl \
 	$(SDL_CFLAGS) \
	$(OPENGL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-I$(includedir)

noinst_LIBRARIES = libcommon.a

libcommon_a_SOURCES = \
	4dmath.cpp \
	meridian.cpp \
	matrix4.cpp \
	projectile.cpp \
	shader.cpp \
	object.cpp \
	minimap.cpp \
	mesh.cpp \
	ship.cpp \
	asteroid.cpp \
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
	world_proj.glsl.hpp \
	projectile.glsl.hpp \
	asteroid-vert.glsl.hpp \
	asteroid-frag.glsl.hpp

minimap_proj.glsl.hpp: minimap_proj.glsl
	${XXD} -i minimap_proj.glsl > minimap_proj.glsl.hpp

world_proj.glsl.hpp: world_proj.glsl
	${XXD} -i world_proj.glsl > world_proj.glsl.hpp

projectile.glsl.hpp: projectile.glsl
	${XXD} -i projectile.glsl > projectile.glsl.hpp

asteroid-vert.glsl.hpp: asteroid-vert.glsl
	${XXD} -i asteroid-vert.glsl > asteroid-vert.glsl.hpp

asteroid-frag.glsl.hpp: asteroid-frag.glsl
	${XXD} -i asteroid-frag.glsl > asteroid-frag.glsl.hpp

CLEANFILES = ${BUILT_SOURCES}
