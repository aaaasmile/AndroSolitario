

#include "AlgPlayer.h"

#include <map>

#include "Config.h"
#include "InvidoCoreEnv.h"
#include "TraceService.h"

typedef std::map<eSayPlayer, LPCSTR> _MAP_SAY;
static _MAP_SAY g_MapSay;

AlgAdvancedPlayer::AlgAdvancedPlayer() {
    m_ixCurrMano = 0;
    m_pCoreGame = 0;
    m_iOppIndex = NOT_VALID_INDEX;
    m_itrChan = 3;
    g_MapSay[eSayPlayer::SP_AMONTE] = "A monte";
    g_MapSay[eSayPlayer::SP_INVIDO] = "Invido";
    g_MapSay[eSayPlayer::SP_TRASMAS] = "Tras mas";
    g_MapSay[eSayPlayer::SP_TRASMASNOEF] = "Tras mas noef";
    g_MapSay[eSayPlayer::SP_FUERAJEUQ] = "Feura jeuq";
    g_MapSay[eSayPlayer::SP_PARTIDA] = "Partida";
    g_MapSay[eSayPlayer::SP_VABENE] = "Va bene";
    g_MapSay[eSayPlayer::SP_VADOVIA] = "Vado via";
    g_MapSay[eSayPlayer::SP_CHIAMADIPIU] = "Chiama di più";
    g_MapSay[eSayPlayer::SP_NO] = "No";
    g_MapSay[eSayPlayer::SP_VADODENTRO] = "Vado dentro";
    g_MapSay[eSayPlayer::SP_GIOCA] = "Gioca";
}

AlgAdvancedPlayer::~AlgAdvancedPlayer() {}

void AlgAdvancedPlayer::ALG_SetCoreInterface(I_CORE_Game* pCore) {
    m_pCoreGame = pCore;
}

void AlgAdvancedPlayer::ALG_SetPlayerIndex(int iIndex) { m_iMyIndex = iIndex; }

void AlgAdvancedPlayer::ALG_SetOpponentIndex(int iIndex, int iOpponentNr) {
    m_iOppIndex = iIndex;
}
void AlgAdvancedPlayer::ALG_SetAssociateIndex(int iIndex) {}

void AlgAdvancedPlayer::ALG_NewMatch(int iNumPlayer) {}

void AlgAdvancedPlayer::ALG_NewGiocata(const CARDINFO* pCardArray,
                                       int iNumOfCards, int iPlayerIx) {
    SDL_assert(iNumOfCards == NUM_CARDS_HAND);
    int i;
    m_iNumManiWon = 0;
    m_iNumChiamateMonte = 0;
    m_iNumChiamateInGiocata = 0;
    m_bIamCalledPoints = false;
    m_opponetIsVadoDentro = false;
    m_WonFirstHand = false;
    m_ixCurrMano = 0;

    m_iPlayerOnTurn = iPlayerIx;

    TRACE_DEBUG("[TRALG]Giocata new:");
    for (i = 0; i < NUM_CARDS_HAND; i++) {
        m_vct_Cards_CPU[i].SetCardInfo(pCardArray[i]);
        m_arrIxPlayerWonHand[i] = -1;
        TRACE_DEBUG("%d , carta: %s ", i, pCardArray[i].CardName.c_str());
    }
    TRACE_DEBUG("\n");
    for (i = 0; i < NUM_HANDS; i++) {
        m_vct_Cards_played[i].clear();
    }

    m_MyLastSay = eSayPlayer::SP_VABENE;
    m_sayMyRisp = eSayPlayer::SP_VABENE;
    m_OpponentSay = eSayPlayer::SP_VABENE;
    m_sayOppRisp = eSayPlayer::SP_VABENE;
    m_bLastManoPatada = false;

    CardSpec Card;
    TRACE_DEBUG("[TRALG]Cards of player %d are: \n", m_iMyIndex);
    for (int i = 0; i < NUM_CARDS_HAND; i++) {
        Card.SetCardInfo(pCardArray[i]);
        TRACE_DEBUG("[%s] ", Card.GetName());
    }
    TRACE_DEBUG("\n");
}

void AlgAdvancedPlayer::ALG_PlayerHasVadoDentro(int iPlayerIx) {
    TRACE_DEBUG("[TRALG]Player %1 va dentro\n", iPlayerIx);
    CardSpec cardDentro;
    if (iPlayerIx == m_iMyIndex) {
        // l'idea è quella di ritrovare la carta con la quale si è andati dentro
        // e poi di chiamare ALG_PlayerHasPlayed
        SDL_assert(m_iCPUCardDentroPos != eGameConst::NOT_VALID_INDEX);
        SDL_assert(m_vct_Cards_CPU[m_iCPUCardDentroPos].GetCardIndex() !=
                   eGameConst::NOT_VALID_INDEX);
        cardDentro.SetCardIndex(
            m_vct_Cards_CPU[m_iCPUCardDentroPos].GetCardIndex());
        ALG_PlayerHasPlayed(iPlayerIx, cardDentro.GetCardInfo());
        m_iCPUCardDentroPos = eGameConst::NOT_VALID_INDEX;
    } else {
        m_opponetIsVadoDentro = true;
        cardDentro.SetCardIndex(3);
        ALG_PlayerHasPlayed(iPlayerIx, cardDentro.GetCardInfo());
    }
}

