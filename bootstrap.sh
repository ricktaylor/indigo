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

echo Bootstrapping OOXML...
cd ooxml
./bootstrap.sh
cd ..

echo Bootstrapping 3rdparty/freetype2...
cd 3rdparty/freetype2
./autogen.sh
cd ../..
