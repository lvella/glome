AC_DEFUN([AX_CHECK_BLENDER],
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
    min_blender_major_version=`echo $MIN_BLENDER_VERSION | sed 's/^\([[0-9]]*\).*/\1/'`
    min_blender_middle_version=`echo $MIN_BLENDER_VERSION | sed 's/^\([[0-9]]*\)\.\([[0-9]]\).*/\2/'`
    min_blender_minor_version=`echo $MIN_BLENDER_VERSION | sed 's/^\([[0-9]]*\)\.\([[0-9]]\)\([[0-9]]*\).*/\3/'`

    AC_MSG_CHECKING(for Blender >= $MIN_BLENDER_VERSION)
    BLENDER_VERSION=`$BLENDER --version | awk 'NR > 1{exit};1' | cut -d' ' -f2`
    
    blender_major_version=`echo $BLENDER_VERSION | sed 's/^\([[0-9]]*\).*/\1/'`
    blender_middle_version=`echo $BLENDER_VERSION | sed 's/^\([[0-9]]*\)\.\([[0-9]]\).*/\2/'`
    blender_minor_version=`echo $BLENDER_VERSION | sed 's/^\([[0-9]]*\)\.\([[0-9]]\)\([[0-9]]*\).*/\3/'`

    if test $blender_major_version -gt $min_blender_major_version ; then
      AC_MSG_RESULT(yes)
    else if test '(' $blender_major_version -eq $min_blender_major_version ')' -a '(' $blender_middle_version -gt $min_blender_middle_version ')' ; then
	   AC_MSG_RESULT(yes)
    	 else if test '(' $blender_middle_version -eq $min_blender_middle_version ')' -a '(' $blender_minor_version -ge $min_blender_minor_version ')' ; then
	        AC_MSG_RESULT(yes)
	      else
    	        AC_MSG_RESULT(no)
                AC_MSG_ERROR(Blender version >= $MIN_BLENDER_VERSION is required)
	      fi
         fi
    fi
  AC_SUBST(BLENDER)
fi
  AM_CONDITIONAL([HAVE_BLENDER], [test "x$want_blender" = "xyes"])
])
