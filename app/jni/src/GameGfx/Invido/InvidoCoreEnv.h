#ifndef _C_COREINVIDOENV_H_
#define _C_COREINVIDOENV_H_

#include <string>
#include <vector>


const int g_PointsTable[] = {
    /*Asso*/ 11, /*Due*/ 12,  /*Tre*/ 13,  /*Quattro*/ 4, /*cinque*/ 5,
    /*Sei*/ 6,   /*Sette*/ 7, /*Fante*/ 8, /*Cavallo*/ 9, /*Re*/ 10,
    /*Asso*/ 11, /*Due*/ 12,  /*Tre*/ 13,  /*Quattro*/ 4, /*cinque*/ 5,
    /*Sei*/ 6,   /*Sette*/ 7, /*Fante*/ 8, /*Cavallo*/ 9, /*Re*/ 10,
    /*Asso*/ 11, /*Due*/ 12,  /*Tre*/ 13,  /*Quattro*/ 4, /*cinque*/ 5,
    /*Sei*/ 6,   /*Sette*/ 7, /*Fante*/ 8, /*Cavallo*/ 9, /*Re*/ 10,
    /*Asso*/ 11, /*Due*/ 12,  /*Tre*/ 13,  /*Quattro*/ 4, /*cinque*/ 5,
    /*Sei*/ 6,   /*Sette*/ 7, /*Fante*/ 8, /*Cavallo*/ 9, /*Re*/ 10};

const std::string g_CardsNameX[] = {
    "Asso di bastoni",    "Due di bastoni",     "Tre di bastoni",
    "Quattro di bastoni", "Cinque di bastoni",  "Sei di bastoni",
    "Sette di bastoni",   "Fante di bastoni",   "Cavallo di bastoni",
    " Re di bastoni",     "Asso di  coppe",     "Due di  coppe",
    "Tre di  coppe",      "Quattro di  coppe",  "Cinque di  coppe",
    "Sei di  coppe",      "Sette di  coppe",    "Fante di  coppe",
    "Cavallo di  coppe",  " Re di  coppe",      "Asso di  denari",
    "Due di  denari",     "Tre di  denari",     "Quattro di  denari",
    "Cinque di  denari",  "Sei di  denari",     "Sette di  denari",
    "Fante di  denari",   "Cavallo di  denari", " Re di  denari",
    "Asso di  spade",     "Due di  spade",      "Tre di  spade",
    "Quattro di  spade",  "Cinque di  spade",   "Sei di  spade",
    "Sette di  spade",    "Fante di  spade",    "Cavallo di  spade",
    " Re di  spade"};

typedef std::vector<long> VCT_LONG;
typedef VCT_LONG::iterator IT_VCTLONG;
enum eGameConst {
    PLAYER_ME = 0,
    PLAYER_OPPONENT = 1,
    NUM_CARDS = 32,
    NUM_CARDS_MAZZBRI = 40,
    NUM_CARDS_HAND = 3,
    NUM_HANDS = 3,
    NUM_CARDS_PLAYED = 2,
    SCORE_GOAL = 24,
    SPECIAL_SCORE = 23,
    SPECIAL_SCORE_GOAL = 8,
    SCORE_SEVEN = 7,
    NOT_VALID_INDEX = -1,
    NUM_SEEDS = 5,
    BUFF_NAME = 64,
    PLAYER1 = 0,
    PLAYER2 = 1,
    PLAYER3 = 2,
    PLAYER4 = 3,
    MAX_NUM_PLAYER = 4,
    NUM_PLAY_INVIDO_2 = 2
};
enum eGameType { LOCAL_TYPE = 0, NET_CLIENT_TYPE = 1, NET_SERVER_TYPE = 2 };
enum ePartitaStatus {
    WAIT_NEW_PARTITA = 0,
    PARTITA_ONGOING = 1,
    PARTITA_END = 2
};
enum eGiocataStatus { WAIT_NEW_GIOCATA = 0, GIOCATA_ONGOING = 1 };
enum eManoStatus {
    MNST_WAIT_NEW_MANO = 0,
    MNST_WAIT_PLAY_PLAYER_1 = 1,
    MNST_WAIT_PLAY_PLAYER_2 = 2,
    MNST_WAIT_PLAY_PLAYER_3 = 3,
    MNST_WAIT_PLAY_PLAYER_4 = 4,
    MNST_MANO_END = 5,
    MNST_WAIT_RESP_PLAYER_1 = 6,
    MNST_WAIT_RESP_PLAYER_2 = 7,
    MNST_WAIT_RESP_PLAYER_3 = 8,
    MNST_WAIT_RESP_PLAYER_4 = 9,
};
enum eGameLevel {
    DUMMY = 0,
    BEGINNER = 1,
    ADVANCED = 2,
    NIGHTMARE = 3,
    HMI = 4,
    TEST_PYTHON,
    SERVER_LEVEL
};
enum eTypeOfPLayer { PT_LOCAL, PT_MACHINE, PT_REMOTE, PT_SERVER };
enum eScriptNotification {
    SCR_NFY_SHUFFLEDECK = 0,
    SCR_NFY_HAVETO_SAY = 1,
    SCR_NFY_HAVETO_PLAY = 2,
    SCR_NFY_NEWGIOCATA = 3,
    SCR_NFY_ALGMANOEND = 4,
    SCR_NFY_ALGGIOCATAEND = 5,
    SCR_NFY_ALGMATCHEND = 6,
    SCR_NFY_NEWMATCH = 7
};

enum eSUIT { BASTONI = 0, COPPE = 1, DENARI = 2, SPADE = 3, UNDEF = 4 };

enum eSayPlayer {
    AMONTE = 0,
    INVIDO = 1,
    TRASMAS = 2,
    TRASMASNOEF = 3,
    FUERAJEUQ = 4,
    PARTIDA = 5,
    VABENE = 6,
    VADOVIA = 7,
    CHIAMADIPIU = 8,
    NO = 9,
    VADODENTRO = 10,
    GIOCA = 11,
    CHIAMA_BORTOLO = 12,
    NOTHING = 13
};

enum eGiocataScoreState {
    SC_AMONTE = 0,
    SC_PATTA = 0,
    SC_CANELA = 1,
    SC_INVIDO = 3,
    SC_TRASMAS = 6,
    SC_TRASMASNOEF = 9,
    SC_FUERAJEUQ = 12,
    SC_PARTIDA = 24,
};

#endif