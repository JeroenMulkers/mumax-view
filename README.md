ViMag
=====

[ViMag](http://dynamag.ugent.be/vimag) is a web app to visualize OVF files. The app is not yet officially released and is still under heavy development.

Local installation
------------------

Download the source code and enter the directory
```sh
git clone git@github.com:JeroenMulkers/ViMag.git
cd ViMag
```

You can choose if you want to build the desktop version or the web version. Use the following commands to build the desktop version:

```sh
mkdir -p build/desktop
cd build/desktop
cmake ../..
make
```

or the following commands to build the web version

```sh
mkdir -p build/web
cd build/web
emcmake cmake ../..
emmake make
```