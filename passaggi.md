## SDL 3.0
Ho creato un branch sdl3 e cancellato tutti i sorgenti di SDL*
Poi ho preso i sorgenti di SDL3.0 da https://github.com/libsdl-org/SDL/releases in formato zip
Al momento uso SDL3-3.2.26.
Ho fatto il git clone di AndroSolitario in una nuova directory AndroSolitario3. 
Qui mi manca l'esecuzione di gradle in quanto il programma l'ho già installato (vedi https://github.com/aaaasmile/Solitario/blob/main/android.md alla voce Gradle).
Ho ripreso le due directory con gli assets:

    app/src/main/assets
    asset_data_forwsl
Ho dovuto cancellare gradlew e gradle.bat perché non hanno funzionato dopo il clone.

    gradle wrapper
_Could not create service of type ScriptPluginFactory using BuildScopeServices.createScriptPluginFactory()._

Non so perché gradle e gradlew non funzionano. Ho bisogno sicuramente della directory gradle, che
non è in git. L'ho copiata dalla altra directory e l'ho messa in git. La directory .gradle, invece, può essere tralasciata.

Il comando ./gradlew compileDebugSources mi dice che va tutto bene, ma non crea l'eseguibile. Provo il seguente:

    ./gradlew compileDebugSources
che esegue solo un pre step, ma non chiama ndk. Questo viene eseguito con:

    ./gradlew installDebug
e qui si devono correggere tutti gli errori. Per esempio gli header.
Ho avuto dei problemi con ttf nel generare il file libSDL3_ttf.so. Ho cambiato il file android.mk
includendo i sorgenti mancanti. 
Per quanto riguarda sdl_mixer, sdl_image e sdl_ttf ho preso i sorgenti main latest al 11.11.2024.

## Upgrade di SDL3
Per aggiornare la libreria SDL con una nuova versione (sono passato da una prerelease a SDL3-3.2.26), 
bisogna scaricare il tar da github e
poi sostitutire i files di:

    app/jni/SDL
    app/src/main/java/org/libsdl/app
In SDL non ho messo tutti i files del tar per non appesantire troppo la repository. Nel tar
c'è un progetto demo di Android. In quel folder (per esempio \SDL3-3.2.26\android-project\app\src\main\java\org\libsdl\app) ci sono i files java da sostituire nel 
mio progetto sotto app/src/main/java/org/libsdl/app. Quando sono riuscito a compilare per
Android, nel far partire l'app ho avuto un crash immediato con l'errore:

    JNI DETECTED ERROR IN APPLICATION: JNI FindClass called with pending exception java.lang.NoSuchMethodError: no static or non-static method "Lorg/libsdl/app/SDLActivity;.onNativePen(IIIFFF)V"
Allora ho sostituito i files java con quelli attuali del tar.
L'errore l'ho notato con il log genrico:

    adb logcat

Il problema successivo è che non viene trovato libMain. Questa è una novità di SDL3
e si risolve mettendo nel file main.cpp il seguente header:

    #include <SDL3/SDL.h>
    #include <SDL3/SDL_main.h>
Nota che la versione WSL funziona senza problemi.

Il problema successivo è quello dell'orientamento.
Il problema si risolve impostando per la versione Android lo schermo in questo modo:

    _screenW = 1080;
    _screenH = 1920;
Nota che in WSL lo schermo è troppo grande.

## Per Partire
Nella finestra WSL UbuntuMinitoro vado nella directory ~/projects/AndroSolitario3 e lancio 
    
    code .

## Compilazione con target WSL2
Voglio compilare i sorgenti di AndroSolitario3 anche su WSL, questo per vedere se l'app funziona
prima di tutto su questo target.
Uso CMake per compilare il target su WSL, mentre ndk con gradle (Android.mk) per quanto
riguarda il target Android.

    rm -r -R build
    mkdir build
    cd build
    cmake ../app/jni/ -DSDL_WAYLAND=OFF
    cmake --build .
o alternativo, molto meglio, senza cambiare la directory (-S è la source dir, -B è la build dir):

    rm -r -R build
    cmake -S app/jni/ -B build  -DSDL_WAYLAND=OFF
    cmake --build build
Una volta compilato:

    cd build
    ./solitario

Problemi con la funzione random di std. Se uso

    cmake ../app/jni/ -DSDL_WAYLAND=OFF -DCMAKE_CXX_COMPILER=/usr/bin/g++
    
per avere g++ in modo esplicito non risolve il problema, quindi è meglio lasciarlo fuori
ed usare lo standard c++.

In ogni modo sembra una cascata di problemi in quanto il progetto non è compatibile 
cpn SDL3. Quindi per prima cosa bisogna usare tutte le macro e funzioni di SDL3.
Poi si può vedere di risolvere l'include di random, che nel progetto scratch funziona benissimo con g++ e c++, mentre con gcc non compila.

Problemi con wayland li ho rimossi con -DSDL_WAYLAND=OFF

    dpkg -L libwayland-dev
    #include "primary-selection-unstable-v1-client-protocol.h"
gli header si trovano in /usr/include e nella mia distribuzione non li ho. Ho semplicemente disattivato libwayland
nella lista CMakeList.txt. 

Le librerie di SDL le ho compilate in modo statico. Per il file
di musica watermusic.it l'ho convertito in watermusic.ogg in quanto la mia libreria statica sdl_mixer non 
supporta il formato it. 
Quando si cambia un file della configuarzione di SDL (il suo CMakeLists.txt) per escludere qualche libreria
che non uso tipo wayland, devo poi ricostruire la directory build.

Nota che il programma va a finire di dafult nella directory ./build/src. 
Cambio la destinazione con set_target_properties in CMakeFiles.
Gli asset li ho copiati con un post build command.

## Compilazione con target Windows (MySys2)

    rm -r build
    cmake -S app/jni/ -B build  -DSDL_WAYLAND=OFF
    cmake --build build

Per far partire il Solitario occorre un device attaccato, altrimenti non parte.
Siccome ho un'altra versione installata, quella col setup 2_0_1, ho bisogno di salvare
i files in un'altra directory (.solitario + VERSION_HOME). 
Asset e dlls non sono su github, le ho copiate nella dir root del progetto. 
Le librerie di SDL vengono compilate nel progetto, ma non sono
statiche bensì dll e vanno copiate alla fine della compilazione. Le dll mancanti, invece, vanno prese 
da C:\msys64\ucrt64\bin.
Per far partire il programma:

    .\build\solitario.exe
I vari files, compresi i logs, vanno a finire in C:\Users\igor\.solitario030002

### Problemi in Windows
 - Quando scopre una carta sulla pila impiega troppo tempo per scoprirla
 - L'icona dell'asso di bastoni non è trasparaente
 - L'animazione finale della vittoria è troppo veloce


## Directory Scratch
Voglio creare dei piccoli progetti per testare delle funzionalità singole.
Per esempio cd scratch/test_font vorrei compilare il programma hello.c.
A questo scopo ho creato un nuovo file CMakeList.txt che mi compila sdl e il file.

    rm -r -R build
    cmake -S . -B build -DSDL_WAYLAND=OFF
    cmake --build build
    cd build
    ./hello

## Assets
Gli Asset in Android sono piazzati in app/src/main.
Gli asset in WSL2 vanno piazzati in una directory data di build/bin, che è la directory dove
viene creato il solitario. Per questo task uso un custom command in POST_BUILD definito nel 
file CMakeList.

## Debugger WSL
In Visual Code seezionare "Debug WSL", settare un break point e far partire il debugger
di Visual Code. Funziona immediatamente senza problemi.

# Versione Per SDL 2.0
Qui di seguito ho messo tutte le istruzioni che ho usato per compilare la versione Android con 
SDL2.0.

## Solitario per Android

AndroSolitario è il tentativo di compilare il progetto https://github.com/aaaasmile/Solitario per la piattaforma Android.
Nota che ho già scritto alcune notte per lo sviluppo nel file https://github.com/aaaasmile/Solitario/blob/main/android.md
In questo progetto ho compilato i sorgenti sia per Android che per WSL2.

## Info per partire con lo sviluppo
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
Per il collegamento vedi la sezione Adb Devices

## Target Nothing Phone 3a
Ha lo stesso setup del RedMe. Ho fatto un upgrade della platform e gradle semplicememente
cambiando il file build.gradle. Quando ho lanciato

    ./gradlew compileDebugSources
mi ha aggiornato la platform e il compiler ndk in modo automatico.
Per eseguire il debug su Nothing Phone basta andare nel menu Settings > About phone > Software info > Build number
E ripetere 7 volte il tap sul build number. Poi nel settings si abilita Usb debugging.
A questo punto adb devices riconosce il nuovo device e gradle può installare la versione debug
sul telefono.
Su questo telefono ho avuto il problema che non partiva la app, al contrario di RedMe. Il problema
era il file di settings.bin che ha un nuovo formato. L'ho risolto evitando di uscire dall'app se
i settings non vengono caricati.

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
Ho bisogno di SDl e anche di altre librerie. 
1) SDL_Image: https://github.com/libsdl-org/SDL_image/releases (SDL2_image-2.6.3)
2) SDL_mixer: https://github.com/libsdl-org/SDL_mixer/releases (SDL2_mixer-2.6.3)
3) SDL_ttf: https://github.com/libsdl-org/SDL_ttf/releases (SDL2_ttf-2.20.2)