void AlgAdvancedPlayer::doVadoDentro(int cardPos) {
    CardSpec cardUndef;
    m_iCPUCardDentroPos = cardPos;
    SDL_assert(m_vct_Cards_CPU[cardPos] != cardUndef);
    m_pCoreGame->Player_vaDentro(m_iMyIndex,
                                 m_vct_Cards_CPU[cardPos].GetCardInfo());
}

void AlgAdvancedPlayer::ALG_PlayerHasPlayed(int iPlayerIx,
                                            const CARDINFO* pCard) {
    SDL_assert(pCard);
    SDL_assert(pCard->byIndex != eGameConst::NOT_VALID_INDEX);
    int i;
    if (iPlayerIx == m_iMyIndex) {
        CardSpec Card;
        CardSpec CardUndef;
        Card.SetCardInfo(*pCard);
        bool bFound = false;
        // card successfully played
        for (i = 0; !bFound && i < eGameConst::NUM_CARDS_HAND; i++) {
            if (Card == m_vct_Cards_CPU[i]) {
                // card found
                m_vct_Cards_CPU[i] = CardUndef;
                bFound = true;
            }
        }
        SDL_assert(bFound);
    } else {
        // oponent play
        m_OpponentSay = eSayPlayer::SP_VABENE;
    }
    SDL_assert(pCard->byIndex != eGameConst::NOT_VALID_INDEX);
    TRACE_DEBUG("[TRALG]Player %d has played [%s]\n", iPlayerIx,
                pCard->CardName.c_str());

    CardSpec cardPlayed;
    cardPlayed.SetCardInfo(*pCard);
    m_vct_Cards_played[m_ixCurrMano].push_back(cardPlayed);

    m_iPlayerOnTurn = iPlayerIx == 0 ? 1 : 0;
    m_iNumChiamateMonte = 0;
    m_sayMyRisp = eSayPlayer::SP_VABENE;
}

void AlgAdvancedPlayer::ALG_PlayerHasSaid(int iPlayerIx, eSayPlayer eSay) {
    LPCSTR lpsNameSay = g_MapSay[eSay];
    TRACE_DEBUG("[TRALG]Player %d has said %s\n", iPlayerIx, lpsNameSay);
    if (iPlayerIx == m_iOppIndex) {
        // Avversario ha parlato
        if (eSay > eSayPlayer::SP_PARTIDA) {
            m_sayOppRisp = eSay;  // Risposta alla chiamata
        } else {
            // avversario ha chiamato dei punti
            m_OpponentSay = eSay;
        }
    } else if (iPlayerIx == m_iMyIndex) {
        // Algoritmo ha parlato
        if (eSay > eSayPlayer::SP_PARTIDA) {
            m_sayMyRisp = eSay;  // Risposta alla chiamata
        } else {
            // chiamata a punti
            m_MyLastSay = eSay;
            m_bIamCalledPoints = true;
        }
    }
}

bool AlgAdvancedPlayer::Cagna(int lastNumChiamate) {
    VCT_COMMANDS vct_cmd;
    SDL_assert(m_pCoreGame);
    bool res = false;
    m_pCoreGame->GetAdmittedCommands(vct_cmd, m_iMyIndex);
    size_t iNumCmds = vct_cmd.size();
    if (iNumCmds > 0) {
        eSayPlayer eSay;

        int iTry = 0;
        do {
            size_t iRndIndex = SDL_rand(iNumCmds);
            eSay = vct_cmd[iRndIndex];
            iTry++;
        } while (eSay == eSayPlayer::SP_VADOVIA && iTry < 5);
        Chiama(eSay, lastNumChiamate);
        res = true;
    }
    return res;
}

bool AlgAdvancedPlayer::ChiamaAMonte(int lastNumChiamate) {
    m_iNumChiamateMonte += 1;
    if (m_iNumChiamateMonte < 4) {
        Chiama(eSayPlayer::SP_AMONTE, lastNumChiamate);
        return true;
    }
    return false;
}

void AlgAdvancedPlayer::Chiama(eSayPlayer eSay, int lastChiamataNum) {
    SDL_assert(lastChiamataNum == m_iNumChiamateInGiocata);
    m_iNumChiamateInGiocata += 1;
    m_pCoreGame->Player_saySomething(m_iMyIndex, eSay);
}

bool AlgAdvancedPlayer::ChiamaDiPiu(int lastNumChiamate) {
    VCT_COMMANDS vct_cmd;
    SDL_assert(m_pCoreGame);

    m_pCoreGame->GetAdmittedCommands(vct_cmd, m_iMyIndex);
    for (size_t i = 0; i < vct_cmd.size(); i++) {
        eSayPlayer eSay = vct_cmd[i];
        if (eSay >= eSayPlayer::SP_INVIDO && eSay <= eSayPlayer::SP_PARTIDA) {
            Chiama(eSay, lastNumChiamate);
            return true;
        }
    }
    return false;
}

