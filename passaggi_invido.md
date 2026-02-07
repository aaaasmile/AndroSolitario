# Invido
Il progetto invido è integrato nei sorgenti che originariamente erano solo per il Solitario.
L'idea è quella di riutilizzare il codice della App centrale, di SDL e di cambiare il codice specifico 
per avere l'invido.

## Compilazione su MySys2 con Antigravity (extension clangd)
Il software usa l'extension clangd che è molto diversa da quella di Visual Studio Code.
I comandi da usare sono:

    rm -r buildinvido
    cmake -S app/jni/ -B buildinvido  -DSDL_WAYLAND=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DINVIDO=ON
    cmake --build buildinvido
    cd buildinvido 
    .\invido.exe
    ninja
Nota che -DCMAKE_EXPORT_COMPILE_COMMANDS=ON è fondamentale per l'estension clangd in quanto non 
usa configurazioni come quelle descritte in c_cpp_properties.json, ma deve essere dentro la directory build.
Con il progetto solitario compilo anche i sorgenti del solitario.