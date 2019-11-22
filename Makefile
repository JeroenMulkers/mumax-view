.PHONY: desktop
desktop:
		mkdir -p build/desktop
		clang++ main.cpp -o main -lGL -lglfw
		mv main build/desktop/ovfview

.PHONY: run-desktop
run-desktop: desktop
		./build/desktop/ovfview

.PHONY: web
web:
		mkdir -p build/web
		cp index.html build/web/index.html
		em++ main.cpp -std=c++14 -I/usr/include/glm/ -o build/web/index.js \
			-s EXPORT_ALL=1 \
			-s FORCE_FILESYSTEM=1 \
			-s EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']\
			-s WASM=1 \
			-s ALLOW_MEMORY_GROWTH=1 \
			-s USE_WEBGL2=1 \
			-s USE_GLFW=3 

.PHONY: run-web
run-web: web
		firefox ./build/web/index.html

.PHONY: demo
demo:
		mkdir -p build/demo
		em++ main.cpp -std=c++14 -I/usr/include/glm/ -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s USE_GLFW=3 -o build/demo/index.js

.PHONY: emdebug
emdebug:
		mkdir -p build/emdebug
		em++ main.cpp -std=c++14 -I/usr/include/glm/ -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1 -s USE_GLFW=3 -o build/emdebug/debug.html

.PHONY: clean
clean:
		rm -rf build