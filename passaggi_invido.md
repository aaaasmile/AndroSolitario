# Invido
Il progetto invido è integrato nei sorgenti che originariamente erano solo per il Solitario.
L'idea è quella di riutilizzare il codice della App centrale, di SDL e di cambiare il codice specifico 
per avere l'invido.

## Compilazione con target Windows su MySys2 con Antigravity (extension clangd)
Nota: Antigravity non è la mia scelta preferita, Visual Code va bene. Lascio questa sezione solo nel caso fosse necessaria.
Il software usa l'extension clangd che è molto diversa da quella di Visual Studio Code.
I comandi da usare sono:

    rm -r buildinvido
    cmake -S app/jni/ -B buildinvido  -DSDL_WAYLAND=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DINVIDOPRJ=ON -DBUILD_INVIDO_TESTS=ON
    cmake --build buildinvido
    cd buildinvido 
    .\invido.exe
    ninja
Nota che -DCMAKE_EXPORT_COMPILE_COMMANDS=ON è fondamentale per l'estension clangd in quanto non 
usa configurazioni come quelle descritte in c_cpp_properties.json, ma deve essere dentro la directory build.
Con il progetto solitario compilo anche i sorgenti del solitario.

## Compilazione con target Windows (MySys2) con Visual Studio Code (C/C++ extension)
Nota che i comandi vanno messi nel terminal di Visual Code previo il settings del path dove si trova cmake (vedi start_code.ps1)

    rm -r buildinvido
    cmake -S app/jni/ -B buildinvido  -DSDL_WAYLAND=OFF -DINVIDOPRJ=ON -DBUILD_INVIDO_TESTS=ON
    cmake --build buildinvido
    cd buildinvido 
    .\invido.exe
    .\InvidoTest.exe
Quando poi cambi un sorgente usa:

    .\ninja

## Unit test
Le unit test si trovano nella sottodirectory tests. Esse vengono compilati con

    -DBUILD_INVIDO_TESTS=ON
E in buildinvido subdir ci sarà il file InvidoTest.exe.
Nell'esecuzione di InvidoTest.exe al primo errore o assertion error si pianta. 

    .\InvidoTest.exe
Nota che è possibile eseguire il debug di InvidoTest in Visual Code in windows.
Basta fare partire il Debugger su "Windows Debug MySys2 - InvidoTESTS".

## Compilazione con WSL2 target Linux
Nota che con WSL2 non è possibile usare Copilot in agent mode, solo in chat mode.

    rm -r -R buildinvido
    mkdir buildinvido && cd buildinvido
    cmake ../app/jni/  -DSDL_WAYLAND=OFF -DINVIDOPRJ=ON -DBUILD_INVIDO_TESTS=ON
    cmake --build .

## Sorgenti e selezione
L'idea è quella di avere tutti i files compilati (Solitario e Invido) e poi attraverso
la classe GameSelector si esegue lo switch del gioco. Lo switch avviene usando 
GameSettings. 
Però non mi convince. Se compilo il gioco invido, ho il file invido.exe. Per il solitario
solitario.exe. Non sono nomi di programmi che vanno bene per entrambi i giochi con
uno switcher.
Probabilmente è meglio avere dei singoli giochi anche perché nella distribuzione
uso i file singoli (web, itch, android). Una collezione di tutti i giochi si portrebbe 
fare alla fine.
Il menu HighScore in Invido non ha molto senso. Le opzioni sono diverse, nell'invido
ci sono delle opzioni in più.

## TODO
Il testSimpleMatch non funziona (callback in giocata)