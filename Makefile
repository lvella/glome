all: minimap_proj.glsl.hpp world_proj.glsl.hpp
	g++ *.cpp -o navigna -lGL -lGLU -lGLEW `sdl-config --cflags --libs` -g

minimap_proj.glsl.hpp: minimap_proj.glsl
	xxd -i minimap_proj.glsl > minimap_proj.glsl.hpp

world_proj.glsl.hpp: world_proj.glsl
	xxd -i world_proj.glsl > world_proj.glsl.hpp
