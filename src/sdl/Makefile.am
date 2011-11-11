AM_CXXFLAGS = \
	$(SDL_CFLAGS) \
	$(OPENGL_CFLAGS) \
	$(BOOST_CPPFLAGS) 

INCLUDES = \
 	$(SDL_CFLAGS) \
	$(OPENGL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-I$(includedir) \
	-I$(top_srcdir)/common

BOOST_LIBS = -lboost_system -lboost_thread

AM_LDFLAGS = \
	$(SDL_LIBS) \
	$(SDL_IMAGE_LIBS) \
	$(GUICHAN_LIBS) \
	$(OPENGL_LIBS) \
	$(BOOST_LIBS)

bin_PROGRAMS = glome

glome_LDADD = $(top_srcdir)/src/common/libcommon.a

glome_SOURCES = \
	kbinput.cpp \
	net_input.cpp \
	input.cpp \
	jsinput.cpp \
	menu.cpp \
	button.cpp \
	sdl_main.cpp
