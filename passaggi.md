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
In Admin Powershell uso (non più, vedi sotto)

    usbipd wsl list
    usbipd wsl attach --busid 5-1 --distribution UbuntuMinitoro
    usbipd wsl detach --busid 5-1  (per scollegare)
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

## SDL_Images & Co.
Ho bisogno anche delle altre librerie. 
1) SDL_Image: https://github.com/libsdl-org/SDL_image/releases
2) SDL_mixer: https://github.com/libsdl-org/SDL_mixer/releases
3) SDL_ttf: https://github.com/libsdl-org/SDL_ttf/releases

Per quanto riguarda le librerie esterne (esempio ttf usa freetype e harfbuzz) esse vanno messe
in external. Non si mette la copia della release come  ho fatto per le 4 librerie sdl, ma si segue
su github il link external che è un fork. All'interno del fork (si vede perché c'è il file Android.mk)
si scarica il sorgente attraverso lo zip.

## Assets
Dove vanno messi tutti gli assets?
Secondo questo tizio https://georgik.rocks/sdl2_image-for-android-with-png-image-format/
vanno messi in The location for graphic assets is app/src/main/assets.
Qui ho copiato la directory data del progetto solitario.

## Compile e Link clean
Sono riuscito a compilare e linkare i sorgenti. Ho avuto delle difficoltà con STL in fase di link
che ho risolto usando 

    arguments "APP_STL=c++_static"
nel file build.gradle. Nota come in questo file ogni argomento aggiuntivo viene semplicemente inserito
sotto.
Per compilare la libini a livello statico non è stato affatto semplice. Più che altro è la
creazione del file Andorid.mk che non è così intuitiva partendo dal file CLanguages.txt.
Ho dovuto anche cambiare il nome del modulo in inimod per evitare magie sul nome libini.
Ho incluso solo il file cpp per via del suffix. La direttiva include $(BUILD_STATIC_LIBRARY)
compila la libreria in modo statico. Essa va poi referenziato con LOCAL_STATIC_LIBRARIES := inimod,
mentre tutte le altre librerie di SDL sono qui LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer SDL2_image.

## Adb Device è una lista vuota
Se il comando

    adb devices
fornisce una lista vuota, mentre usbipd wsl attach funziona così come lsusb, allora è meglio
cercare un altra soluzione. 
Quando collego il telefono, oppure faccio un reboot di windows11 con il telefono collegato, se
apro file explorer riesco a vedere RedMiNote 11. In windows Explorer posso poi navigare su tutti i files.
Seguendo questo link https://stackoverflow.com/questions/60166965/adb-device-list-empty-using-wsl2,
in fondo al post viene elencato un modo per far funzionare adb devices su ubuntu-minitoro.
Su telefono ho abilitato il transfer USB dei files, che mi fa comparire il telefono in windows explorer.

Per prima cosa apro una powershell->cmd in D:\Xiaomi\platform-tools_r34.0.5-windows\platform-tools e lancio:

    adb kill-server
    adb -a nodaemon server start
Poi faccio partire una bash di ubuntu-minitoro e qui lancio:

    export WSL_HOST_IP="$(tail -1 /etc/resolv.conf | cut -d' ' -f2)"
    socat TCP-LISTEN:5037,reuseaddr,fork TCP:$WSL_HOST_IP:5037

Ora apro un'altra bash su WSL2 e lancio la sequenza:

    export ANDROID_SDK_ROOT=$HOME/android/
    export PATH=${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin:${ANDROID_SDK_ROOT}/platform-tools:${PATH}
    adb devices
Il risultato è ora:  
List of devices attached
f5c24a47        device
Il tutto senza usare usbipd, che non mi sembra molto stabile, in quanto funziona solo a tratti.
Ora posso usare:

    ./gradlew compileDebugSources
    ./gradlew installDebug
    adb shell am start -n org.libsdl.app/.SDLActivity
Alla fine ho deinstallato usbipd.
Ho creato start_adb_server.bat in D:\Xiaomi\platform-tools_r34.0.5-windows\platform-tools e 
./start_adb_service.sh nel WSL di Minitoro per sveltire un po' la prassi.

## Logs
Ho messo nel progetto il file AndroTrace.cpp che esegue un redirect dei TRACE su
__android_log_print. Poi in un terminal si può usare logcat

    adb logcat -s "SoltarioIta"
che funziona splendidamente. Per usare un file di log, come utilizzo in msys2, ci dovrebbero essere
delle difficoltà nel creare il file di log (vedi https://www.gamedev.net/forums/topic/690641-android-ndk-logging-with-sdl/), anche se non l'ho provato.
