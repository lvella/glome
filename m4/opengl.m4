dnl AX_CHECK_GL([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN([AX_CHECK_GL],
[
AC_CHECK_HEADER(GL/gl.h, have_gl=yes, have_gl=no)
if test $have_gl = yes ; then
  AC_CHECK_LIB(GL, glEnable,, have_gl=no)
fi

if test $have_gl = yes ; then
  ifelse([$1], , :, [$1])
  LIBGL_LIBS="-lGL"
  LIBGL_CFLAGS=""
else
  ifelse([$2], , :, [$2])
  LIBGL_LIBS=""
  LIBGL_CFLAGS=""
fi

AC_SUBST(LIBGL_CFLAGS)
AC_SUBST(LIBGL_LIBS)
])


dnl AX_CHECK_GLU([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN([AX_CHECK_GLU],
[
AC_CHECK_HEADER(GL/glu.h, have_glu=yes, have_glu=no)
if test $have_glu = yes ; then
  AC_CHECK_LIB(GLU, gluSphere,, have_glu=no)
fi

if test $have_glu = yes ; then
  ifelse([$1], , :, [$1])
  LIBGLU_LIBS="-lGLU"
  LIBGLU_CFLAGS=""
else
  ifelse([$2], , :, [$2])
  LIBGLU_LIBS=""
  LIBGLU_CFLAGS=""
fi

AC_SUBST(LIBGLU_CFLAGS)
AC_SUBST(LIBGLU_LIBS)
])


dnl AX_CHECK_GLX([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN([AX_CHECK_GLX],
[
AC_CHECK_HEADER(GL/glx.h, have_glx=yes, have_glx=no)
if test $have_glx = yes ; then
  AC_CHECK_LIB(GL, glXCreateContext,, have_glx=no)
fi

if test $have_glx = yes ; then
  ifelse([$1], , :, [$1])
  LIBGLX_LIBS=""
  LIBGLX_CFLAGS=""
else
  ifelse([$2], , :, [$2])
  LIBGLX_LIBS=""
  LIBGLX_CFLAGS=""
fi

AC_SUBST(LIBGLX_CFLAGS)
AC_SUBST(LIBGLX_LIBS)
])

dnl AX_CHECK_GLEW([ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN([AX_CHECK_GLEW],
[
AC_CHECK_HEADER(GL/glew.h, have_glew=yes, have_glew=no)
if test $have_glew = yes ; then
  AC_CHECK_LIB(GLEW, glewInit,, have_glew=no)
fi

if test $have_glew = yes ; then
  ifelse([$1], , :, [$1])
  LIBGLEW_LIBS="-lGLEW"
  LIBGLEW_CFLAGS=""
else
  ifelse([$2], , :, [$2])
  LIBGLEW_LIBS=""
  LIBGLEW_CFLAGS=""
fi

AC_SUBST(LIBGLEW_CFLAGS)
AC_SUBST(LIBGLEW_LIBS)
])


# AX_CHECK_OPENGL
#
# Checks for mandatory OpenGL support

AC_DEFUN([AX_CHECK_OPENGL],
[
AX_CHECK_GL( [AX_CHECK_GLU(have_opengl=yes) AX_CHECK_GLEW(have_opengl=yes)] )


if test "$have_opengl" = yes ; then
  OPENGL_CFLAGS="$LIBGL_CFLAGS $LIBGLU_CFLAGS $LIBGLEW_CFLAGS"
  OPENGL_LIBS="$LIBGL_LIBS $LIBGLU_LIBS $LIBGLEW_LIBS"

  if test "$no_x" != yes ; then
    AX_CHECK_GLX(, have_opengl=no)
    if test "$have_opengl" = yes ; then
      OPENGL_CFLAGS="$OPENGL_CFLAGS $LIBGLX_CFLAGS"
      OPENGL_LIBS="$OPENGL_LIBS $LIBGLX_LIBS"
    fi
  fi
fi

if test "$have_opengl" != yes ; then
  AC_MSG_ERROR(lacking proper OpenGL support)
fi
AC_SUBST(OPENGL_CFLAGS)
AC_SUBST(OPENGL_LIBS)
])
