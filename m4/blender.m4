AC_DEFUN([AC_CHECK_BLENDER],
[
AC_ARG_WITH([blender],
  [AS_HELP_STRING([--with-blender@<:@=ARG@:>@],
    [use Blender from a standard location (ARG=yes),
     from the specified location (ARG=<path>),
     or disable it (ARG=no)
     @<:@ARG=no@:>@ ])],
    [
    if test "$withval" = "no"; then
        want_blender="no"
    elif test "$withval" = "yes"; then
        want_blender="yes"
        ac_blender_path=""
    else
        want_blender="yes"
        ac_blender_path="$withval"
    fi
    ],
    [want_blender="no"])

if test "x$want_blender" = "xyes" ; then
  if test "$ac_blender_path" != ""; then
    if test -d "$ac_blender_path" && test -x "$ac_blender_path/blender"; then
      AC_MSG_NOTICE(Found Blender in $ac_blender_path)
      BLENDER=$ac_blender_path/blender
    else
      AC_MSG_ERROR(Blender not Found in $ac_blender_path)
    fi
  else
    AC_PATH_PROG(BLENDER, blender, no, [$PATH])
    if test "$BLENDER" = "no" ; then
      AC_MSG_ERROR(Blender not Found)
    fi
  fi
    MIN_BLENDER_VERSION=$1
    AC_MSG_CHECKING(for Blender - version >= $MIN_BLENDER_VERSION)
    BLENDER_VERSION=`$BLENDER --version | awk 'NR > 1{exit};1' | cut -d' ' -f2`
#    blender_major_version=`echo $BLENDER_VERSION | sed 's/^\([[0-9]]*\).*/\1/'`
#    blender_minor_version=`echo $BLENDER_VERSION | sed 's/^\([[0-9]]*\)\.\([[0-9]]*\)\([[0-9]]*\).*/\2\3/'`
    if test "$BLENDER_VERSION" != "$MIN_BLENDER_VERSION" ; then
      AC_MSG_RESULT(no)
      AC_MSG_ERROR(Blender version $MIN_BLENDER_VERSION is required)
    else
      AC_MSG_RESULT(yes)
    fi
  AC_SUBST(BLENDER)
fi
])
