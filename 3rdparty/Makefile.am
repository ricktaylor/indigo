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
