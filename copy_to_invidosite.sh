#!/bin/bash
# copia la wasm app anche dove viene creato il sito invido.it
rsync -av ./build-web/index.html /mnt/d/Projects/go-lang/invido-site/invido-site/static/invido/web/solitario/
rsync -av ./build-web/solitario_*.* /mnt/d/Projects/go-lang/invido-site/invido-site/static/invido/web/solitario/