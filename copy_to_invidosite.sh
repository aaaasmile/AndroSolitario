#!/bin/bash
#elimina la vecchia versione 
rm /mnt/d/Projects/go-lang/invido-site/invido-site/static/invido/web/solitario/*.*
# copia la wasm app anche dove viene creato il sito invido.it
rsync -av ./build-webrel/index.html /mnt/d/Projects/go-lang/invido-site/invido-site/static/invido/web/solitario/
rsync -av ./build-webrel/solitario_*.* /mnt/d/Projects/go-lang/invido-site/invido-site/static/invido/web/solitario/