void AlgAdvancedPlayer::PlayAsFirst() {
    int lastNumChiamate = m_iNumChiamateInGiocata;
    CARDINFO* result = NULL;
    CardSpec cardUndef{};  // TODO: check if this is correct
    int curr_mano = NumMano();
    if (SDL_rand(40) >= 35 && !m_bIamCalledPoints) {
        if (Cagna(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PLF_R1cagna\n");
            return;
        }
    }

    int i, maxpoints = 0, max_pos = 0, sum_points = 0, min_pos = 0;
    int min_points = 20;
    int med_points = 0, med_pos = 0;
    int arrPoints[eGameConst::NUM_CARDS_HAND] = {0, 0, 0};
    for (i = 0; i < eGameConst::NUM_CARDS_HAND; i++) {
        if (m_vct_Cards_CPU[i] == cardUndef) {
            continue;
        }
        int points = m_vct_Cards_CPU[i].GetPoints();
        if (points > maxpoints) {
            med_points = maxpoints;
            med_pos = max_pos;
            maxpoints = points;
            max_pos = i;
        } else if (points >= med_points) {
            med_points = points;
            med_pos = i;
        }
        if (points < min_points) {
            min_pos = i;
            min_points = points;
        }
        arrPoints[i] = points;
        sum_points += points;
    }
    TRACE_DEBUG(
        "[1ST]Take: min pos %d (pt %d), med pos %d (pt %d), max pos %d(pt %d) "
        "\n",
        min_pos, min_points, med_pos, med_points, max_pos, maxpoints);

    if (curr_mano == 2 && m_iNumManiWon == 1) {
        result = m_vct_Cards_CPU[min_pos].GetCardInfo();
        TRACE_DEBUG("Candidate from min position\n");
    } else {
        result = m_vct_Cards_CPU[max_pos].GetCardInfo();
    }
    TRACE_DEBUG("[TRALG]candidate play PLF_pos: %s\n",
                result->CardName.c_str());

    if (curr_mano == 2 && m_iNumManiWon == 1) {
        int pointsCardMano1_pl1 = m_vct_Cards_played[0][0].GetPoints();
        int pointsCardMano1_pl2 = m_vct_Cards_played[0][1].GetPoints();
        int maxPointsMano1 = pointsCardMano1_pl1 > pointsCardMano1_pl2
                                 ? pointsCardMano1_pl1
                                 : pointsCardMano1_pl2;

        // seconda mano prima mano vinta
        TRACE_DEBUG("[TRALG]Vado dentro? maxpoints is %d\n", maxpoints);
        if (maxpoints == 13) {
            // tre in mano che vince sicuro
            if (SDL_rand(10) != 9) {
                TRACE_DEBUG("[TRALG]Dentro per win\n");
                doVadoDentro(min_pos);
                return;
            }
        } else if (maxpoints > 10) {
            if (SDL_rand(maxpoints) > 9) {
                TRACE_DEBUG("[TRALG]Dentro per win con rischio\n");
                doVadoDentro(min_pos);
                return;
            }
        } else if (result != NULL && min_points < maxPointsMano1) {
            TRACE_DEBUG("[TRALG]Dentro per bluf\n");
            doVadoDentro(min_pos);
            return;
        } else if (SDL_rand(3) == 1 && m_eScoreCurrent <= SC_INVIDO) {
            if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PLF_R5_primavinta\n");
                result = NULL;
            }
        }
        if (result != NULL) {
            result = m_vct_Cards_CPU[min_pos].GetCardInfo();
            TRACE_DEBUG("[TRALG]PLF_cand_min_pos: %s\n",
                        result->CardName.c_str());
        }
    } else if (m_iNumManiWon == 1 && maxpoints >= 12 &&
               m_arrIxPlayerWonHand[0] == m_iMyIndex) {
        if (ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PLF_R1bisFortePrimamia\n");
            result = NULL;
        }
    } else if (curr_mano == 1 && maxpoints >= 12 && med_points < 10 &&
               med_points > 0) {
        result = m_vct_Cards_CPU[med_pos].GetCardInfo();
        TRACE_DEBUG("[TRALG]PLF_cand_med_pos: %s\n", result->CardName.c_str());
    } else if (curr_mano == 1 && maxpoints >= 11 && med_points < 10) {
        if (SDL_rand(10) > 5) {
            result = m_vct_Cards_CPU[med_pos].GetCardInfo();
            TRACE_DEBUG("[TRALG]PLF_cand_med_pos Brnd: %s\n",
                        result->CardName.c_str());
        }
    } else if (m_iNumManiWon == 1 && maxpoints >= 12 &&
               m_eScoreCurrent == SC_CANELA) {
        if (ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PLF_R1Bvintauna\n");
            result = NULL;
        }
    } else if (m_bLastManoPatada) {
        if (maxpoints == 13) {
            if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PLF_R2A_manopatada\n");
                result = NULL;
            }
        } else if (maxpoints <= 10) {
            if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PLF_R2B_manopatada\n");
                result = NULL;
            }
        } else if ((maxpoints == 12 || maxpoints == 11) &&
                   m_eScoreCurrent <= SC_INVIDO) {
            if (SDL_rand(30) < maxpoints && result != NULL &&
                ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PLF_R2C_manopatada\n");
                result = NULL;
            }
        }
    } else if (maxpoints == 13 && sum_points > 21 && SDL_rand(10) > 6 &&
               m_eScoreCurrent <= SC_INVIDO) {
        if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PLF_R3_tre\n");
            result = NULL;
        }
    } else if (sum_points < 15 && curr_mano == 1 && SDL_rand(2) == 1) {
        // mano scarsa
        if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PLF_R4_poco\n");
            result = NULL;
        }
    } else if (curr_mano == 3 && m_arrIxPlayerWonHand[0] == m_iMyIndex) {
        // terza mano prima mia
        if (maxpoints == 13) {
            if (SDL_rand(20) > 16) {
                // bluff
                if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                    result = NULL;
                    TRACE_DEBUG("[TRALG]PLF_R7_bluff_coltre\n");
                }
            } else {
                if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                    result = NULL;
                    TRACE_DEBUG("[TRALG]PLF_R8_tre\n");
                }
            }
        } else if (maxpoints == 12) {
            if (SDL_rand(20) > 14) {
                if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R9_bluffcoldue\n");
                    result = NULL;
                }
            } else if (SDL_rand(2) == 1 && m_eScoreCurrent <= SC_INVIDO) {
                if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R10_due\n");
                    result = NULL;
                }
            }
        } else if (maxpoints == 10 || maxpoints == 9) {
            if (SDL_rand(20) > 11) {
                if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R11_poco\n");
                    result = NULL;
                }
            } else if (SDL_rand(20) > 15 && m_eScoreCurrent <= SC_INVIDO) {
                if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R12_bluffconpoco\n");
                    result = NULL;
                }
            }
        } else {
            if (SDL_rand(20) > 7) {
                if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R13_amonte\n");
                    result = NULL;
                }
            }
        }

    } else if (curr_mano == 3 && m_arrIxPlayerWonHand[0] == m_iOppIndex) {
        // terza mano, prima vinta dall'avversario
        if (maxpoints == 13) {
            if (SDL_rand(20) > 15) {
                // bluff
                if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R14_bluffcoltre\n");
                    result = NULL;
                }
            } else if (SDL_rand(20) > 9 && m_eScoreCurrent <= SC_INVIDO) {
                if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R15_tre\n");
                    result = NULL;
                }
            }
        } else if (maxpoints == 12) {
            if (SDL_rand(20) > 12) {
                if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R16_montecoldue\n");
                    result = NULL;
                }
            } else if (SDL_rand(3) == 1) {
                if (result != NULL && ChiamaDiPiu(lastNumChiamate) &&
                    m_eScoreCurrent <= SC_INVIDO) {
                    TRACE_DEBUG("[TRALG]PLF_R17_rilanciodue\n");
                    result = NULL;
                }
            }
        } else if (maxpoints == 10 || maxpoints == 9) {
            if (SDL_rand(20) > 9) {
                if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R17_montecolre\n");
                    result = NULL;
                }
            } else if (SDL_rand(20) > 17) {
                if (result != NULL && ChiamaDiPiu(lastNumChiamate) &&
                    m_eScoreCurrent <= SC_INVIDO) {
                    TRACE_DEBUG("[TRALG]PLF_R18_rilanciocolre\n");
                    result = NULL;
                }
            }
        } else {
            if (SDL_rand(20) > 5) {
                if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG]PLF_R19_monte\n");
                    result = NULL;
                }
            }
        }
    }

    if (SDL_rand(40) > 37) {
        // chiamata a monte rara
        if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PLF_R20_monteraro\n");
            result = NULL;
        }
    }

    if (lastNumChiamate == m_iNumChiamateInGiocata && result == NULL) {
        SDL_assert(0);
        TRACE_DEBUG("[TRALG]PLF_R21_giocaacaso\n");
        GiocaACaso();
    }

    if (result != NULL) {
        SDL_assert(m_pCoreGame);
        m_pCoreGame->Player_playCard(m_iMyIndex, result);
    } else {
        TRACE_DEBUG("[TRALG]PLF non gioca carta per chiamata\n");
    }
}

