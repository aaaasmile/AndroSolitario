# Solitario per Android

Questo progetto è il tentativo di compilare il progetto https://github.com/aaaasmile/Solitario per la piattaforma Android.
Nota che ho già scritto alcune notte per lo sviluppo nel file https://github.com/aaaasmile/Solitario/blob/main/android.md

## Sviluppo
(Vedi sotto per le versioni)
Lo sviluppo l'ho eseguito su UbuntuMinitoro del mio Mini-k7. Sono partito dal progetto sdl-android-prj-hello
in scratch ed ho poi aggiunto tutti i sorgenti.
Le librerie da compilare sono:
- "SDL2" (2.28.5)
- "SDL2_image"
- "SDL2_mixer"
- "SDL2_ttf"

e devono essere presenti nel progetto con tutti i suoi sorgenti. Come IDE uso Visual Code. Ho installato l'SDK di
Android, NDK e gradle (Prerequisiti: openjdk-17-jdk ant android-sdk-platform-tools-common). 
Con sdkmanager a liena di comando ho scaricato:

    sdkmanager "platforms;android-33" "build-tools;33.0.2"
sdkmanager fa parte di commandlinetools-linux-9477386_latest.zip che ho scaricato da https://developer.android.com/studio#downloads dalla sezione _Command line tools only_.
Nota che SDL la compilo in modo statico su tutti i target.
Nota che gradle, che ho aggiornato alla versione 8.1.1 installa tutti gli sdk e ndk
automaticamente senza dover usare sdkmanager. In questo progetto ho aggiornato tutti i tools così
come il progetto Android di default di Sdl che avevo costruito in scrach hellosdl.

## Target RedMe Note 11
Il device che utilizzo ha un Cortex-A73 (Snapdragon 680) un arm 64 bit v8.
Ha una risoluzione di 1080 x 2400
Il RedMe, per il deployment, deve essere collegato ed accessibile in WSL2 con il cavo USB.
In Admin Powershell uso (non più, vedi sotto)

    usbipd wsl list
    usbipd wsl attach --busid 5-1 --distribution UbuntuMinitoro
    usbipd wsl detach --busid 5-1  (per scollegare)
In WSL

    lsusb
    Bus 001 Device 006: ID 2717:ff48 Xiaomi Inc. Mi/Redmi series (MTP + ADB)

Per compilare (meglio vedi l'ultima sezione)
Nel terminal di Visual Code:

    export ANDROID_SDK_ROOT=$HOME/android/
	export PATH=${ANDROID_SDK_ROOT}/cmdline-tools/latest/bin:${ANDROID_SDK_ROOT}/platform-tools:${PATH}
	sudo $ANDROID_SDK_ROOT/platform-tools/adb start-server
	adb devices
Qui l'output deve essere qualcosa di simile a questo (la lista vuota non va):

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
Sono partito da SDL-release-2.26.5.tar.gz (che poi ho aggiornato a 2.28.5, vedi sotto) e ho copiato i files di root 
le directory src e include. Tutte le altre no.
Nel file app/jni/src/Android.mk ho messo tutti i sorgenti che vengono compilati nel progetto.

L'applicazione Android parte dai file in java messi all'interno di src/main/java.
Qui viene instaziata la SDLActivity che al suo interno crea la SDL_Surface dove viene eseguito
il game loop del solitario.

## SDL_Images & Co.
Ho bisogno anche delle altre librerie. 
1) SDL_Image: https://github.com/libsdl-org/SDL_image/releases (SDL2_image-2.6.3)
2) SDL_mixer: https://github.com/libsdl-org/SDL_mixer/releases (SDL2_mixer-2.6.3)
3) SDL_ttf: https://github.com/libsdl-org/SDL_ttf/releases (SDL2_ttf-2.20.2)

Per quanto riguarda le librerie esterne (esempio ttf usa freetype e harfbuzz) esse vanno messe
in external. Non si mette la copia della release come  ho fatto per le 4 librerie sdl, ma si segue
su github il link external che è un fork. All'interno del fork (si vede perché c'è il file Android.mk)
si scarica il sorgente attraverso lo zip.

