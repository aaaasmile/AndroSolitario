# Solitario per Android

Questo progetto è il tentativo di compilare il progetto https://github.com/aaaasmile/Solitario per la piattaforma Android.
Nota che ho già scritto alcune notte per lo sviluppo nel file https://github.com/aaaasmile/Solitario/blob/main/android.md

## Sviluppo
Lo sviluppo l'ho eseguito su UbuntuMinitoro del mio Mini-k7. Sono partito dal progetto sdl-android-prj-hello
in scratch ed ho poi aggiunto tutti i sorgenti.
Le librerie da compilare sono:
- "SDL2"
- "SDL2_image"
- "SDL2_mixer"
- "SDL2_ttf"

e devono essere presenti nel progetto con tutti i suoi sorgenti.

## Target RedMe Note 11
Il device che utilizzo ha un Cortex-A73 (Snapdragon 680) un arm 64 bit v8.
Il RedMe, per il deployment, deve essere collegato ed accessibile in WSL2 con il cavo USB.
In Admin Powershell uso

    usbipd wsl list
    usbipd wsl attach --busid 5-1 --distribution UbuntuMinitoro
In WSL

    lsusb
    Bus 001 Device 006: ID 2717:ff48 Xiaomi Inc. Mi/Redmi series (MTP + ADB)

Per compilare
Nel terminal di Visual Code:

    export ANDROID_SDK_ROOT=$HOME/android/
	export PATH=${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin:${ANDROID_SDK_ROOT}/platform-tools:${PATH}
	sudo $ANDROID_SDK_ROOT/platform-tools/adb start-server
	adb devices
Qui l'output deve essere qualcosa di simile a questo:

    List of devices attached
    f5c24a47        device

Ora si possono lanciare i comandi di compilazione:

    ./gradlew compileDebugSources
Per installare

    ./gradlew installDebug
Per lanciare l'app dal terminal di Visual Code:

    adb shell am start -n org.libsdl.app/.SDLActivity

## Struttura del progetto
Riferimento: https://wiki.libsdl.org/SDL2/Android#install_sdl_in_a_gcc_toolchain
Ho messo i sorgenti di SDL nella directory app/jni/SDL
Sono partito da SDL-release-2.26.5.tar.gz e ho copiato i files di root 
le directory src e include. Tutte le altre no.
Nel file app/jni/src/Android.mk ho messo tutti i sorgenti che vengono compilati nel progetto.

L'applicazione Android parte dai file in java messi all'interno di src/main/java.
Qui viene instaziata la SDLActivity che al suo interno crea la SDL_Surface dove viene eseguito
il game loop del solitario.