void AlgAdvancedPlayer::GiocaACaso() {
    CardSpec cardUndef;
    int iCartaPos = SDL_rand(3);
    int iLoops = 0;
    while (m_vct_Cards_CPU[iCartaPos] == cardUndef && iLoops < NUM_CARDS_HAND) {
        iCartaPos++;
        if (iCartaPos >= NUM_CARDS_HAND) {
            iCartaPos = 0;
        }
        iLoops++;
    }
    SDL_assert(m_pCoreGame);
    m_pCoreGame->Player_playCard(m_iMyIndex,
                                 m_vct_Cards_CPU[iCartaPos].GetCardInfo());
}

void AlgAdvancedPlayer::PlayAsSecond() {
    CARDINFO* result = NULL;
    CardSpec cardUndef;
    SDL_assert(m_vct_Cards_played[m_ixCurrMano].size() == 1);
    CardSpec cardPlayed = m_vct_Cards_played[m_ixCurrMano].front();

    int pointsFirstCard = cardPlayed.GetPoints();
    int curr_mano = NumMano();
    int lastNumChiamate = m_iNumChiamateInGiocata;

    int iLoops = 0, i, maxpoints = 0, max_pos = 0, sum_points = 0, min_pos = 0;
    int min_points = 20, same_points_pos = -1, first_take_pos = -1,
        first_take_points = 20;
    int arrPoints[NUM_CARDS_HAND] = {0, 0, 0};
    for (i = 0; i < NUM_CARDS_HAND; i++) {
        if (m_vct_Cards_CPU[i] == cardUndef) {
            continue;
        }
        int points = m_vct_Cards_CPU[i].GetPoints();
        if (points > maxpoints) {
            maxpoints = points;
            max_pos = i;
        }
        if (points < min_points) {
            min_pos = i;
            min_points = points;
        }
        if (pointsFirstCard == points) {
            same_points_pos = i;
        }
        if (points > pointsFirstCard && points < first_take_points) {
            first_take_pos = i;  // La carta pi� bassa che pu� prendere
            first_take_points = points;
        }
        arrPoints[i] = points;
        sum_points += points;
    }
    TRACE_DEBUG(
        "[2ND]Points played card %d, Take: min pos %d (pt %d), first_take_pos "
        "%d (pt %d), max pos %d(pt %d) \n",
        pointsFirstCard, min_pos, min_points, first_take_pos, first_take_points,
        max_pos, maxpoints);
    if (m_opponetIsVadoDentro) {
        // Avversario è  andato dentro, quindi si gioca la carta più bassa che
        // si vince la mano
        result = m_vct_Cards_CPU[min_pos].GetCardInfo();
        TRACE_DEBUG("[TRALG]PL2nd_min_vadodentro: %s\n",
                    result->CardName.c_str());
    } else if (first_take_pos != -1) {
        // si prende
        result = m_vct_Cards_CPU[first_take_pos].GetCardInfo();
        TRACE_DEBUG("[TRALG]PL2nd_cand_first_take_pos: %s\n",
                    result->CardName.c_str());
    } else if (same_points_pos != -1) {
        // poi se patta
        result = m_vct_Cards_CPU[same_points_pos].GetCardInfo();
        TRACE_DEBUG("[TRALG]PL2nd_cand_same_pos: %s\n",
                    result->CardName.c_str());
    } else {
        // perdo, quindi gioca la carta pi bassa
        result = m_vct_Cards_CPU[min_pos].GetCardInfo();
        TRACE_DEBUG("[TRALG]PL2nd_cand_min_pos: %s\n",
                    result->CardName.c_str());
    }

    if (m_opponetIsVadoDentro) {
        // play the min card and say nothing
    } else if (pointsFirstCard > maxpoints && m_iNumManiWon == 0 &&
               curr_mano > 1) {
        if (SDL_rand(20) < 16) {
            if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PL2nd_R1_monte\n");
                result = NULL;
            }
        }
    } else if (curr_mano == 3 && pointsFirstCard == maxpoints &&
               pointsFirstCard == 13 && m_arrIxPlayerWonHand[0] == m_iMyIndex) {
        if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PL2nd_R2_megacagna\n");
            result = NULL;
        } else if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PL2nd_R2C_piu\n");
            result = NULL;
        }

    } else if (first_take_pos == -1 && m_iNumManiWon == 0 && curr_mano > 1) {
        if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PL2nd_R1B_perdosempre\n");
            result = NULL;
        }
    } else if (first_take_pos != -1 && m_bLastManoPatada) {
        int rest = pointsFirstCard >= 12 ? 15 : 35;
        if (SDL_rand(40) > rest) {
            if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PL2nd_R2A_monte_cagna\n");
                result = NULL;
            }
        } else {
            if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PL2nd_R2B_chiama\n");
                result = NULL;
            }
        }
    } else if (curr_mano == 3 && same_points_pos != -1 && m_iNumManiWon == 1 &&
               m_WonFirstHand) {
        // terza posso pattare e la prima è mia, si vice sicuro, quindi chiama
        // di più
        if ((SDL_rand(30 - pointsFirstCard) > 3) ||
            (m_MyLastSay == SP_AMONTE)) {
            if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PL2nd_R3B_pata_chiama\n");
                result = NULL;
            }
        } else {
            if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PL2nd_R3B_pata_cagna\n");
                result = NULL;
            }
        }
    } else if (curr_mano == 3 && first_take_pos != -1 && m_iNumManiWon == 1) {
        if (SDL_rand(40) > 35 && pointsFirstCard >= 10 &&
            m_eScoreCurrent <= SC_TRASMAS) {
            if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PL2nd_R2B_monte_cagna\n");
                result = NULL;
            }
        } else {
            if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PL2nd_R2B_chiama\n");
                result = NULL;
            }
        }
    } else if (curr_mano == 1 && first_take_pos != -1 &&
               pointsFirstCard <= 11) {
        if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG]PL2nd_R3A_chiama\n");
            result = NULL;
        }
    } else if (same_points_pos != -1 && maxpoints >= 12) {
        result = m_vct_Cards_CPU[same_points_pos].GetCardInfo();
        TRACE_DEBUG("[TRALG]PL2nd_cand_same_points_pos: %s\n",
                    result->CardName.c_str());
        if (SDL_rand(20) > 5) {
            if (result != NULL && ChiamaDiPiu(lastNumChiamate)) {
                result = NULL;
                TRACE_DEBUG("[TRALG]PL2nd_R4A_dipiu\n");
            }
        } else if (SDL_rand(20) > 17) {
            if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PL2nd_R4B_monte\n");
                result = NULL;
            }
        }
    } else if (pointsFirstCard > maxpoints) {
        // si perde la mano
        if (SDL_rand(20) > 15 || (curr_mano == 3 && m_iNumManiWon == 1) ||
            (curr_mano == 2 && m_iNumManiWon == 0)) {
            if (result != NULL && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG]PL2nd_R5_monte\n");
                result = NULL;
            }
        }
        if (result != NULL && curr_mano > 1) {
            // Vado dentro
            TRACE_DEBUG("[TRALG]PL2nd non prendo, quindi dentro per vergona\n");
            doVadoDentro(min_pos);
            return;
        }
    } else if (lastNumChiamate == m_iNumChiamateInGiocata && result == NULL) {
        TRACE_DEBUG("[TRALG]PL2nd_R5_acaso\n");
        SDL_assert(0);
        GiocaACaso();
    }

    if (result != NULL) {
        m_pCoreGame->Player_playCard(m_iMyIndex, result);
    } else {
        TRACE_DEBUG("[TRALG]PL2nd non gioca per chiamata pending\n");
    }
}