## SDL Versione Update
Ho fatto un update della versione SDL2 alla versione 2.28.5. Ho scaricato il tar dal sito
https://github.com/libsdl-org/SDL/releases, e poi ho copiato quasi tutte le directory nel mio progetto.
Ho rinominato la vecchia per fare posto alla nuova nella sottodirectory SDL.
Ho anche cambiato il min compileSdkVersion 34 nel file app/build.gradle.
Nota che il nuovo sdk34 viene scaricato automaticamente da gradle.
Però ho poi avuto il problema che Gradle 7.0.3 non è più compatibile. Allora sono passato alla versione
8.1.1 messa nel file build.gradle. Ho poi seguito questo messaggio, che mi è comparso quando ho fatto partire
il compilatore:

    Minimum supported Gradle version is 8.0. Current version is 7.3. If using the gradle wrapper, try editing the distributionUrl in /home/igor/projects/AndroSolitario/gradle/wrapper/gradle-wrapper.properties to gradle-8.0-all.zip
che mi scaricato un nuovo wrapper.
Ho dovuto poi fare un aggiornamento del jdk alla versione 17 ( sudo apt install openjdk-17-jdk openjdk-17-jre).
Dove ho preso il numero 8.1.1? Sono andato a vedere main latest della repository sdl, per vedere il progetto
di Android come è composto. Li ho anche cambiato build.gradle di app/src/main. 
Lì si vede che il namespace è cambiato. 
Ho dovuto spostare la vecchia directory SDL_2_26_5 da jini in quanto veniva compilata lo stesso. L'ho messa in 
D:\tmp\sdl-source\LastUsed_working\SDL_2_26_5 nel caso servisse.
Poi finalmente ho avuto almeno un compile clean.

## Assets
Dove vanno messi tutti gli assets?
Secondo questo tizio https://georgik.rocks/sdl2_image-for-android-with-png-image-format/
vanno messi in The location for graphic assets is app/src/main/assets.
Qui ho copiato la directory data del progetto solitario.
Il problema è che ho creato il progetto su wsl usando il define DATA_PREFIX.
Ora in wsl/windows è la directory Data, su Android è vuota. Per avere questo distinguo
uso il define USE_EMPTY_DATA_PREFIX che è definito nel make file di app/jni/src/Android.mk

## Compile e Link clean
Sono riuscito a compilare e linkare i sorgenti. Ho avuto delle difficoltà con STL in fase di link
che ho risolto usando 

    arguments "APP_STL=c++_static"
nel file build.gradle. Nota come in questo file ogni argomento aggiuntivo viene semplicemente inserito
sotto.
Per compilare la libini a livello statico non è stato affatto semplice. Più che altro è la
creazione del file Andorid.mk che non è così intuitiva partendo dal mio file CLanguages.txt del progetto solitario di mysys2.
Ho dovuto anche cambiare il nome del modulo in inimod per evitare magie sul nome libini (prefisso automatico lib).
Ho incluso solo il file cpp per via del suffix. La direttiva include $(BUILD_STATIC_LIBRARY)
compila la libreria in modo statico. Essa va poi referenziato con LOCAL_STATIC_LIBRARIES := inimod,
mentre tutte le altre librerie di SDL sono qui: LOCAL_SHARED_LIBRARIES := SDL2 SDL2_ttf SDL2_mixer SDL2_image.

### libini e settings files
Alla fine l'ho tolta dal progetto in quanto non mi funzionava su Android (permission denied?).
Ho deciso di usare lo stesso principio di HighScore dove salvo la lista in formato binario usando le
le funzioni di SDL che funzionano anche su Android. In questo modo rinuncio alla possibilità di 
editare facilmente il file usando notepad. 
Dove vanno a finire i files che vengono creati quando cambiano i settings oppure lo High Score?
Su wsl vanno nella home directory nella nuova directory .solitario. Su windows è la directory User.
Su Android è l'internal storage che SDL riesce a cambiare. Però accedere a questi nuovi files,
bisogna utilizzare le funzioni di SDL.

