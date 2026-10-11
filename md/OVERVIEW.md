## Source Code

The main application language is C++. The SDL integration uses source code written in C. For the Android part, a little Java is needed. For WebAssembly, a small amount of JavaScript and HTML is needed.
The main compiler is Clang. The exception is the Android target, where Gradle is used.

## File Structure

### SDL3
Inside the app\jni directory, there is the source code of the SDL3 library that is statically linked
into the final app. The SDL folders are:

    app\jni\SDL
    app\jni\SDL_image
    app\jni\SDL_mixer
    app\jni\SDL_ttf
Here we do not edit anything; we compile and link the code as is.

### App source code
The game app is located inside the directory

    app\jni\src
There are some C++ files that are used only on a particular target:

    app\jni\src\Android for the Android target
    app\jni\src\mysys2 for the Windows target (MSYS2)
    app\jni\src\UnixSys for the Linux target (WSL2)
    app\jni\src\cmake required CMake template source for a special target (Windows RC file)

The game is developed inside the following directories:

    app\jni\src\App
    app\jni\src\CompGfx
    app\jni\src\GameGfx

Since there are two games programmed (Solitaire and Invido), common graphic components are placed
inside the CompGfx subdirectory. The game Invido is placed inside the GameGfx\Invido subdirectory,
and Solitaire is placed inside the GameGfx\Solitario subdirectory.
The app is used to handle the game loop and the initial menu selection, including settings,
High Score, Credits, and Help.