bool AlgAdvancedPlayer::IsPlayerFirst() {
    return m_vct_Cards_played[m_ixCurrMano].size() == 0 ? true : false;
}

// Ritorna: 1,2,3 a seconda della mano
int AlgAdvancedPlayer::NumMano() { return m_ixCurrMano + 1; }

void AlgAdvancedPlayer::ALG_Play() {
    if (IsPlayerFirst()) {
        PlayAsFirst();
    } else {
        PlayAsSecond();
    }
}

void AlgAdvancedPlayer::ALG_ManoEnd(I_MatchScore* pScore) {
    m_opponetIsVadoDentro = false;
    m_ixCurrMano++;

    m_bLastManoPatada = pScore->IsManoPatada();
    int index = pScore->GetManoNum() - 1;
    SDL_assert(index >= 0 && index < NUM_CARDS_HAND);
    m_arrIxPlayerWonHand[index] =
        m_bLastManoPatada ? -2 : pScore->GetManoWinner();
    m_iNumChiamateMonte = 0;
    if (pScore->GetManoWinner() == m_iMyIndex && !pScore->IsManoPatada()) {
        if (index == 0 || (index == 1 && m_bLastManoPatada)) {
            m_WonFirstHand = true;
        }
        m_iNumManiWon += 1;
    }
}

