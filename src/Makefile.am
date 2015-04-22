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
	
%.c: ../resources/%.png bin2c
	$(AM_V_GEN)./bin2c "$<" "$@" "$*"
	
%.c: ../resources/%.fnt bin2c
	$(AM_V_GEN)./bin2c "$<" "$@" "$*"

indigo_SOURCES = \
	../lib/Window.cpp \
	../lib/Framebuffer.cpp \
	../lib/State.cpp \
	../lib/StateFns.cpp \
	../lib/Shader.cpp \
	../lib/Texture.cpp \
	../lib/BufferObject.cpp \
	../lib/VertexArrayObject.cpp \
	../lib/Image.cpp \
	../lib/Font.cpp \
	Render.cpp \
	LogicThread.cpp \
	Main.cpp \
	Splash.cpp \
	Titillium-Regular_0.c \
	Titillium-Regular.c
		
indigo_LDADD = \
	@top_builddir@/oobase/liboobase.la \
	@top_builddir@/3rdparty/glfw/src/libglfw3.a
	
indigo_LDADD += -lGL

if WIN32
#indigo_SOURCES += Indigo.rc
indigo_LDADD += -lole32 -lrpcrt4
else
indigo_LDADD += -lX11 -lXrandr -lXi -lXxf86vm -lXcursor -lXinerama
endif
