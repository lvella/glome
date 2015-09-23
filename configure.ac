AC_PREREQ([2.60])

AC_INIT([Glome], [0.1], [support@fractal.com])

AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_HEADERS([src/config.hpp])
DATA_DIR=`${PWDCMD-pwd}`/data
AC_DEFINE_UNQUOTED([DATA_DIR], ["$DATA_DIR"] , [Data directory])

m4_pattern_allow([^AX_])
AC_CONFIG_MACRO_DIR([m4])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE

AC_ARG_PROGRAM
AC_PROG_CXX
AC_PROG_RANLIB

# Packages configuration
AC_PATH_PROG(PKG_CONFIG, pkg-config)

#http://www.gnu.org/software/autoconf-archive/ax_prog_doxygen.html
m4_include([m4/ax_prog_doxygen.m4])
DX_DOXYGEN_FEATURE(ON)
DX_DOT_FEATURE(ON)
DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(OFF)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(OFF)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(Navigna, Doxyfile.cfg, [docs])

m4_include([m4/ax_boost_base.m4])
AX_BOOST_BASE([1.42],,AC_MSG_FAILURE())

#http://sources.redhat.com/automake/automake.html#Extending-aclocal
# Check for OpenGL
AX_CHECK_OPENGL
#check for libglew
#PKG_CHECK_MODULES([GLEW],[glew >= 1.5])

# Check for opusfile decoding library
PKG_CHECK_MODULES([OPUS], [opusfile])

# Check for OpenAL
PKG_CHECK_MODULES([AL], [openal])

# Check for SDL
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AM_PATH_SDL2(2.0.2, :, AC_MSG_ERROR(SDL not found. Make sure sdl-config is in your PATH, or specify with --with-sdl-prefix))
AC_LANG_RESTORE

# Check for Blender
AX_CHECK_BLENDER([2.58])

CXXFLAGS+=" -std=c++11 -pthread"

AC_CONFIG_FILES([
	Makefile
	data/models/Makefile
	src/Makefile
	src/common/Makefile
	src/sdl/Makefile
])
AC_OUTPUT

ALL_FLAGS="${CXXFLAGS} ${OPENGL_CFLAGS} ${SDL_CFLAGS} ${AL_CFLAGS} ${OPUS_CFLAGS} ${BOOST_CPPFLAGS} -I${SRC_COMMON} -I${SRC_SDL}"
AC_SUBST(ALL_FLAGS)
AC_SUBST([OPUS_CFLAGS])
AC_SUBST([OPUS_LIBS])
AC_SUBST([AL_CFLAGS])
AC_SUBST([AL_LIBS])

echo ["\
-------------------------------------------------

 ${PACKAGE_NAME} Version ${PACKAGE_VERSION}

 Prefix: '${prefix}'

 Build system: $build
 Host system: $host
 Target system: $target

 Compiler: '${CXX} ${CXXFLAGS} ${CPPFLAGS} ${OPENGL_CFLAGS} ${SDL_CFLAGS} ${BOOST_CPPFLAGS}'
 Libraries: '${SDL_LIBS} ${OPENGL_LIBS} ${BOOST_LDFLAGS}'
"]
if test "$BLENDER" != ""; then
echo ["
 Blender : '${BLENDER}'
"]
fi
echo ["\
--------------------------------------------------
"]
