all:
	g++ *.cpp -o navigna -lGL -lGLU -lGLEW `sdl-config --cflags --libs` -g