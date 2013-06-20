SUBDIRS = freetype2 .

clean-local:
	@if [ -f @builddir@/glfw/Makefile ]; then make -C @builddir@/glfw clean; fi
	
all-local:
	@mkdir -p @builddir@/glfw
	cd @builddir@/glfw && cmake -G 'Unix Makefiles' \
		-D BUILD_SHARED_LIBS:BOOL=OFF \
		-D GLFW_BUILD_EXAMPLES:BOOL=OFF \
		-D GLFW_BUILD_TESTS:BOOL=OFF \
		@top_srcdir@/3rdparty/glfw
	@make -C @builddir@/glfw all

	
# Custom squirrel makefile
noinst_LTLIBRARIES = libsquirrel.la

libsquirrel_la_CPPFLAGS = -I @srcdir@/SQUIRREL3/include

libsquirrel_la_SOURCES = \
	SQUIRREL3/squirrel/sqapi.cpp \
	SQUIRREL3/squirrel/sqbaselib.cpp \
	SQUIRREL3/squirrel/sqfuncstate.cpp \
	SQUIRREL3/squirrel/sqdebug.cpp \
	SQUIRREL3/squirrel/sqlexer.cpp \
	SQUIRREL3/squirrel/sqobject.cpp \
	SQUIRREL3/squirrel/sqcompiler.cpp \
	SQUIRREL3/squirrel/sqstate.cpp \
	SQUIRREL3/squirrel/sqtable.cpp \
	SQUIRREL3/squirrel/sqmem.cpp \
	SQUIRREL3/squirrel/sqvm.cpp \
	SQUIRREL3/squirrel/sqclass.cpp
