#!/bin/bash
rsync -av ./build-web/index.html igor@invido.it:/var/www/invido.it/html/solitario/
rsync -av ./build-web/solitario.* igor@invido.it:/var/www/invido.it/html/solitario/