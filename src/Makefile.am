AM_CXXFLAGS = $(SDL_CFLAGS) $(GLEW_CFLAGS) $(BOOST_CPPFLAGS) 

bin_PROGRAMS = navigna
navigna_LDADD = $(SDL_LIBS) $(GLEW_LIBS) -lboost_system -lboost_thread
BUILT_SOURCES = minimap_proj.glsl.hpp world_proj.glsl.hpp projectile.glsl.hpp
navigna_SOURCES = 4dmath.cpp main.cpp meridian.cpp matrix4.cpp projectile.cpp shader.cpp \
	kbinput.cpp object.cpp minimap.cpp ship.cpp drawable.cpp init_gl.cpp udp_server.cpp \
	client.cpp randomcube.cpp world.cpp

minimap_proj.glsl.hpp: minimap_proj.glsl
	xxd -i minimap_proj.glsl > minimap_proj.glsl.hpp

world_proj.glsl.hpp: world_proj.glsl
	xxd -i world_proj.glsl > world_proj.glsl.hpp

projectile.glsl.hpp: projectile.glsl
	xxd -i projectile.glsl > projectile.glsl.hpp

CLEANFILES = ${BUILT_SOURCES}
