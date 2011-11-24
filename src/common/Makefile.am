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
	udp_server.cpp \
	shader.cpp

BUILT_SOURCES = \
	hud.vertex.glsl.hpp \
	minimap.vertex.glsl.hpp \
	minimap.fragment.glsl.hpp \
	world.vertex.glsl.hpp \
	world.fragment.glsl.hpp \
	projectile.vertex.glsl.hpp 

hud.vertex.glsl.hpp: hud.vertex.glsl
	${XXD} -i hud.vertex.glsl > hud.vertex.glsl.hpp

minimap.vertex.glsl.hpp: minimap.vertex.glsl
	${XXD} -i minimap.vertex.glsl > minimap.vertex.glsl.hpp

minimap.fragment.glsl.hpp: minimap.fragment.glsl
	${XXD} -i minimap.fragment.glsl > minimap.fragment.glsl.hpp

world.vertex.glsl.hpp: world.vertex.glsl
	${XXD} -i world.vertex.glsl > world.vertex.glsl.hpp

world.fragment.glsl.hpp: world.fragment.glsl
	${XXD} -i world.fragment.glsl > world.fragment.glsl.hpp

projectile.vertex.glsl.hpp: projectile.vertex.glsl
	${XXD} -i projectile.vertex.glsl > projectile.glsl.vertex.hpp

CLEANFILES = ${BUILT_SOURCES}
