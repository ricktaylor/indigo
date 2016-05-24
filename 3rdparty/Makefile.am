@SET_MAKE@

SUBDIRS = . glfw

# Lua 5.3.1 automake
noinst_LTLIBRARIES = liblua.la

liblua_la_CPPFLAGS = -DLUA_USE_LINUX
liblua_la_CFLAGS = $(PTHREAD_CFLAGS) $(AM_CFLAGS)
liblua_la_LDFLAGS = $(PTHREAD_LIBS)
liblua_la_LIBADD = -lrt -ldl -lm

liblua_la_SOURCES = \
	lua/src/lua.c \
	lua/src/lapi.c \
	lua/src/lcode.c \
	lua/src/lctype.c \
	lua/src/ldebug.c \
	lua/src/ldo.c \
	lua/src/ldump.c \
	lua/src/lfunc.c \
	lua/src/lgc.c \
	lua/src/llex.c \
	lua/src/lmem.c \
	lua/src/lobject.c \
	lua/src/lopcodes.c \
	lua/src/lparser.c \
	lua/src/lstate.c \
	lua/src/lstring.c \
	lua/src/ltable.c \
	lua/src/ltm.c \
	lua/src/lundump.c \
	lua/src/lvm.c \
	lua/src/lzio.c \
	lua/src/lauxlib.c \
	lua/src/lbaselib.c \
	lua/src/lbitlib.c \
	lua/src/lcorolib.c \
	lua/src/ldblib.c \
	lua/src/liolib.c \
	lua/src/lmathlib.c \
	lua/src/loslib.c \
	lua/src/lstrlib.c \
	lua/src/ltablib.c \
	lua/src/lutf8lib.c \
	lua/src/loadlib.c \
	lua/src/linit.c
	
BUILT_SOURCES = glfw/Makefile

glfw/Makefile:
	$(MKDIR_P) glfw
	cd glfw; cmake -G 'Unix Makefiles' \
		-D BUILD_SHARED_LIBS:BOOL=OFF \
		-D GLFW_BUILD_EXAMPLES:BOOL=OFF \
		-D GLFW_BUILD_TESTS:BOOL=OFF \
		@abs_srcdir@/glfw
	
#clean-local:
#	@if [ -f @builddir@/glfw/Makefile ]; then make -C @builddir@/glfw clean; fi
	
#all-local:
	
	