Per quanto riguarda le librerie esterne (esempio ttf usa freetype e harfbuzz) esse vanno messe
in external (vedi app/jni/SDL_image/Android.mk). Per esempio in app/jni/SDL_ttf/external ho copiato freetype e harfbuzz dallo zip scaricato da Github.
Queste librerie esterne, vanno prese dal fork di github. Per esempio png. Dalla repository di SDL_Image si segue il link external di libpng,
il quale rimanda alla repository  https://github.com/libsdl-org/libpng/tree/c22c2de876e0c2de7a62c6454bd6ee09ddab5571. Qui scarico lo zip del fork e lo copio nella directory external, nel caso mi servissero. Per la libreria SDL_mixer e SDL_Image non ho copiato nessuna
libreria esterna, solo per SDL_ttf. Per il formato musicale It, magari ci vorrebbe
qualche libreria esterna, ma non so quale. Oppure è in formato midi e basta abilitare timidity in 
Android.mk attraverso SUPPORT_MID_TIMIDITY nel progetto SDL_Mixer.

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
Più che altro è la creazione del file Andorid.mk che non è così intuitiva partendo dal mio file CLanguages.txt del progetto solitario di mysys2.
Ho tolto libini.
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

## Collegare il dispositivo: Adb Devices 
Nota che sviluppo in Visual Code su WSL2 remoto, ma i comandi adb vanno fatti partire
da powershell -> cmd in windows.
Se il comando (powershell in D:\Xiaomi\platform-tools_r34.0.5-windows\platform-tools windows - > cmd ->)

    adb devices
