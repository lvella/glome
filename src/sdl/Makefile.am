AM_CXXFLAGS = \
	$(SDL_CFLAGS) \
	$(OPENGL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(OPUS_CFLAGS) \
	$(AL_CFLAGS)

INCLUDES = \
	$(SDL_CFLAGS) \
	$(OPENGL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	-I$(includedir) \
	-I$(top_srcdir)/src/common

#BOOST_LIBS = -lboost_system -lboost_thread

bin_PROGRAMS = glome

glome_LDADD = $(top_srcdir)/src/common/libcommon.a \
	$(SDL_LIBS) \
	$(SDL_IMAGE_LIBS) \
	$(GUICHAN_LIBS) \
	$(OPENGL_LIBS) \
	$(BOOST_LDFLAGS) \
	$(OPUS_LIBS) \
	$(AL_LIBS)

glome_SOURCES = \
	input.cpp \
	kbinput.cpp \
	jsinput.cpp \
	sdl_main.cpp

flags:
	@echo $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) $(CXXFLAGS)
