#! /bin/sh

if [ "$USER" = "root" ]; then
	echo "*** You cannot do this as "$USER" please use a normal user account."
	exit 1
fi
if test ! -f configure.ac ; then
	echo "*** Please invoke this script from directory containing configure.ac."
	exit 1
fi

if test ! -d build-aux ; then
	/bin/mkdir ./build-aux
fi

#echo "running libtoolize"
#libtoolize -c -f -i

echo "running aclocal"
aclocal -I ./m4
rc=$?

if test $rc -eq 0; then
	echo "running autoreconf"
	autoreconf -i
	rc=$?
else
	echo "An error occured, autogen.sh stopping."
	exit $rc
fi

if test $rc -eq 0; then
	echo "running automake"
	automake --add-missing
	rc=$?
else
	echo "An error occured, autogen.sh stopping."
	exit $rc
fi

echo "autogen.sh complete"
exit $rc
