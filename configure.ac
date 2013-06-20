AC_PREREQ(2.61)

AC_INIT(indigo,0.0.1)
AC_CONFIG_SRCDIR([README.md])
AC_CONFIG_HEADERS([autoheader_dummy.h])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SUBDIRS([3rdparty/freetype2 oobase ooxml])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([
	Makefile
	3rdparty/Makefile
])

AM_INIT_AUTOMAKE([foreign 1.10 no-define])

# Use the latest short output macro if available
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# I like maintainer mode
AM_MAINTAINER_MODE

AC_CANONICAL_HOST
AC_PROG_CC
AC_PROG_CXX

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

# Set up libtool correctly
m4_ifdef([LT_PREREQ],,[AC_MSG_ERROR([Need libtool version 2.2.6 or later])])
LT_PREREQ([2.2.6])
LT_CONFIG_LTDL_DIR([libltdl])
LT_INIT([dlopen])

AC_OUTPUT
