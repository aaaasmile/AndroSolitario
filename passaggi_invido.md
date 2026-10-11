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

## Compilazione con WSL2 target Linux
Nota che con WSL2 non è possibile usare Copilot in agent mode, solo in chat mode.

    rm -r -R buildinvido
    mkdir buildinvido && cd buildinvido
    cmake ../app/jni/  -DSDL_WAYLAND=OFF -DINVIDOPRJ=ON
    cmake --build .