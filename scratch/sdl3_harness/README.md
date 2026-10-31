# SDL3 C++ Harness

A minimal desktop SDL3 application for experimenting with game code. It builds
against the SDL3 source already included in `app/jni/SDL`, independently of the
Android app. The sample opens a window, animates a rectangle, and exits when the
window is closed or Escape is pressed.

From the repository root, configure and build with CMake:

```powershell
cmake -S scratch/sdl3_harness -B scratch/sdl3_harness/build
cmake --build scratch/sdl3_harness/build --config Debug
```

Run the generated `sdl3_harness` executable from the build directory (for
Visual Studio generators it is under `Debug`). Add your game sources to the
executable in `CMakeLists.txt` and start developing in `main.cpp`.