## Adb Device è una lista vuota
Se il comando

    adb devices
fornisce una lista vuota con _usbipd wsl attach_ che funziona così come il comando lsusb, allora è meglio
cercare un altra soluzione. 
Quando collego il teleRedMi, oppure faccio un reboot di windows11 con il RedMi collegato, se
apro file explorer riesco a vedere RedMiNote 11. In windows Explorer posso poi navigare su tutti i files.
Seguendo questo link https://stackoverflow.com/questions/60166965/adb-device-list-empty-using-wsl2,
in fondo al post viene elencato un modo per far funzionare adb devices su ubuntu-minitoro.
Sul RedMe ho abilitato il transfer USB dei files, che mi fa comparire il telefono _RedMe 11_ in windows explorer.

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
Ora posso compilare ed eseguire un deployment (vedi sotto).
Alla fine ho deinstallato usbipd perché proprio non mi ha convinto (quando è attivo non mi funzionano i tools in windows).
Ho creato in D:\Xiaomi\platform-tools_r34.0.5-windows\platform-tools il bat:
    
    start_adb_server.bat 
 Poi su MiniToro nel WSL di Minitoro su ~ lo script:
    
    ./start_adb_service.sh 
per sveltire un po' la prassi iniziale. Sono due finsestre sempre aperte in sottofondo, ma
la comunicazione col device attaccato al cavo USB funziona che è una meraviglia.

## Logs
Far funzionare un progetto come Solitario su Android senza traces è una chimera.
Ho creato nel progetto il file AndroTrace.cpp che esegue un redirect dei TRACE su
__android_log_print. Poi in un terminal si può usare logcat

    adb logcat -s "SoltarioIta"
