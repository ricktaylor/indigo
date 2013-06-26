include $(top_srcdir)/m4/ltrc.inc

AM_CPPFLAGS = \
	-I@top_srcdir@/oobase/include \
	-I@top_builddir@/oobase/include \
	-I@top_srcdir@/3rdparty/glfw/include
	
AM_CFLAGS = $(PTHREAD_CFLAGS)
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(PTHREAD_LIBS)

if WIN32
AM_CPPFLAGS += -D_WIN32_WINNT=0x0502
endif

bin_PROGRAMS = indigo
		
indigo_LDADD = \
	@top_builddir@/oobase/liboobase.la \
	@top_builddir@/3rdparty/glfw/src/libglfw3.a \
	-lGL

indigo_SOURCES = \
	Main.cpp \
	DrawThread.cpp \
	LogicThread.cpp \
	Queue.cpp

if WIN32
indigo_SOURCES += Indigo.rc
indigo_LDADD += -lole32 -lrpcrt4
else
indigo_LDADD += -lX11 -lXrandr -lXi -lXxf86vm
endif
