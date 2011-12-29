AM_CXXFLAGS = \
	$(SDL_CFLAGS) \
	$(OPENGL_CFLAGS) \
	$(BOOST_CPPFLAGS) 

INCLUDES = \
	$(SDL_CFLAGS) \
	$(OPENGL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-I$(includedir) \
	-I$(top_srcdir)/src/common

BOOST_LIBS = -lboost_system -lboost_thread

bin_PROGRAMS = glome

glome_LDADD = $(top_srcdir)/src/common/libcommon.a \
	$(SDL_LIBS) \
	$(SDL_IMAGE_LIBS) \
	$(GUICHAN_LIBS) \
	$(OPENGL_LIBS) \
	$(BOOST_LIBS)

glome_SOURCES = \
	input.cpp \
	kbinput.cpp \
	sdl_main.cpp
