AM_CXXFLAGS = $(SDL_CFLAGS) $(GLEW_CFLAGS)

bin_PROGRAMS = navigna
navigna_LDADD = $(SDL_LIBS) $(GLEW_LIBS)
BUILT_SOURCES = minimap_proj.glsl.hpp world_proj.glsl.hpp
navigna_SOURCES = main.cpp matrix4.cpp projectile.cpp shader.cpp input.cpp object.cpp minimap.cpp ship.cpp

minimap_proj.glsl.hpp: minimap_proj.glsl
	xxd -i minimap_proj.glsl > minimap_proj.glsl.hpp

world_proj.glsl.hpp: world_proj.glsl
	xxd -i world_proj.glsl > world_proj.glsl.hpp