void AlgAdvancedPlayer::ALG_GiocataEnd(I_MatchScore* pScore) {
    if (m_iMyIndex == 0) {
        bool bIsPata = pScore->IsGiocataPatada();
        if (bIsPata) {
            // giocata patada
            TRACE_DEBUG("[TRALG]Giocata patada\n");
        } else if (pScore->IsGiocataMonte()) {
            // giocata a monte
            TRACE_DEBUG("[TRALG]Giocata a monte\n");
        } else {
            // winner
            int iPlWinner = pScore->GetGiocataWinner();

            TRACE_DEBUG("[TRALG]Giocata vinta da %d\n", iPlWinner);
            TRACE_DEBUG(
                "[TRALG]Punteggio player 0: %d, punteggio player 1: %d\n",
                pScore->GetPointsPlayer(m_iMyIndex),
                pScore->GetPointsPlayer(m_iOppIndex));
        }
    }
}

void AlgAdvancedPlayer::ALG_MatchEnd(I_MatchScore* pScore) {
    int iPlWinner = pScore->GetMatchWinner();
    TRACE_DEBUG("[TRALG]Match vinto da %d\n", iPlWinner);
    TRACE_DEBUG("[TRALG]Punteggio player 0: %d, punteggio player 1: %d\n",
                pScore->GetPointsPlayer(m_iMyIndex),
                pScore->GetPointsPlayer(m_iOppIndex));
}

