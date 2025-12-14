#include "Languages.h"

Languages::Languages() {
    for (int i = 0; i < TOT_STRINGS; i++) {
        for (int j = 0; j < TOT_LANG; j++) {
            _mtxLangString[j][i] = "No Translation!";
        }
    }
    _eLangid = LANG_ITA;
    // dialetto
    _mtxLangString[LANG_DIAL_BREDA][ID_START] = "Cümincia";
    _mtxLangString[LANG_DIAL_BREDA][ID_LANGUAGESEL] = "Lengui furesti";
    _mtxLangString[LANG_DIAL_BREDA][ID_HIGHSCORE] = "Al mèi di mèi";
    _mtxLangString[LANG_DIAL_BREDA][ID_VIDEOSET] = "Giosta al display";
    _mtxLangString[LANG_DIAL_BREDA][ID_MAINMENU] = "Menu principal";
    _mtxLangString[LANG_DIAL_BREDA][ID_EXIT] = "Fnomla che";
    _mtxLangString[LANG_DIAL_BREDA][ID_CHOOSELEVEL] = "Cata fòra al jeuq";
    _mtxLangString[LANG_DIAL_BREDA][ID_BACKMAINMENU] = "Al menu principal";
    _mtxLangString[LANG_DIAL_BREDA][ID_EASY] = "Facil";
    _mtxLangString[LANG_DIAL_BREDA][ID_HARD] = "Dificilisim";
    _mtxLangString[LANG_DIAL_BREDA][ID_HARDER] = "Mia tant facil";
    _mtxLangString[LANG_DIAL_BREDA][ID_BACK] = "Turna indrè";
    _mtxLangString[LANG_DIAL_BREDA][ID_SELECTDISPLAYSETTINGS] =
        "Cata föra al display";
    _mtxLangString[LANG_DIAL_BREDA][ID_INVIDO] = "Invidu";
    _mtxLangString[LANG_DIAL_BREDA][ID_CHOOSELANGUA] = "Cata föra la lengua";
    _mtxLangString[LANG_DIAL_BREDA][ID_ENGLISH] = "L'ingles";
    _mtxLangString[LANG_DIAL_BREDA][ID_DIALETMN] = "Al dialet";
    _mtxLangString[LANG_DIAL_BREDA][ID_ITALIANO] = "L'italian";
    _mtxLangString[LANG_DIAL_BREDA][ID_ANONIM] = "Anonim";
    _mtxLangString[LANG_DIAL_BREDA][ID_PUSHBUTTON] = "Schesa an buton";
    _mtxLangString[LANG_DIAL_BREDA][ID_CMD_UNKNOWN] = "Cumand mia cunusi";
    _mtxLangString[LANG_DIAL_BREDA][ID_CMD_HELP] = "Cumand cat po catar fora:";
    _mtxLangString[LANG_DIAL_BREDA][ID_CMD_CHATWELCOME] =
        "Benarriva' in dla chat";
    _mtxLangString[LANG_DIAL_BREDA][ID_CMD_CHATLIST] =
        "La lista adla gent in dla chat:";
    _mtxLangString[LANG_DIAL_BREDA][ID_SOLITARIO] = "Sulitari";
    _mtxLangString[LANG_DIAL_BREDA][ID_DECKNAME] = "Mas ad li carti";
    _mtxLangString[LANG_DIAL_BREDA][ID_SOUNDOPT] = "Müsica";
    _mtxLangString[LANG_DIAL_BREDA][ID_ABILITATO] = "Impisada";
    _mtxLangString[LANG_DIAL_BREDA][ID_DISABILITATO] = "Smorsa";

    _mtxLangString[LANG_DIAL_BREDA][ID_CHOOSEDECK] =
        "Cata föra al mas ad lì carti";
    _mtxLangString[LANG_DIAL_BREDA][ID_PIACENTINA] = "Piacentini";
    _mtxLangString[LANG_DIAL_BREDA][ID_BERGAMO] = "Bergum";
    _mtxLangString[LANG_DIAL_BREDA][ID_BOLOGNA] = "Bulugnesi";
    _mtxLangString[LANG_DIAL_BREDA][ID_GENOVA] = "Genuesi";
    _mtxLangString[LANG_DIAL_BREDA][ID_MILANO] = "Milanesi";
    _mtxLangString[LANG_DIAL_BREDA][ID_NAPOLI] = "Napuletani";
    _mtxLangString[LANG_DIAL_BREDA][ID_PIEMONTE] = "Piemuntesi";
    _mtxLangString[LANG_DIAL_BREDA][ID_ROMAGNA] = "Rumagnoli";
    _mtxLangString[LANG_DIAL_BREDA][ID_SARDEGNA] = "Sardagnöli";
    _mtxLangString[LANG_DIAL_BREDA][ID_TOSCANA] = "Tuscani";
    _mtxLangString[LANG_DIAL_BREDA][ID_TRENTO] = "Trentini";
    _mtxLangString[LANG_DIAL_BREDA][ID_TREVISO] = "Trevisani";
    _mtxLangString[LANG_DIAL_BREDA][ID_TRIESTE] = "triestini";
    _mtxLangString[LANG_DIAL_BREDA][ID_SICILIA] = "Siciliani";
    _mtxLangString[LANG_DIAL_BREDA][ID_CREDITS] = "Ringrasiament";

    _mtxLangString[LANG_DIAL_BREDA][ID_WELCOMETITLEBAR] =
        "At capiti giost al Sulitari";
    _mtxLangString[LANG_DIAL_BREDA][ID_MEN_OPTIONS] = "Upsion";
    _mtxLangString[LANG_DIAL_BREDA][ID_MN_HELP] = "Ad du nà man";
    _mtxLangString[LANG_DIAL_BREDA][ID_OK] = "Ukei";
    _mtxLangString[LANG_DIAL_BREDA][ID_CANCEL] = "Cancèla";
    _mtxLangString[LANG_DIAL_BREDA][ID_NEWGAME] = "Natra";
    _mtxLangString[LANG_DIAL_BREDA][ID_CHOOSEBACKGROUND] = "Sfond";
    _mtxLangString[LANG_DIAL_BREDA][ID_COMMESSAGGIO] = "Cumsàs";
    _mtxLangString[LANG_DIAL_BREDA][ID_MANTOVA] = "Mantua";
    _mtxLangString[LANG_DIAL_BREDA][ID_BLACK] = "Negàr";
    _mtxLangString[LANG_DIAL_BREDA][ID_SCORE] = "Punt";
    _mtxLangString[LANG_DIAL_BREDA][ID_YES] = "Si";
    _mtxLangString[LANG_DIAL_BREDA][ID_NO] = "No";
    _mtxLangString[LANG_DIAL_BREDA][ASK_NEWGAME] =
        "Voet dabòn incumincià na partdia növa?";
    _mtxLangString[LANG_DIAL_BREDA][ASK_QUIT] = "Voet dabòn andà dal jeuq?";
    _mtxLangString[LANG_DIAL_BREDA][FINAL_SCORE] =
        "I me cumpliment, te riès al sulitari e tè fat %d punt";
    _mtxLangString[LANG_DIAL_BREDA][NAME] = "Nom";
    _mtxLangString[LANG_DIAL_BREDA][POINTS] = "Punt";
    _mtxLangString[LANG_DIAL_BREDA][CARDDECK] = "Carti in dal mas";
    _mtxLangString[LANG_DIAL_BREDA][ON] = "impià";
    _mtxLangString[LANG_DIAL_BREDA][OFF] = "smors";
    _mtxLangString[LANG_DIAL_BREDA][PLEASE_ENTER_NAME] = "Al to nom";

    // italiano
    _mtxLangString[LANG_ITA][ID_START] = "Inizia";
    _mtxLangString[LANG_ITA][ID_LANGUAGESEL] = "Lingua";
    _mtxLangString[LANG_ITA][ID_HIGHSCORE] = "High Score";
    _mtxLangString[LANG_ITA][ID_VIDEOSET] = "Settings Display";
    _mtxLangString[LANG_ITA][ID_MAINMENU] = "Menu principale";
    _mtxLangString[LANG_ITA][ID_EXIT] = "Esci";
    _mtxLangString[LANG_ITA][ID_CHOOSELEVEL] = "Seleziona il gioco";
    _mtxLangString[LANG_ITA][ID_BACKMAINMENU] = "Vai al menu principale";
    _mtxLangString[LANG_ITA][ID_EASY] = "Facile";
    _mtxLangString[LANG_ITA][ID_HARD] = "Difficilissimo";
    _mtxLangString[LANG_ITA][ID_HARDER] = "Non facile";
    _mtxLangString[LANG_ITA][ID_BACK] = "Torna indietro";
    _mtxLangString[LANG_ITA][ID_SELECTDISPLAYSETTINGS] = "Seleziona il display";
    _mtxLangString[LANG_ITA][ID_INVIDO] = "Invidu";
    _mtxLangString[LANG_ITA][ID_CHOOSELANGUA] = "Seleziona la lingua";
    _mtxLangString[LANG_ITA][ID_ENGLISH] = "Inglese";
    _mtxLangString[LANG_ITA][ID_DIALETMN] = "Dialetto Bredese";
    _mtxLangString[LANG_ITA][ID_ITALIANO] = "Italiano";
    _mtxLangString[LANG_ITA][ID_ANONIM] = "Anonimo";
    _mtxLangString[LANG_ITA][ID_PUSHBUTTON] = "Premi un tasto";
    _mtxLangString[LANG_ITA][ID_CMD_UNKNOWN] = "Comando non riconosciuto";
    _mtxLangString[LANG_ITA][ID_CMD_HELP] = "Comandi che puoi usare";
    _mtxLangString[LANG_ITA][ID_CMD_CHATWELCOME] = "Bene arrivato nella chat";
    _mtxLangString[LANG_ITA][ID_CMD_CHATLIST] =
        "La lista dei presenti nella chat";
    _mtxLangString[LANG_ITA][ID_SOUNDOPT] = "Suono";
    _mtxLangString[LANG_ITA][ID_ABILITATO] = "Abilitato";
    _mtxLangString[LANG_ITA][ID_DISABILITATO] = "Disabilitato";

    _mtxLangString[LANG_ITA][ID_SOLITARIO] = "Solitario";
    _mtxLangString[LANG_ITA][ID_DECKNAME] = "Mazzi di carte";
    _mtxLangString[LANG_ITA][ID_CHOOSEDECK] = "Scegli il mazzo di carte";
    _mtxLangString[LANG_ITA][ID_PIACENTINA] = "Piacentine";
    _mtxLangString[LANG_ITA][ID_BERGAMO] = "Bergamesche";
    _mtxLangString[LANG_ITA][ID_BOLOGNA] = "Bolognesi";
    _mtxLangString[LANG_ITA][ID_GENOVA] = "Genovesi";
    _mtxLangString[LANG_ITA][ID_MILANO] = "Milanesi";
    _mtxLangString[LANG_ITA][ID_NAPOLI] = "Napoletane";
    _mtxLangString[LANG_ITA][ID_PIEMONTE] = "Piemontesi";
    _mtxLangString[LANG_ITA][ID_ROMAGNA] = "Romagnole";
    _mtxLangString[LANG_ITA][ID_SARDEGNA] = "Sarde";
    _mtxLangString[LANG_ITA][ID_TOSCANA] = "Toscane";
    _mtxLangString[LANG_ITA][ID_TRENTO] = "Trentine";
    _mtxLangString[LANG_ITA][ID_TREVISO] = "Trevigiane";
    _mtxLangString[LANG_ITA][ID_TRIESTE] = "Triestine";
    _mtxLangString[LANG_ITA][ID_SICILIA] = "Siciliane";
    _mtxLangString[LANG_ITA][ID_CREDITS] = "Credits";
    _mtxLangString[LANG_ITA][ID_WELCOMETITLEBAR] = "Il Solitario";
    _mtxLangString[LANG_ITA][ID_MEN_OPTIONS] = "Opzioni";
    _mtxLangString[LANG_ITA][ID_MN_HELP] = "Guida";
    _mtxLangString[LANG_ITA][ID_OK] = "OK";
    _mtxLangString[LANG_ITA][ID_CANCEL] = "Cancella";
    _mtxLangString[LANG_ITA][ID_NEWGAME] = "Nuova";
    _mtxLangString[LANG_ITA][ID_CHOOSEBACKGROUND] = "Sfondo";
    _mtxLangString[LANG_ITA][ID_COMMESSAGGIO] = "Commessaggio";
    _mtxLangString[LANG_ITA][ID_MANTOVA] = "Mantova";
    _mtxLangString[LANG_ITA][ID_BLACK] = "Nero";
    _mtxLangString[LANG_ITA][ID_SCORE] = "Punti";
    _mtxLangString[LANG_ITA][ID_YES] = "Si";
    _mtxLangString[LANG_ITA][ID_NO] = "No";
    _mtxLangString[LANG_ITA][ASK_NEWGAME] =
        "Vuoi davvero cominciare una nuova partita?";
    _mtxLangString[LANG_ITA][ASK_QUIT] = "Vuoi davvero uscire dal gioco?";
    _mtxLangString[LANG_ITA][FINAL_SCORE] =
        "Congratulazioni, hai finito il solitario con %d punti";
    _mtxLangString[LANG_ITA][NAME] = "Nome";
    _mtxLangString[LANG_ITA][POINTS] = "Punteggio";
    _mtxLangString[LANG_ITA][CARDDECK] = "Carte Mazzo";
    _mtxLangString[LANG_ITA][ON] = "ON";
    _mtxLangString[LANG_ITA][OFF] = "OFF";
    _mtxLangString[LANG_ITA][PLEASE_ENTER_NAME] = "Nome del giocatore";

    // English
    _mtxLangString[LANG_ENG][ID_START] = "Start";
    _mtxLangString[LANG_ENG][ID_LANGUAGESEL] = "Language";
    _mtxLangString[LANG_ENG][ID_HIGHSCORE] = "High Score";
    _mtxLangString[LANG_ENG][ID_VIDEOSET] = "Settings Display";
    _mtxLangString[LANG_ENG][ID_MAINMENU] = "Main menu";
    _mtxLangString[LANG_ENG][ID_EXIT] = "Exit";
    _mtxLangString[LANG_ENG][ID_CHOOSELEVEL] = "Select the game";
    _mtxLangString[LANG_ENG][ID_BACKMAINMENU] = "Go to main menu";
    _mtxLangString[LANG_ENG][ID_EASY] = "Easy";
    _mtxLangString[LANG_ENG][ID_HARD] = "Very difficult";
    _mtxLangString[LANG_ENG][ID_HARDER] = "Not easy";
    _mtxLangString[LANG_ENG][ID_BACK] = "Back";
    _mtxLangString[LANG_ENG][ID_SELECTDISPLAYSETTINGS] = "Select the display";
    _mtxLangString[LANG_ENG][ID_INVIDO] = "Invido";
    _mtxLangString[LANG_ENG][ID_CHOOSELANGUA] = "Select language";
    _mtxLangString[LANG_ENG][ID_ENGLISH] = "English";
    _mtxLangString[LANG_ENG][ID_DIALETMN] = "Bredese dialect";
    _mtxLangString[LANG_ENG][ID_ITALIANO] = "Italian";
    _mtxLangString[LANG_ENG][ID_ANONIM] = "Anonymous";
    _mtxLangString[LANG_ENG][ID_PUSHBUTTON] = "Press a key";
    _mtxLangString[LANG_ENG][ID_CMD_UNKNOWN] = "Unrecognized command";
    _mtxLangString[LANG_ENG][ID_CMD_HELP] = "Commands you can use";
    _mtxLangString[LANG_ENG][ID_CMD_CHATWELCOME] = "Well arrived in the chat";
    _mtxLangString[LANG_ENG][ID_CMD_CHATLIST] =
        "The list of those present in the chat";
    _mtxLangString[LANG_ENG][ID_SOUNDOPT] = "Sound";
    _mtxLangString[LANG_ENG][ID_ABILITATO] = "Enabled";
    _mtxLangString[LANG_ENG][ID_DISABILITATO] = "Disabled";

    _mtxLangString[LANG_ENG][ID_SOLITARIO] = "Solitaire";
    _mtxLangString[LANG_ENG][ID_DECKNAME] = "Card decks";
    _mtxLangString[LANG_ENG][ID_CHOOSEDECK] = "Choose the deck of cards";
    _mtxLangString[LANG_ENG][ID_PIACENTINA] = "Piacentine";
    _mtxLangString[LANG_ENG][ID_BERGAMO] = "Bergamesche";
    _mtxLangString[LANG_ENG][ID_BOLOGNA] = "Bolognesi";
    _mtxLangString[LANG_ENG][ID_GENOVA] = "Genovesi";
    _mtxLangString[LANG_ENG][ID_MILANO] = "Milanesi";
    _mtxLangString[LANG_ENG][ID_NAPOLI] = "Napoletane";
    _mtxLangString[LANG_ENG][ID_PIEMONTE] = "Piemontesi";
    _mtxLangString[LANG_ENG][ID_ROMAGNA] = "Romagnole";
    _mtxLangString[LANG_ENG][ID_SARDEGNA] = "Sarde";
    _mtxLangString[LANG_ENG][ID_TOSCANA] = "Toscane";
    _mtxLangString[LANG_ENG][ID_TRENTO] = "Trentine";
    _mtxLangString[LANG_ENG][ID_TREVISO] = "Trevigiane";
    _mtxLangString[LANG_ENG][ID_TRIESTE] = "Triestine";
    _mtxLangString[LANG_ENG][ID_SICILIA] = "Siciliane";
    _mtxLangString[LANG_ENG][ID_CREDITS] = "Credits";
    _mtxLangString[LANG_ENG][ID_WELCOMETITLEBAR] = "The Solitaire";
    _mtxLangString[LANG_ENG][ID_MEN_OPTIONS] = "Options";
    _mtxLangString[LANG_ENG][ID_MN_HELP] = "Help";
    _mtxLangString[LANG_ENG][ID_OK] = "OK";
    _mtxLangString[LANG_ENG][ID_CANCEL] = "Cancel";
    _mtxLangString[LANG_ENG][ID_NEWGAME] = "New";
    _mtxLangString[LANG_ENG][ID_CHOOSEBACKGROUND] = "Background";
    _mtxLangString[LANG_ENG][ID_COMMESSAGGIO] = "Commessaggio";
    _mtxLangString[LANG_ENG][ID_MANTOVA] = "Mantova";
    _mtxLangString[LANG_ENG][ID_BLACK] = "Black";
    _mtxLangString[LANG_ENG][ID_SCORE] = "Score";
    _mtxLangString[LANG_ENG][ID_YES] = "Yes";
    _mtxLangString[LANG_ENG][ID_NO] = "No";
    _mtxLangString[LANG_ENG][ASK_NEWGAME] =
        "Do you really want to start a new game?";
    _mtxLangString[LANG_ENG][ASK_QUIT] =
        "Do you really want to leave the game?";
    _mtxLangString[LANG_ENG][FINAL_SCORE] =
        "Congratulation, solitaire finished scoring: %d points";
    _mtxLangString[LANG_ENG][NAME] = "Name";
    _mtxLangString[LANG_ENG][POINTS] = "Points";
    _mtxLangString[LANG_ENG][CARDDECK] = "Deck Cards";
    _mtxLangString[LANG_ENG][ON] = "ON";
    _mtxLangString[LANG_ENG][OFF] = "OFF";
    _mtxLangString[LANG_ENG][PLEASE_ENTER_NAME] = "Please enter the name";
}

std::string Languages::GetStringId(eStringID eId) {
    if (eId < TOT_STRINGS && eId >= 0) {
        return _mtxLangString[_eLangid][eId];
    }

    return "";
}

const char* Languages::GetCStringId(eStringID eId) {
    if (eId < TOT_STRINGS && eId >= 0) {
        return _mtxLangString[_eLangid][eId].c_str();
    } else {
        return "";
    }
}
