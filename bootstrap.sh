#! /bin/sh
echo Running autotools...

mkdir -p m4
aclocal -I m4
autoheader
libtoolize --force --no-warn
automake --foreign --add-missing
autoconf

echo Bootstrapping OOBase...
cd oobase
./bootstrap.sh
cd ..
