AC_PREREQ([2.63])

AC_INIT([Navigna], [1.0], [support@fractal.com])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror])

AC_PROG_CXX

#http://sources.redhat.com/automake/automake.html#Extending-aclocal
# Check for OpenGL
AC_CHECK_OPENGL

#check for libglew
PKG_CHECK_MODULES([GLEW],[glew >= 1.5])

# Check for SDL
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AM_PATH_SDL(1.2.14, :, AC_MSG_ERROR(SDL not found. Make sure sdl-config is in your PATH, or specify with --with-sdl-prefix))
AC_LANG_RESTORE

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])
AC_OUTPUT
