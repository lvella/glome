all:
	g++ *.cpp -o navigna -lGL -lGLU `sdl-config --cflags --libs` -g