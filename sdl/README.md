# sdl
Experiments with SDL3 (audio and video), ImGui, ImPlot, etc.

Prerequisites:
 - SDL3
 - CMake

ImGui and ImPlot get downloaded automatically by CMake but may require development headers for X11 if you use Wayland.

To run:
```sh
$ cmake -B bin
$ cmake --build bin -j $(nproc)
$ ./bin/sdl-test
```