void AlgAdvancedPlayer::ALG_HaveToRespond() {
    TRACE_DEBUG("[TRALG] have to respond");
    CardSpec cardUndef;
    int lastNumChiamate = m_iNumChiamateInGiocata;
    if (m_OpponentSay == SP_AMONTE && m_sayMyRisp == eSayPlayer::SP_NO) {
        // a monte was called and it was responded NO, don't change idea
        Chiama(m_sayMyRisp, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say R1");
        return;
    }

    size_t iRndIndex;
    VCT_COMMANDS vct_cmd;
    SDL_assert(m_pCoreGame);

    int pointsFirstCard = -1;
    if (m_vct_Cards_played[m_ixCurrMano].size() > 0) {
        pointsFirstCard = m_vct_Cards_played[m_ixCurrMano][0].GetPoints();
    }

    m_pCoreGame->GetAdmittedCommands(vct_cmd, m_iMyIndex);
    size_t iNumCmds = vct_cmd.size();
    int iLoops = 0, i, maxpoints = 0, max_pos = 0, sum_points = 0, min_pos = 0;
    int min_points = 20;
    int arrPoints[NUM_CARDS_HAND] = {0, 0, 0};
    for (i = 0; i < NUM_CARDS_HAND; i++) {
        if (m_vct_Cards_CPU[i] == cardUndef) {  // TODO check
            continue;
        }
        int points = m_vct_Cards_CPU[i].GetPoints();
        if (points > maxpoints) {
            maxpoints = points;
            max_pos = i;
        }
        if (points < min_points) {
            min_pos = i;
            min_points = points;
        }
        arrPoints[i] = points;
        sum_points += points;
    }
    int curr_mano = NumMano();
    TRACE_DEBUG("[ALG] Say: points first card %d, max points %d \n",
                pointsFirstCard, maxpoints);

    if (m_OpponentSay >= eSayPlayer::SP_INVIDO && m_OpponentSay <= eSayPlayer::SP_PARTIDA) {
        handleSayPopints(curr_mano, pointsFirstCard, lastNumChiamate, maxpoints,
                         sum_points);
    } else if (m_OpponentSay == SP_AMONTE) {
        handleSayAmonte(curr_mano, pointsFirstCard, lastNumChiamate, maxpoints,
                        sum_points);
    } else {
        iRndIndex = SDL_rand(iNumCmds);
        Chiama(vct_cmd[iRndIndex], lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say R18");
    }
}

void AlgAdvancedPlayer::handleSayPopints(int curr_mano, int pointsFirstCard,
                                         int lastNumChiamate, int maxpoints,
                                         int sum_points) {
    if (maxpoints == 13 && m_bLastManoPatada) {
        if (!ChiamaDiPiu(lastNumChiamate)) {
            Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R000");
        } else {
            TRACE_DEBUG("[TRALG] Say Pt R001");
        }
    } else if ((curr_mano == 1 && sum_points > 22 && maxpoints >= 12) ||
               (maxpoints >= 12 && m_iNumManiWon == 1)) {
        if (SDL_rand(40) > 32) {
            if (!ChiamaDiPiu(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R002");
            } else {
                TRACE_DEBUG("[TRALG] Say Pt R003");
            }
        } else {
            if (!m_bIamCalledPoints && SDL_rand(35) > 28) {
                if (!ChiamaAMonte(lastNumChiamate)) {
                    Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
                    TRACE_DEBUG("[TRALG] Say Pt R004");
                } else {
                    TRACE_DEBUG("[TRALG] Say Pt R005");
                }
            } else {
                Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R006");
            }
        }
    } else if (m_iPlayerOnTurn != m_iMyIndex && pointsFirstCard < 10 &&
               curr_mano == 3) {
        if (SDL_rand(2) == 1) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R007");
            } else {
                TRACE_DEBUG("[TRALG] Say Pt R008");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R009");
        }
    } else if (curr_mano > 1 && m_iNumManiWon == 0 &&
               m_iPlayerOnTurn == m_iMyIndex && pointsFirstCard > maxpoints) {
        // persa a tutte le maniere
        if (SDL_rand(2) == 1) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R010");
            } else {
                TRACE_DEBUG("[TRALG] Say Pt R011");
            }
        } else if (SDL_rand(40) > 36) {
            // bluff spudorato
            if (!ChiamaDiPiu(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R012");
            } else {
                TRACE_DEBUG("[TRALG] Say Pt R013");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R014");
        }
    } else if (pointsFirstCard < maxpoints && maxpoints >= 12) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say Pt R015");
    } else if (pointsFirstCard == 11 && curr_mano == 1 &&
               m_iPlayerOnTurn == m_iOppIndex && maxpoints <= 11) {
        if (SDL_rand(10) > 2) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R016");
            } else {
                TRACE_DEBUG("[TRALG] Say Pt R017");
            }
        } else {
            Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R018");
        }
    } else if (curr_mano == 1 && pointsFirstCard >= 12 &&
               m_iOppIndex == m_iPlayerOnTurn) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say Pt R019");
    } else if (curr_mano == 2 && maxpoints >= 12 && m_iNumManiWon >= 1) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say Pt R020");
    } else if (m_bLastManoPatada &&
               ((pointsFirstCard < 10 && m_iOppIndex == m_iPlayerOnTurn) ||
                maxpoints < 10)) {
        if (!ChiamaAMonte(lastNumChiamate)) {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R021");
        } else {
            TRACE_DEBUG("[TRALG] Say Pt R022");
        }
    } else if (m_iNumManiWon == 1 && pointsFirstCard >= 12 &&
               m_iOppIndex == m_iPlayerOnTurn) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say Pt R023");
    } else if (m_iNumManiWon == 1 &&
               (pointsFirstCard < maxpoints ||
                (m_arrIxPlayerWonHand[0] == m_iMyIndex &&
                 pointsFirstCard == maxpoints)) &&
               m_iMyIndex == m_iPlayerOnTurn) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say Pt R024");
    } else if (m_bLastManoPatada && pointsFirstCard <= maxpoints &&
               m_iMyIndex == m_iPlayerOnTurn) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say Pt R025");
    } else if (curr_mano == 1 && maxpoints < 11 && pointsFirstCard == -1) {
        if (SDL_rand(10) > 4) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R026");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R027");
        }
    } else if (curr_mano == 1 && maxpoints <= pointsFirstCard &&
               pointsFirstCard < 11 && m_iPlayerOnTurn == m_iOppIndex) {
        if (SDL_rand(10) > 4) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R028");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R029");
        }
    } else if (curr_mano == 2 &&
               (maxpoints < pointsFirstCard ||
                (pointsFirstCard == -1 && maxpoints < 11)) &&
               m_iNumManiWon == 0) {
        if (SDL_rand(10) > 4) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R030");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R031");
        }
    } else if ((m_iNumManiWon == 1 || m_bLastManoPatada) &&
               m_bIamCalledPoints) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say Pt R032");
    } else if (curr_mano == 1 && pointsFirstCard == -1 && maxpoints >= 30) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say Pt R033");
    } else if (curr_mano == 2 && m_iNumManiWon == 0 && maxpoints < 12 &&
               m_iPlayerOnTurn == m_iOppIndex) {
        if (!ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG] Say Pt R034");
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
        } else {
            TRACE_DEBUG("[TRALG] Say Pt R035");
        }
    } else if (curr_mano == 1 && maxpoints < 12 && pointsFirstCard < 12 &&
               m_iPlayerOnTurn == m_iOppIndex) {
        if (!ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG] Say Pt R036");
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
        } else {
            TRACE_DEBUG("[TRALG] Say Pt R037");
        }
    } else if (curr_mano == 3 && m_iPlayerOnTurn == m_iOppIndex &&
               ((pointsFirstCard < 11 && !m_WonFirstHand) ||
                (pointsFirstCard < 12 && m_WonFirstHand))) {
        if (!ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG] Say Pt R038");
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
        } else {
            TRACE_DEBUG("[TRALG] Say Pt R039");
        }
    } else {
        TRACE_DEBUG(
            "[TRALG] unhandled response rule: curr_mano %d, maxpoints: %d, "
            "pointsFirstCard %d, m_iPlayerOnTurn %d, prima vinta %d, mani "
            "vinte %d \n",
            curr_mano, maxpoints, pointsFirstCard, m_iPlayerOnTurn,
            m_WonFirstHand, m_iNumManiWon);
        if (SDL_rand(10) > 5) {
            Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R040");
        } else if (SDL_rand(10) > 4) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
                TRACE_DEBUG("[TRALG] Say Pt R041");
            } else {
                TRACE_DEBUG("[TRALG] Say Pt R042");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say Pt R043");
        }
    }
}

