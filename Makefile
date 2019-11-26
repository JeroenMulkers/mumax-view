BUILDDIR=build
CXX=clang++ 
CXXFLAGS=-std=c++14 -g
LDFLAGS=-lGL -lglfw

EMCXX=em++ 
EMCXXFLAGS=-std=c++14 \
			-s EXPORT_ALL=1 \
			-s FORCE_FILESYSTEM=1 \
			-s EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']\
			-s WASM=1 \
			-s ALLOW_MEMORY_GROWTH=1 \
			-s USE_WEBGL2=1 \
			-s USE_GLFW=3 \
			-I/usr/include/glm/

CPPFILES=main.cpp arrowmodel.cpp
HPPFILES=arrowmodel.hpp camera.hpp field.hpp shaderprogram.hpp shaders.hpp


.PHONY: all
all: desktop web

.PHONY: clean
clean:
		rm -rf ${BUILDDIR}




######## DESKTOP VERSION ########

.PHONY: desktop
desktop: ${BUILDDIR}/desktop/magvis

${BUILDDIR}/desktop/magvis: ${CPPFILES} ${HPPFILES}
		mkdir -p ${BUILDDIR}/desktop
		${CXX} ${CXXFLAGS} ${CPPFILES} -o $@ ${LDFLAGS}




######## WEB VERSION ########

.PHONY: web
web: build/web/index.js build/web/index.html

build/web/index.js: ${CPPFILES} ${HPPFILES}
		mkdir -p build/web
		${EMCXX} ${EMCXXFLAGS} ${CPPFILES} -o build/web/index.js

build/web/index.html: index.html
		mkdir -p build/web
		cp index.html ${@}