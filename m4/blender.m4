AC_DEFUN([AC_CHECK_BLENDER],
[
AC_ARG_WITH([blender],
  [AS_HELP_STRING([--with-blender@<:@=ARG@:>@],
    [use Blender from a standard location (ARG=/usr/local/bin),
     or from the specified location (ARG=<path>),
     @<:@ARG=/usr/local/bin@:>@ ])],
    [
    if test "$withval" = "NONE"; then
        ac_blender_path=""
    else
        ac_blender_path="$withval"
    fi
    ],
    [want_blender="yes"])

if test "$ac_blender_path" != ""; then
	AC_MSG_NOTICE(Found Blender in $ac_blender_path)
	BLENDER=$ac_blender_path/blender
else
	AC_PATH_PROG(BLENDER, blender, , [$PATH])
	AC_MSG_ERROR(Blender not Found)
	#TODO: find correct blender version		
fi
  AC_SUBST(BLENDER)
])