fornisce una lista vuota con _usbipd wsl attach_ che funziona così come il comando lsusb, allora è meglio
cercare un altra soluzione. 
Quando collego il telefono RedMi, oppure faccio un reboot di windows11 con il RedMi collegato, se
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

In powershell -> cmd, ma anche in WSL2

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
SDL_Log lo sto utilizzando nella versione WSL con un redirect della macro TRACE impostato sulle
funzioni definite in  UnixsysTrace.cpp.

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

### Ricerca all'interno del progetto
Per evitare di cercare ovunque, posso limitare la ricerca all'interno solo dei miei sorgenti con
questo _files to include_

    app/jni/src/**/

## Problemi del Solitario su Android
Questi sono i todo della conversione in Android.

- Gli spin delle opzioni sono troppo piccoli (col finger down va bene) [DONE]
- Le carte dei mazzi piacentina son troppo piccole
- Quando finisce una partita, il nome del record è vuoto (data e ora vanno bene) [DONE]
- GoTo Invido non funziona nella label gfx
- Sound On/Off meglio un icona [DONE]
- Doppio tap dovrebbe avere lo stesso effetto del tasto destro del mouse [DONE]
- Problema con il formato dello schermo: il tavolo di gioco è troppo piccolo [DONE]
- Nel tavolo da gioco ci vorrebbe un bottone per mettere la musica in pausa.[DONE]
- Il programma sul RedMe si chiama Game invece di Solitario. (si cambia strings.xml) [DONE]
- SoltarioIta: Fatal: Cannot create dir (null)/.solitario [DONE]
- Fatal: loadProfile: Ini file error /data/data/org.libsdl.app/files/.solitario/options.ini
Qui ho rimosso i file ini della libreria ini. Uso un formato binario con SDL. [DONE]
- Non mi funzionano poi i pulsanti [DONE]
- Nei credits e hight score non si può tornare indietro, qui serve un "back by tap" in High Score [DONE]
- shuffle_random è marcato obsoleto in ndk, il risultato è che non mescola un bel nulla [DONE]
- A livello generale direi che il tap non ce l'ho sotto controllo [DONE]
- Il menu Esci non mi fa funzionare il suono quando poi riavvio il programma. Questo è dovuto al fatto che le App in Android non hanno il comando Quit. Quando uso lo Swap, il programma termina in modo corretto e anche riparte in modo corretto. Per questo ho rimosso il comando Esci [DONE]
- Rimosso il menu Guida in quanto non riesco a far partire la visualizzazione del pdf [DONE]

