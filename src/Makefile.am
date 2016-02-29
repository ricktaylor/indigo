include $(top_srcdir)/m4/ltrc.inc

bin_PROGRAMS = indigo

indigo_CPPFLAGS = \
	-I@top_srcdir@/oobase/include \
	-I@top_builddir@/oobase/include \
	-I@top_srcdir@/3rdparty/glfw/include \
	-I@top_srcdir@/3rdparty/glm
	
indigo_CFLAGS = $(PTHREAD_CFLAGS)
indigo_CXXFLAGS = $(PTHREAD_CFLAGS)
indigo_LDFLAGS = $(PTHREAD_LIBS)

if WIN32
indigo_CPPFLAGS += -D_WIN32_WINNT=0x0502
endif

bin2c: $(top_srcdir)/tools/bin2c.c Makefile
	$(AM_V_CC)$(CC_FOR_BUILD) $(INCLUDES) $(AM_CFLAGS) -o $@ $(top_srcdir)/tools/bin2c.c
	
%.png.h: ../resources/%.png bin2c
	$(AM_V_GEN)./bin2c "$<" "$@" "$*"
	
%.fnt.h: ../resources/%.fnt bin2c
	$(AM_V_GEN)./bin2c "$<" "$@" "$*"

%.vert.h: ../resources/%.vert bin2c
	$(AM_V_GEN)./bin2c -t "$<" "$@" "$*_vert"

%.frag.h: ../resources/%.frag bin2c
	$(AM_V_GEN)./bin2c -t "$<" "$@" "$*_frag"
	
%.lua.h: ../resources/%.lua bin2c
	$(AM_V_GEN)./bin2c -t "$<" "$@" "$*_lua"
	
BUILT_SOURCES = \
	Font_8bit.vert.h \
	Font_8bit.frag.h \
	UI9Patch.vert.h \
	UI9Patch.frag.h \
	Titillium-Regular_0.png.h \
	Titillium-Regular.fnt.h \
	menu_border.png.h
	
indigo_SOURCES = \
	../lib/Window.cpp \
	../lib/Framebuffer.cpp \
	../lib/State.cpp \
	../lib/StateFns.cpp \
	../lib/Shader.cpp \
	../lib/Texture.cpp \
	../lib/BufferObject.cpp \
	../lib/VertexArrayObject.cpp \
	Pipe.cpp \
	Thread.cpp \
	StaticResources.cpp \
	ZipResource.cpp \
	Render.cpp \
	Main.cpp \
	Window.cpp \
	Image.cpp \
	Font.cpp \
	UILayer.cpp \
	UIButton.cpp \
	UI9Patch.cpp \
	UIText.cpp \
	App.cpp
	
	
#	GUIStyle.cpp \
#	GUIWidget.cpp \
#	GUIPanel.cpp \
#	GUILayer.cpp \
#	GUILabel.cpp \
#	GUISizer.cpp \
#	GUIGridSizer.cpp \
#	LuaAllocator.cpp \
#	LuaResource.cpp \
#	Splash.cpp
		
indigo_LDADD = \
	@top_builddir@/oobase/liboobase.la \
	@top_builddir@/3rdparty/glfw/src/libglfw3.a
	
#	@top_builddir@/3rdparty/liblua.la 
	
indigo_LDADD += -lGL

if WIN32
indigo_SOURCES += ../resources/indigo.rc
indigo_LDADD += -lole32 -lrpcrt4
else
indigo_LDADD += -lX11 -lXrandr -lXi -lXxf86vm -lXcursor -lXinerama
endif
