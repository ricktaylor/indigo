@SET_MAKE@

# Lua 5.3.0 automake
noinst_LTLIBRARIES = liblua.la

liblua_la_CPPFLAGS = -DLUA_USE_LINUX
liblua_la_CFLAGS = $(PTHREAD_CFLAGS) $(AM_CFLAGS)
liblua_la_LDFLAGS = $(PTHREAD_LIBS)
liblua_la_LIBADD = -lrt -lreadline -ldl -lm

liblua_la_SOURCES = \
	lua-5.3.0/src/lua.c \
	lua-5.3.0/src/lapi.c \
	lua-5.3.0/src/lcode.c \
	lua-5.3.0/src/lctype.c \
	lua-5.3.0/src/ldebug.c \
	lua-5.3.0/src/ldo.c \
	lua-5.3.0/src/ldump.c \
	lua-5.3.0/src/lfunc.c \
	lua-5.3.0/src/lgc.c \
	lua-5.3.0/src/llex.c \
	lua-5.3.0/src/lmem.c \
	lua-5.3.0/src/lobject.c \
	lua-5.3.0/src/lopcodes.c \
	lua-5.3.0/src/lparser.c \
	lua-5.3.0/src/lstate.c \
	lua-5.3.0/src/lstring.c \
	lua-5.3.0/src/ltable.c \
	lua-5.3.0/src/ltm.c \
	lua-5.3.0/src/lundump.c \
	lua-5.3.0/src/lvm.c \
	lua-5.3.0/src/lzio.c \
	lua-5.3.0/src/lauxlib.c \
	lua-5.3.0/src/lbaselib.c \
	lua-5.3.0/src/lbitlib.c \
	lua-5.3.0/src/lcorolib.c \
	lua-5.3.0/src/ldblib.c \
	lua-5.3.0/src/liolib.c \
	lua-5.3.0/src/lmathlib.c \
	lua-5.3.0/src/loslib.c \
	lua-5.3.0/src/lstrlib.c \
	lua-5.3.0/src/ltablib.c \
	lua-5.3.0/src/lutf8lib.c \
	lua-5.3.0/src/loadlib.c \
	lua-5.3.0/src/linit.c

clean-local:
	@if [ -f @builddir@/glfw/Makefile ]; then make -C @builddir@/glfw clean; fi
	
all-local:
	$(MKDIR_P) @builddir@/glfw
	cd @builddir@/glfw; cmake -G 'Unix Makefiles' \
		-D BUILD_SHARED_LIBS:BOOL=OFF \
		-D GLFW_BUILD_EXAMPLES:BOOL=OFF \
		-D GLFW_BUILD_TESTS:BOOL=OFF \
		@abs_srcdir@/glfw
	$(MAKE) -C @builddir@/glfw all
	