## Problemi SDL3 in Android
- Il doppio tab sugli assi non mi manda la carta sulla pila in automatico.


## Problemi SDL3 in WSL
- Schermata del dialogo Opzioni è completamente traspararente anziché verde. [DONE]
- Schermata iniziale con i colori a palette fredda. (problema con create surface mask) [DONE]
- Shuffle mazzo non va (spostato la dichiarazione di include sotto quella di vector) [DONE]
- Nel gioco il tasto escape non interrompe la musica
- Nel gioco il bottone della musica non va bene nella posizione
- Il font della message box è troppo piccolo
- Il nome del giocatore deve essere cambiato con le opzioni. Come default può andare bene usare env, 
ma non va settato nella funzione save high score.
- Il bottone dello stop della musica va messo come icona.

## Mouse e Touch
Una discussione interessante sull'argomento si trova su 
https://discourse.libsdl.org/t/mouse-emulation-of-touch-events/19255/6
Il fatto è che il touch event viene rimbalzato anche come mouse event. Solo che il rimbalzo
non è assolutamente preciso. Quindi su Android scelgo di ignorare gli eventi del mouse.


## SDL Activity log
Se ci sono dei problemi nello startup oppure nella chiusura della App (per esempio mancano dei delete)
si possono vedere i traces di SDL prima e dopo la chiamata di main() del mio programma con la seguente:

    adb logcat -s "SDL"

## Compile, Deploy e Start App
Per prima cosa collego il device e lancio in windows D:\Xiaomi\platform-tools_r34.0.5-windows\platform-tools\start_adb_server.bat
Su WSL lancio ./start_adb_service.sh che è un terminal senza output, se tutto procede bene.
in ~/projects/AndroSolitario lancio 
    
    code .
All'interno del terminal di Visual Code posso compilare, installare e lanciare il sfotware:

    source start_env.sh
    ./gradlew compileDebugSources
    ./gradlew installDebug
    adb logcat -s "SoltarioIta"
    adb shell am start -n org.libsdl.app/.SDLActivity
Nota il comando source. Esso viene usato in quanto le variabili definite rimangono definite all'interno
dello script.
