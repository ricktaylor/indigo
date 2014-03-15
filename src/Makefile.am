include $(top_srcdir)/m4/ltrc.inc

AM_CPPFLAGS = \
	-I@top_srcdir@/oobase/include \
	-I@top_builddir@/oobase/include \
	-I@top_srcdir@/3rdparty/glfw/include \
	-I@top_srcdir@/3rdparty/glm
	
AM_CFLAGS = $(PTHREAD_CFLAGS)
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(PTHREAD_LIBS)

if WIN32
AM_CPPFLAGS += -D_WIN32_WINNT=0x0502
endif

bin_PROGRAMS = indigo

bin2c: $(top_srcdir)/tools/bin2c.c Makefile
	$(AM_V_CC)$(CC_FOR_BUILD) $(INCLUDES) $(AM_CFLAGS) -o $@ $(top_srcdir)/tools/bin2c.c
	
%.c: ../resources/%.tga bin2c
	$(AM_V_GEN)./bin2c "$<" "$@" "$*"
	
%.c: ../resources/%.fnt bin2c
	$(AM_V_GEN)./bin2c "$<" "$@" "$*"

indigo_SOURCES = \
	Main.cpp \
	Render.cpp \
	LogicThread.cpp \
	Window.cpp \
	Framebuffer.cpp \
	Viewport.cpp \
	Camera.cpp \
	State.cpp \
	DejaVu_Sans_0.c \
	DejaVu_Sans_1.c \
	DejaVu_Sans.c
		
indigo_LDADD = \
	@top_builddir@/oobase/liboobase.la \
	@top_builddir@/3rdparty/glfw/src/libglfw3.a
	
indigo_LDADD += -lGL

if WIN32
#indigo_SOURCES += Indigo.rc
indigo_LDADD += -lole32 -lrpcrt4
else
indigo_LDADD += -lX11 -lXrandr -lXi -lXxf86vm
endif
