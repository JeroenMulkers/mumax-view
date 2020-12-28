mumax-view
==========

[mumax-view](http://mumax.ugent.be/mumax-view) is a web app to visualize OVF files.

Building mumax-view
-------------------

mumax-view is meant to be compiled to web assembly using the emscripten compiler toolchain.

If you have to emscripten compiler toolchain installed, then you can build mumax-view as follows
```sh
git clone --recursice https://github.com/JeroenMulkers/mumax-view
mkdir -p build && cd build
emcmake cmake ..
emmake make
```

Alternatively, you can build mumax-view using the emscripten/emsdk docker image with the following one liner:

```sh
docker run --rm -v $(pwd):/src -u $(id -u):$(id -g) emscripten/emsdk \
  sh -c "mkdir -p build && cd build && emcmake cmake .. && emmake make"
```