void AlgAdvancedPlayer::handleSayAmonte(int curr_mano, int pointsFirstCard,
                                        int lastNumChiamate, int maxpoints,
                                        int sum_points) {
    if (curr_mano == 1 && pointsFirstCard == 13 &&
        m_iPlayerOnTurn != m_iMyIndex) {
        Chiama(eSayPlayer::SP_NO, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRisp R20No");
    } else if (pointsFirstCard > maxpoints && m_iNumManiWon == 0 &&
               m_iPlayerOnTurn == m_iMyIndex) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRisp R13");
    } else if (((maxpoints < 10 && m_iNumManiWon == 0) ||
                (sum_points < 15 && m_iNumManiWon == 0)) &&
               m_iPlayerOnTurn == m_iMyIndex) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRisp R14");
    } else if ((m_iPlayerOnTurn != m_iMyIndex) && (curr_mano == 3) &&
               (pointsFirstCard < 10) && (maxpoints == 0)) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say Va bene carta bassa giocata terza mano");
    } else if (curr_mano == 3 && pointsFirstCard == -1 && maxpoints < 9) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRisp R15A");
    } else if (m_iNumManiWon == 1 && m_iPlayerOnTurn == m_iMyIndex &&
               pointsFirstCard != -1 && maxpoints > pointsFirstCard) {
        Chiama(eSayPlayer::SP_NO, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRisp R15D");
    } else if (m_iNumManiWon == 1 && m_iPlayerOnTurn == m_iMyIndex &&
               pointsFirstCard != -1 && maxpoints == pointsFirstCard &&
               m_arrIxPlayerWonHand[0] == m_iMyIndex) {
        Chiama(eSayPlayer::SP_NO, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRispv R15B");
    } else if (m_iNumManiWon == 1 && maxpoints < 10 &&
               m_iPlayerOnTurn == m_iMyIndex) {
        if (SDL_rand(20) > 15) {
            Chiama(eSayPlayer::SP_NO, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say MonteRisp R15C");
        } else {
            Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
            TRACE_DEBUG("[TRALG] Say MonteRisp R16");
        }
    } else if (curr_mano == 1 && sum_points < 23 && pointsFirstCard == -1) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRisp R16B");
    } else if (m_bLastManoPatada &&
               ((pointsFirstCard < 10 && m_iOppIndex == m_iPlayerOnTurn) ||
                maxpoints < 10)) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRisp R16C");
    } else if (curr_mano > 1 && m_iNumManiWon == 0 && pointsFirstCard == -1 &&
               maxpoints < 12 && m_eScoreCurrent >= SC_INVIDO) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRisp R16D");
    } else {
        Chiama(eSayPlayer::SP_NO, lastNumChiamate);
        TRACE_DEBUG("[TRALG] Say MonteRisp R17");
    }
}

void AlgAdvancedPlayer::ALG_GicataScoreChange(eGiocataScoreState eNewScore) {
    m_eScoreCurrent = eNewScore;
    if (m_iMyIndex == 0) {
        TRACE_DEBUG("[TRALG]Score changed to %d\n", eNewScore);
    }
}

void AlgAdvancedPlayer::ALG_PlayerSaidWrong(int iPlayerIx) {}
