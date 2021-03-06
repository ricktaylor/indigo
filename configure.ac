AC_PREREQ(2.61)

AC_INIT(indigo,0.0.1)
AC_CONFIG_SRCDIR([src/Render.cpp])
AC_CONFIG_HEADERS([autoheader_dummy.h src/Config.h])

AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([
	Makefile
])

AM_INIT_AUTOMAKE([foreign 1.10 no-define subdir-objects])

# Use the latest short output macro if available
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# I like maintainer mode
AM_MAINTAINER_MODE

AC_CANONICAL_HOST

# Set the WIN32 conditional in the makefiles if the host is MinGW
# If the host is Cygwin, error
AS_CASE([$host_os],
	[*cygwin*],[AC_MSG_ERROR([Don't build for cygwin, use a native Win32 port such as MinGW])],
	[mingw*],[win32=true]
)
AM_CONDITIONAL([WIN32],[test "x$win32" = "xtrue"])

# Add the --enable-debug arg
AC_ARG_ENABLE([debug],AS_HELP_STRING([--enable-debug],[Turn on debugging]),[debug=true],[debug=false])
AM_CONDITIONAL([DEBUG], [test "x$debug" = "xtrue"])

OO_PROG_CC
OO_PROG_CXX

AX_CC_FOR_BUILD

AS_IF([test -f oobase/include/OOBase/Base.h],,[AC_MSG_ERROR(["No OOBase found, please symlink or copy into $ac_pwd/oobase"])])
AS_IF([test -f oogl/include/OOGL/State.h],,[AC_MSG_ERROR(["No OOGL found, please symlink or copy into $ac_pwd/oogl"])])

# Check the multi-threading flags
AS_CASE([$host_os],
	[mingw*],[PTHREAD_CFLAGS=-mthreads AC_SUBST(PTHREAD_LIBS) AC_SUBST(PTHREAD_CFLAGS)],
	[AX_PTHREAD]
)

# Set sensible default CFLAGS if necessary
AS_IF([test "x$oo_test_CFLAGS" != "xset"],
[
  AS_IF([test "x$debug" = "xtrue"],[
    AX_APPEND_COMPILE_FLAGS([-ggdb -O0],[CFLAGS])
  ],[
    AX_APPEND_COMPILE_FLAGS([-O2],[CFLAGS])
  ])

  AS_CASE([$host_os],
    [mingw*],[AX_APPEND_COMPILE_FLAGS([-march=i686 -mtune=native],[CFLAGS])]
  )
])

# Set sensible default CXXFLAGS if necessary
AS_IF([test "x$oo_test_CXXFLAGS" != "xset"],
[
  AS_IF([test "x$debug" = "xtrue"],[
  	AX_APPEND_COMPILE_FLAGS([-ggdb -O0],[CXXFLAGS])
  ],[
    AX_APPEND_COMPILE_FLAGS([-O2],[CXXFLAGS])
  ])

  AS_CASE([$host_os],
    [mingw*],[AX_APPEND_COMPILE_FLAGS([-march=i686 -mtune=native],[CXXFLAGS])]
  )
])

# Always try to hide symbols
AX_APPEND_COMPILE_FLAGS([-fvisibility=hidden -pipe],[CFLAGS])
AX_APPEND_COMPILE_FLAGS([-fvisibility=hidden -fno-rtti -fno-exceptions -pipe],[CXXFLAGS])

# Always add full warnings
AX_CFLAGS_WARN_ALL
AX_CXXFLAGS_WARN_ALL

# Set up libtool correctly
m4_ifdef([LT_PREREQ],,[AC_MSG_ERROR([Need libtool version 2.2.6 or later])])
LT_PREREQ([2.2.6])
LT_INIT([disable-shared])
LT_CONFIG_LTDL_DIR([libltdl])
LTDL_INIT

AC_OUTPUT