che funziona splendidamente. Per usare un file di log, come utilizzo in msys2, ci dovrebbero essere
delle difficoltà nel creare il file di log (vedi https://www.gamedev.net/forums/topic/690641-android-ndk-logging-with-sdl/), anche se non l'ho provato.
SDL_Log mi sembra, per ora, superfluo anche perché non so bene che cosa faccia.

## Android.mk
Per capire la sintassi di Andoid.mk ho usato il link ufficiale https://developer.android.com/ndk/guides/android_mk.
Esso serve per compilare i sorgenti c/cpp con ndk. In Visual Code c_cpp_properties.json ho messo il path dove si trovano gli 
include utilizzati da ndk (esempio <android/log.h>).
Per esempio, non so come definire due defines che mi vanno ad influenzare WinTypeGlobal.h e il meccanismo
di Trace. Esse si possono definire con 
    
    LOCAL_CFLAGS += -D<mia def>   
con una sequenza multipla separata da spazi.

## Spegnere lo Smartphone via adb

    adb reboot -p
Che spegne il device istantaneamente.

## Shell sul device

    adb shell
In questo modo si vede che non è possibile accedere a files sul device.

## Profilo e File System
Con adb si possono creare e spostare dei files tra wsl e il device. Per esempio creo un profilo 
del mio package e poi lo scarico. Questa la sequenza:

    adb shell cmd package dump-profiles org.libsdl.app
    adb pull /data/misc/profman/org.libsdl.app-primary.prof.txt

Sul mio device riesco a scrivere sulla directory /sdcard/Documents/ e in linea di principio su /sdcard
Ora non so dove si trova l'eseguibile e neanche se gli assets sono copiati nella directory di deployment.
Non sembra che il comando installDebug trasferisca anche gli assets, perlomeno non nella stessa directory
dove si trova il programma.
Se guardo il contenuto del file generato app-debug.apk si nota questa struttura:

    app-debug
    \assets
        default_options.ini
        \mazzi
        \images
        ...
    \lib
        \arm64-v8a
            libmain.so
            libSDL2.so
            ...
quindi per accedere al file default_options.ini bisogna aggiungere la directory assets.
Il comando copy è qualcosa di simile a:

    Copy file from assets/default_options.ini to /sdcard/Documents/.solitario/options.ini
La funzione SDL_AndroidGetInternalStoragePath mi restituisce /data/data/org.libsdl.app/files/
però non sembra esserci il file default_options.ini.
Le funzioni di SDL vanno bene. Per esempio per creare la directory .solitario uso:

    SDL_AndroidGetInternalStoragePath()
    Created dir /data/data/org.libsdl.app/files/.solitario
    File /data/data/org.libsdl.app/files/.solitario/options.ini created OK
Sembra che per le funzioni che accedono agli assets, vale a dire tutti i files e directories 
della directory assets, sono accessibili dalle funzioni SDL senza alcun prefisso.
Per creare una directory, oppure un file, bisogna invece usare SDL_AndroidGetInternalStoragePath().
Siccome non ho più ini file, non devo copiare nulla. Semplicemente il file viene generato
nella directory di Android quando cambiano i settings oppure high score.

## Editor C++
Uso Visual Code con il plugin c/c++. Esso formatta in modalità Visual studio di Default.
Questo non è il mio modo preferito di formattare il codice, ma uso questa stringa 
nella proprietà: C_Cpp:Clang_format_style

    { BasedOnStyle: Google, UseTab: Never, IndentWidth: 4, TabWidth: 4, AllowShortIfStatementsOnASingleLine: false }

## Problemi del Solitario su Android
Questi sono i problemi e todo della conversione in Android.
SoltarioIta: Fatal: Cannot create dir (null)/.solitario [DONE]
Fatal: loadProfile: Ini file error /data/data/org.libsdl.app/files/.solitario/options.ini
Se vuoi una nuova compilazione clean del target Android, allora basta cancellare la directory app/build
Niente ini.
- Problema con il formato dello schermo
- Non mi funzionano poi i pulsanti
- Nei credits e hight score non si può tornare indietro, qui serve un "back by tap" in High Score
- shuffle_random è marcato obsoleto in ndk, il risultato è che non mescola un bel nulla
- A livello generale direi che il tap non ce l'ho sotto controllo

## Mouse e Touch
Una discussione interessante sull'argomento si trova su 
https://discourse.libsdl.org/t/mouse-emulation-of-touch-events/19255/6
Il fatto è che il touch event viene rimbalzato anche come mouse event. Solo che il rimbalzo
non è assolutamente preciso. Quindi su Android scelgo di ignorare gli eventi del mouse.

## Compilzzione con target wsl
Uso CMake per compilare il target su WSL, mentre ndk con gradle (Android.mk) per quanto
riguarda il target Android.

    mkdir build
    cd build
    cmake ../app/jni/

Problemi con wayland

    dpkg -L libwayland-dev
    #include "primary-selection-unstable-v1-client-protocol.h"
gli header si trovano in /usr/include e nella mia distribuzione non li ho. Ho semplicemente disattivato libwayland
nella lista CMakeList.txt. Le librerie di SDL le ho compilate in modo statico. Per il file
di musica watermusic.it l'ho convertito in watermusic.ogg in quanto la mia libreria statica sdl_mixer non 
supporta il formato it. 
Quando si cambia un file della configuarzione di SDL (il suo CMakeLists.txt) per escludere qualche libreria
che non uso tipo wayland, devo poi ricostruire la directory build.

    cd ..
    rm -r -R build
    mkdir build
    cd build
    cmake ../app/jni/
Nota che il programma va a finire nella directory ./build/src ed lì dove ho poi messo le 
risorse nella directory data. Gli asset li ho anche copiati nella directory asset_data_forwsl.

## Compile, Deploy, Start

    source ./start_env.sh
    ./gradlew compileDebugSources
    ./gradlew installDebug
    adb logcat -s "SoltarioIta"
    adb shell am start -n org.libsdl.app/.SDLActivity
Nota il comando source. Esso viene usato in quanto le variabili definite rimangono definite all'interno
dello script.

