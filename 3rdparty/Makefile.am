@SET_MAKE@

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
	