## Source Code

The main application language is C++. The SDL integration is using the source code that it is 
written in C. For the Android part a little Java stuff is needed. For the Wasm here a small 
javascript and Html are needed.
The main compiler is clang. The exception is the Android target where gradle is used.

## File Structure

### SDL3
Inside the app\jni directory there is the source code of the SDL3 library that is static linked
into the final app. The SDL is 

    app\jni\SDL
    app\jni\SDL_image
    app\jni\SDL_mixer
    app\jni\SDL_ttf
Here we do not edit anything, we compile and link the code as is.

### App source code
The game app is locate inside the directory

    app\jni\src
There some c++ files that are used only on a particular target:

    app\jni\src\Android for the Android target
    app\jni\src\mysys2 for Windows target MySys2
    app\jni\src\UnixSys for Linux Target (WSL2)
    app\jni\src\cmake required cmake template source for a special target (windows rc file)

The Game is developed inside following directories:

    app\jni\src\App
    app\jni\src\CompGfx
    app\jni\src\GameGfx

Since there are two games programmed (Solitaire and Invido) common graphic components are
placed inside the CompGfx subdirectory. The Game Invido is placed inside the GameGfx\Invido
subdir and the solitaire inside the GameGfx\Solitario sub dir.
The App is use to handle the game loop and the initial menu selection like settings,
High Score, Credits and Help.
