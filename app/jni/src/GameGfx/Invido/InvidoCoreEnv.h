#ifndef _C_COREINVIDOENV_H_
#define _C_COREINVIDOENV_H_

#include <SDL3/SDL.h>

#include <vector>

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

enum eSayPlayer {
    SP_AMONTE = 0,
    SP_INVIDO = 1,
    SP_TRASMAS = 2,
    SP_TRASMASNOEF = 3,
    SP_FUERAJEUQ = 4,
    SP_PARTIDA = 5,
    SP_VABENE = 6,
    SP_VADOVIA = 7,
    SP_CHIAMADIPIU = 8,
    SP_NO = 9,
    SP_VADODENTRO = 10,
    SP_GIOCA = 11,
    SP_CHIAMA_BORTOLO = 12,
    SP_NOTHING = 13
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

typedef std::vector<eSayPlayer> VCT_COMMANDS;

#endif