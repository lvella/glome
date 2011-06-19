AM_CXXFLAGS = $(SDL_CFLAGS) $(GLEW_CFLAGS) $(BOOST_CPPFLAGS) 

bin_PROGRAMS = navigna
navigna_LDADD = $(SDL_LIBS) $(SDL_IMAGE_LIBS) $(GUICHAN_LIBS) $(GLEW_LIBS) -lboost_system -lboost_thread
BUILT_SOURCES = minimap_proj.glsl.hpp world_proj.glsl.hpp projectile.glsl.hpp
navigna_SOURCES = 4dmath.cpp meridian.cpp matrix4.cpp projectile.cpp shader.cpp \
	kbinput.cpp object.cpp minimap.cpp mesh.cpp ship.cpp drawable.cpp init_gl.cpp udp_server.cpp \
	client.cpp randomcube.cpp net_input.cpp net_world.cpp split_world.cpp input.cpp jsinput.cpp \
	menu.cpp button.cpp main.cpp
minimap_proj.glsl.hpp: minimap_proj.glsl
	${XXD} -i minimap_proj.glsl > minimap_proj.glsl.hpp

world_proj.glsl.hpp: world_proj.glsl
	${XXD} -i world_proj.glsl > world_proj.glsl.hpp

projectile.glsl.hpp: projectile.glsl
	${XXD} -i projectile.glsl > projectile.glsl.hpp

CLEANFILES = ${BUILT_SOURCES}
