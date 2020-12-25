mumax-view
==========

[mumax-view](http://mumax.ugent.be/mumax-view) is a web app to visualize OVF files.

Building mumax-view
-------------------

Prerequisites: 
- emscripten
- cmake
- make 
- glm headers

Build mumax-view:

```sh
git clone git@github.com:JeroenMulkers/mumax-view.git
cd mumax-view
mkdir -p build
cd build
emcmake cmake ..
emmake make
```
