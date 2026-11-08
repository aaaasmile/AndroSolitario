## TODO - Invido
- [x] Il testSimpleMatch non funziona. Sto aggiustando le varie callbacks a InvidoCore
da parte delle varie classi come Mano e Giocata. Su windows usa Visual Code
e il debug di InvidoTests.
- [x] In AlgPLayer m_vct_Cards_CPU va a zero size. I nomi delle variabili locali in AlgPLayer
non vanno bene. 
- La parte grafica InvidoGfx non è corretta e solo convertita dalla vecchia versione
- OptionsGfx non contiene le specifiche di Invido
- Credits.cpp è il file del solitario non dell'invido e va adattato
- In GameSelector si trova l'help dell'Invido. Però al momento contiene le istruzioni
per il solitario.
- Languages. Nella App iniziale vanno messe solo le stringhe che non sono specifiche di
un gioco. Poi il gioco dell'Invido deve aggiungere le proprie stringhe usando una callback.
- AlgPlayerAuto è "Match stalled: no game action was taken". Metti i traces e 
vedi perché testReplayMatch non va. Non vedo perché testReplayMatch non debba 
ricreare il file json che dovrebbe essere esattamente uguale a quello di partenza.
Si pianta sulla seconda giocata, linea 275 del file json di ref. 
Il replay non funziona perché il file creato dal match è sbagliato. Questo non 
è emerso per via dell'error handling in VadoDentro che non funziona.