## Scale
Un progetto per sperimetare lo scaling del mazzo di carte.

## Build in WSL2
Nella bash 

    rm -r -R build
    cmake -S . -B build -DSDL_WAYLAND=OFF
    cmake --build build
    cd build
    ./scale