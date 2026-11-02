

#include "AlgPlayer.h"

#include <map>

#include "Config.h"
#include "InvidoCoreDef.h"
#include "TraceService.h"

using namespace invido;

typedef std::map<eSayPlayer, LPCSTR> _MAP_SAY;
static _MAP_SAY g_MapSay;

AlgPlayer::AlgPlayer() {
    _currentManoIndex = 0;
    _p_CoreGame = NULL;
    _opponentIndex = NOT_VALID_INDEX;
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

AlgPlayer::~AlgPlayer() {}

void AlgPlayer::ALG_SetCoreInterface(I_CORE_Game* pCore) {
    SDL_assert(pCore);
    _p_CoreGame = pCore;
}

void AlgPlayer::ALG_SetPlayerIndex(Uint8 playerIx) { _myIndex = playerIx; }

void AlgPlayer::ALG_SetOpponentIndex(Uint8 playerIx) {
    _opponentIndex = playerIx;
}
void AlgPlayer::ALG_SetAssociateIndex(Uint8 playerIx) {}

void AlgPlayer::ALG_NewMatch(int iNumPlayer) {}

void AlgPlayer::ALG_NewGiocata(const VCT_CARDSPEC& vctCards, Uint8 playerIx) {
    SDL_assert(vctCards.size() == NUM_CARDS_HAND);
    int i;
    _numManiWon = 0;
    _numAmonteCalls = 0;
    _numCallsInGiocata = 0;
    _hasCalledPoints = false;
    _opponentWentDentro = false;
    _wonFirstHand = false;
    _currentManoIndex = 0;

    _playerOnTurn = playerIx;
    _cpuCardDentroPosition = NOT_VALID_INDEX;
    _vctMyCards.resize(NUM_CARDS_HAND);

    TRACE_DEBUG("[TRALG - pl %d]Giocata new:", _myIndex);
    for (i = 0; i < NUM_CARDS_HAND; i++) {
        CardSpec card = vctCards[i];
        int ix = card.GetCardIndex();
        _vctMyCards[i].SetCardIndex(ix);
        _playerWonHandIndices[i] = -1;
        TRACE_DEBUG("%d , carta: %s ", i, card.GetName().c_str());
    }
    TRACE_DEBUG("\n");
    for (i = 0; i < NUM_HANDS; i++) {
        _vctPlayedCards[i].clear();
    }

    _myLastSay = eSayPlayer::SP_VABENE;
    _myResponseSay = eSayPlayer::SP_VABENE;
    _opponentSay = eSayPlayer::SP_VABENE;
    _opponentResponseSay = eSayPlayer::SP_VABENE;
    _lastManoPatada = false;
}

void AlgPlayer::ALG_PlayerHasVadoDentro(Uint8 playerIx) {
    TRACE_DEBUG("[TRALG - pl %d]Player %1 va dentro\n", _myIndex, playerIx);
    CardSpec cardDentro;
    if (playerIx == _myIndex) {
        // l'idea è quella di ritrovare la carta con la quale si è andati dentro
        // e poi di chiamare ALG_PlayerHasPlayed
        SDL_assert(_cpuCardDentroPosition != eGameConst::NOT_VALID_INDEX);
        cardDentro.SetCardIndex(
            _vctMyCards[_cpuCardDentroPosition].GetCardIndex());
        ALG_PlayerHasPlayed(playerIx, cardDentro);
        _cpuCardDentroPosition = eGameConst::NOT_VALID_INDEX;
    } else {
        _opponentWentDentro = true;
        cardDentro.SetCardIndex(3);
        ALG_PlayerHasPlayed(playerIx, cardDentro);
    }
}

void AlgPlayer::doVadoDentro(int cardPos) {
    SDL_assert(cardPos >= 0 && cardPos < _vctMyCards.size());
    _cpuCardDentroPosition = cardPos;
    SDL_assert(_vctMyCards[cardPos].GetCardIndex() !=
               eGameConst::NOT_VALID_INDEX);
    _p_CoreGame->VaDentro(_myIndex, _vctMyCards[cardPos]);
}

void AlgPlayer::ALG_PlayerHasPlayed(Uint8 playerIx, const CardSpec& cardSpec) {
    TRACE_DEBUG("[TRALG - pl %d]Player %d has played [%s]\n", _myIndex,
                playerIx, cardSpec.GetName().c_str());
    int ix = cardSpec.GetCardIndex();
    if (playerIx == _myIndex) {
        bool bFound = false;
        for (auto cardIt = _vctMyCards.begin(); cardIt != _vctMyCards.end();
             ++cardIt) {
            if (ix == cardIt->GetCardIndex()) {
                _vctMyCards.erase(cardIt);
                bFound = true;
                break;
            }
        }
        SDL_assert(bFound);
    } else {
        _opponentSay = eSayPlayer::SP_VABENE;
    }

    CardSpec cardPlayed;
    cardPlayed.SetCardIndex(ix);
    _vctPlayedCards[_currentManoIndex].push_back(cardPlayed);

    _playerOnTurn = playerIx == 0 ? 1 : 0;
    _numAmonteCalls = 0;
    _myResponseSay = eSayPlayer::SP_VABENE;
    TRACE_DEBUG(
        "[TRALG - pl %d]Cards in hand [%d], mum of played %d on Mano "
        "%d/2\n",
        _myIndex, _vctMyCards.size(), _vctPlayedCards[_currentManoIndex].size(),
        _currentManoIndex);
}

void AlgPlayer::ALG_PlayerHasSaid(Uint8 playerIx, eSayPlayer eSay) {
    LPCSTR lpsNameSay = g_MapSay[eSay];
    TRACE_DEBUG("[TRALG - pl %d]Player %d has said %s\n", _myIndex, playerIx,
                lpsNameSay);
    if (playerIx == _opponentIndex) {
        // Avversario ha parlato
        if (eSay > eSayPlayer::SP_PARTIDA) {
            _opponentResponseSay = eSay;  // Risposta alla chiamata
        } else {
            // avversario ha chiamato dei punti
            _opponentSay = eSay;
        }
    } else if (playerIx == _myIndex) {
        // Algoritmo ha parlato
        if (eSay > eSayPlayer::SP_PARTIDA) {
            _myResponseSay = eSay;  // Risposta alla chiamata
        } else {
            // chiamata a punti
            _myLastSay = eSay;
            _hasCalledPoints = true;
        }
    }
}

bool AlgPlayer::Cagna(int lastNumChiamate) {
    VCT_COMMANDS vct_cmd;
    SDL_assert(_p_CoreGame);
    bool res = false;
    _p_CoreGame->GetAdmittedCommands(vct_cmd, _myIndex);
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

bool AlgPlayer::ChiamaAMonte(int lastNumChiamate) {
    _numAmonteCalls += 1;
    if (_numAmonteCalls < 4) {
        Chiama(eSayPlayer::SP_AMONTE, lastNumChiamate);
        return true;
    }
    return false;
}

void AlgPlayer::Chiama(eSayPlayer eSay, int lastChiamataNum) {
    SDL_assert(lastChiamataNum == _numCallsInGiocata);
    _numCallsInGiocata += 1;
    _p_CoreGame->Say(_myIndex, eSay);
}

bool AlgPlayer::ChiamaDiPiu(int lastNumChiamate) {
    VCT_COMMANDS vct_cmd;
    SDL_assert(_p_CoreGame);

    _p_CoreGame->GetAdmittedCommands(vct_cmd, _myIndex);
    for (size_t i = 0; i < vct_cmd.size(); i++) {
        eSayPlayer eSay = vct_cmd[i];
        if (eSay >= eSayPlayer::SP_INVIDO && eSay <= eSayPlayer::SP_PARTIDA) {
            Chiama(eSay, lastNumChiamate);
            return true;
        }
    }
    return false;
}

void AlgPlayer::PlayAsFirst() {
    int lastNumChiamate = _numCallsInGiocata;
    int curr_mano = NumMano();
    if (SDL_rand(40) >= 35 && !_hasCalledPoints) {
        if (Cagna(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PLF_R1cagna\n", _myIndex);
            return;
        }
    }

    int i = 0, maxpoints = 0, max_pos = 0, sum_points = 0, min_pos = 0;
    int min_points = 20;
    int med_points = 0, med_pos = 0;
    for (const CardSpec& card : _vctMyCards) {
        int points = card.GetPoints();
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
        sum_points += points;
        i++;
    }
    CardSpec result;
    bool hasCardResult = true;
    TRACE_DEBUG(
        "[1ST]Take: min pos %d (pt %d), med pos %d (pt %d), max pos %d(pt %d) "
        "\n",
        min_pos, min_points, med_pos, med_points, max_pos, maxpoints);

    if (curr_mano == 2 && _numManiWon == 1) {
        result = _vctMyCards[min_pos];
        TRACE_DEBUG("Candidate from min position\n");
    } else {
        result = _vctMyCards[max_pos];
    }
    TRACE_DEBUG("[TRALG - pl %d]candidate play PLF_pos: %s\n", _myIndex,
                result.GetName().c_str());

    if (curr_mano == 2 && _numManiWon == 1) {
        int pointsCardMano1_pl1 = _vctPlayedCards[0][0].GetPoints();
        int pointsCardMano1_pl2 = _vctPlayedCards[0][1].GetPoints();
        int maxPointsMano1 = pointsCardMano1_pl1 > pointsCardMano1_pl2
                                 ? pointsCardMano1_pl1
                                 : pointsCardMano1_pl2;

        // seconda mano prima mano vinta
        TRACE_DEBUG("[TRALG - pl %d]Vado dentro? maxpoints is %d\n", _myIndex,
                    maxpoints);
        if (maxpoints == 13) {
            // tre in mano che vince sicuro
            if (SDL_rand(10) != 9) {
                TRACE_DEBUG("[TRALG - pl %d]Dentro per win\n", _myIndex);
                doVadoDentro(min_pos);
                return;
            }
        } else if (maxpoints > 10) {
            if (SDL_rand(maxpoints) > 9) {
                TRACE_DEBUG("[TRALG - pl %d]Dentro per win con rischio\n",
                            _myIndex);
                doVadoDentro(min_pos);
                return;
            }
        } else if (min_points < maxPointsMano1) {
            TRACE_DEBUG("[TRALG - pl %d]Dentro per bluf\n", _myIndex);
            doVadoDentro(min_pos);
            return;
        } else if (SDL_rand(3) == 1 && _currentScore <= SC_INVIDO) {
            if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PLF_R5_primavinta\n", _myIndex);
                hasCardResult = false;
            }
        }
        if (hasCardResult) {
            result = _vctMyCards[min_pos];
            TRACE_DEBUG("[TRALG - pl %d]PLF_cand_min_pos: %s\n", _myIndex,
                        result.GetName().c_str());
        }
    } else if (_numManiWon == 1 && maxpoints >= 12 &&
               _playerWonHandIndices[0] == _myIndex) {
        if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PLF_R1bisFortePrimamia\n", _myIndex);
            hasCardResult = false;
        }
    } else if (curr_mano == 1 && maxpoints >= 12 && med_points < 10 &&
               med_points > 0) {
        result = _vctMyCards[med_pos];
        TRACE_DEBUG("[TRALG - pl %d]PLF_cand_med_pos: %s\n", _myIndex,
                    result.GetName().c_str());
    } else if (curr_mano == 1 && maxpoints >= 11 && med_points < 10) {
        if (SDL_rand(10) > 5) {
            result = _vctMyCards[med_pos];
            TRACE_DEBUG("[TRALG - pl %d]PLF_cand_med_pos Brnd: %s\n", _myIndex,
                        result.GetName().c_str());
        }
    } else if (_numManiWon == 1 && maxpoints >= 12 &&
               _currentScore == SC_CANELA) {
        if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PLF_R1Bvintauna\n", _myIndex);
            hasCardResult = false;
        }
    } else if (_lastManoPatada) {
        if (maxpoints == 13) {
            if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PLF_R2A_manopatada\n", _myIndex);
                hasCardResult = false;
            }
        } else if (maxpoints <= 10) {
            if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PLF_R2B_manopatada\n", _myIndex);
                hasCardResult = false;
            }
        } else if ((maxpoints == 12 || maxpoints == 11) &&
                   _currentScore <= SC_INVIDO) {
            if (SDL_rand(30) < maxpoints && hasCardResult &&
                ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PLF_R2C_manopatada\n", _myIndex);
                hasCardResult = false;
            }
        }
    } else if (maxpoints == 13 && sum_points > 21 && SDL_rand(10) > 6 &&
               _currentScore <= SC_INVIDO) {
        if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PLF_R3_tre\n", _myIndex);
            hasCardResult = false;
        }
    } else if (sum_points < 15 && curr_mano == 1 && SDL_rand(2) == 1) {
        // mano scarsa
        if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PLF_R4_poco\n", _myIndex);
            hasCardResult = false;
        }
    } else if (curr_mano == 3 && _playerWonHandIndices[0] == _myIndex) {
        // terza mano prima mia
        if (maxpoints == 13) {
            if (SDL_rand(20) > 16) {
                // bluff
                if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                    hasCardResult = false;
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R7_bluff_coltre\n",
                                _myIndex);
                }
            } else {
                if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
                    hasCardResult = false;
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R8_tre\n", _myIndex);
                }
            }
        } else if (maxpoints == 12) {
            if (SDL_rand(20) > 14) {
                if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R9_bluffcoldue\n",
                                _myIndex);
                    hasCardResult = false;
                }
            } else if (SDL_rand(2) == 1 && _currentScore <= SC_INVIDO) {
                if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R10_due\n", _myIndex);
                    hasCardResult = false;
                }
            }
        } else if (maxpoints == 10 || maxpoints == 9) {
            if (SDL_rand(20) > 11) {
                if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R11_poco\n", _myIndex);
                    hasCardResult = false;
                }
            } else if (SDL_rand(20) > 15 && _currentScore <= SC_INVIDO) {
                if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R12_bluffconpoco\n",
                                _myIndex);
                    hasCardResult = false;
                }
            }
        } else {
            if (SDL_rand(20) > 7) {
                if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R13_amonte\n", _myIndex);
                    hasCardResult = false;
                }
            }
        }

    } else if (curr_mano == 3 && _playerWonHandIndices[0] == _opponentIndex) {
        // terza mano, prima vinta dall'avversario
        if (maxpoints == 13) {
            if (SDL_rand(20) > 15) {
                // bluff
                if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R14_bluffcoltre\n",
                                _myIndex);
                    hasCardResult = false;
                }
            } else if (SDL_rand(20) > 9 && _currentScore <= SC_INVIDO) {
                if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R15_tre\n", _myIndex);
                    hasCardResult = false;
                }
            }
        } else if (maxpoints == 12) {
            if (SDL_rand(20) > 12) {
                if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R16_montecoldue\n",
                                _myIndex);
                    hasCardResult = false;
                }
            } else if (SDL_rand(3) == 1) {
                if (hasCardResult && ChiamaDiPiu(lastNumChiamate) &&
                    _currentScore <= SC_INVIDO) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R17_rilanciodue\n",
                                _myIndex);
                    hasCardResult = false;
                }
            }
        } else if (maxpoints == 10 || maxpoints == 9) {
            if (SDL_rand(20) > 9) {
                if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R17_montecolre\n",
                                _myIndex);
                    hasCardResult = false;
                }
            } else if (SDL_rand(20) > 17) {
                if (hasCardResult && ChiamaDiPiu(lastNumChiamate) &&
                    _currentScore <= SC_INVIDO) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R18_rilanciocolre\n",
                                _myIndex);
                    hasCardResult = false;
                }
            }
        } else {
            if (SDL_rand(20) > 5) {
                if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                    TRACE_DEBUG("[TRALG - pl %d]PLF_R19_monte\n", _myIndex);
                    hasCardResult = false;
                }
            }
        }
    }

    if (SDL_rand(40) > 37) {
        // chiamata a monte rara
        if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PLF_R20_monteraro\n", _myIndex);
            hasCardResult = false;
        }
    }

    if (lastNumChiamate == _numCallsInGiocata && !hasCardResult) {
        SDL_assert(0);
        TRACE_DEBUG("[TRALG - pl %d]PLF_R21_giocaacaso\n", _myIndex);
        GiocaACaso();
    }

    if (hasCardResult) {
        SDL_assert(_p_CoreGame);
        _p_CoreGame->PlayCard(_myIndex, result);
    } else {
        TRACE_DEBUG("[TRALG - pl %d]PLF non gioca carta per chiamata\n",
                    _myIndex);
    }
}

void AlgPlayer::GiocaACaso() {
    SDL_assert(_vctMyCards.size() > 0);
    int iCartaPos = SDL_rand(_vctMyCards.size());
    SDL_assert(_p_CoreGame);
    _p_CoreGame->PlayCard(_myIndex, _vctMyCards[iCartaPos]);
}

void AlgPlayer::PlayAsSecond() {
    SDL_assert(_vctMyCards.size() > 0);
    CardSpec result;
    bool hasCardResult = true;
    SDL_assert(_vctPlayedCards[_currentManoIndex].size() == 1);
    CardSpec cardPlayed = _vctPlayedCards[_currentManoIndex].front();

    int pointsFirstCard = cardPlayed.GetPoints();
    int curr_mano = NumMano();
    int lastNumChiamate = _numCallsInGiocata;

    int i = 0, maxpoints = 0, max_pos = 0, min_pos = 0;
    int min_points = 20, same_points_pos = -1, first_take_pos = -1,
        first_take_points = 20;
    for (const CardSpec& card : _vctMyCards) {
        int points = card.GetPoints();
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
            first_take_pos = i;  // La carta più bassa che può prendere
            first_take_points = points;
        }
        i++;
    }
    TRACE_DEBUG(
        "[2ND]Points played card %d, Take: min pos %d (pt %d), first_take_pos "
        "%d (pt %d), max pos %d(pt %d) \n",
        pointsFirstCard, min_pos, min_points, first_take_pos, first_take_points,
        max_pos, maxpoints);
    if (_opponentWentDentro) {
        // Avversario è  andato dentro, quindi si gioca la carta più bassa che
        // si vince la mano
        result = _vctMyCards[min_pos];
        TRACE_DEBUG("[TRALG - pl %d]PL2nd_min_vadodentro: %s\n", _myIndex,
                    result.GetName().c_str());
    } else if (first_take_pos != -1) {
        // si prende
        result = _vctMyCards[first_take_pos];
        TRACE_DEBUG("[TRALG - pl %d]PL2nd_cand_first_take_pos: %s\n", _myIndex,
                    result.GetName().c_str());
    } else if (same_points_pos != -1) {
        // poi se patta
        result = _vctMyCards[same_points_pos];
        TRACE_DEBUG("[TRALG - pl %d]PL2nd_cand_same_pos: %s\n", _myIndex,
                    result.GetName().c_str());
    } else {
        // perdo, quindi gioca la carta pi bassa
        result = _vctMyCards[min_pos];
        TRACE_DEBUG("[TRALG - pl %d]PL2nd_cand_min_pos: %s\n", _myIndex,
                    result.GetName().c_str());
    }

    if (_opponentWentDentro) {
        // play the min card and say nothing
    } else if (pointsFirstCard > maxpoints && _numManiWon == 0 &&
               curr_mano > 1) {
        if (SDL_rand(20) < 16) {
            if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R1_monte\n", _myIndex);
                hasCardResult = false;
            }
        }
    } else if (curr_mano == 3 && pointsFirstCard == maxpoints &&
               pointsFirstCard == 13 && _playerWonHandIndices[0] == _myIndex) {
        if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PL2nd_R2_megacagna\n", _myIndex);
            hasCardResult = false;
        } else if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PL2nd_R2C_piu\n", _myIndex);
            hasCardResult = false;
        }

    } else if (first_take_pos == -1 && _numManiWon == 0 && curr_mano > 1) {
        if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PL2nd_R1B_perdosempre\n", _myIndex);
            hasCardResult = false;
        }
    } else if (first_take_pos != -1 && _lastManoPatada) {
        int rest = pointsFirstCard >= 12 ? 15 : 35;
        if (SDL_rand(40) > rest) {
            if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R2A_monte_cagna\n", _myIndex);
                hasCardResult = false;
            }
        } else {
            if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R2B_chiama\n", _myIndex);
                hasCardResult = false;
            }
        }
    } else if (curr_mano == 3 && same_points_pos != -1 && _numManiWon == 1 &&
               _wonFirstHand) {
        // terza posso pattare e la prima è mia, si vice sicuro, quindi chiama
        // di più
        if ((SDL_rand(30 - pointsFirstCard) > 3) || (_myLastSay == SP_AMONTE)) {
            if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R3B_pata_chiama\n", _myIndex);
                hasCardResult = false;
            }
        } else {
            if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R3B_pata_cagna\n", _myIndex);
                hasCardResult = false;
            }
        }
    } else if (curr_mano == 3 && first_take_pos != -1 && _numManiWon == 1) {
        if (SDL_rand(40) > 35 && pointsFirstCard >= 10 &&
            _currentScore <= SC_TRASMAS) {
            if (hasCardResult && ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R2B_monte_cagna\n", _myIndex);
                hasCardResult = false;
            }
        } else {
            if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R2B_chiama\n", _myIndex);
                hasCardResult = false;
            }
        }
    } else if (curr_mano == 1 && first_take_pos != -1 &&
               pointsFirstCard <= 11) {
        if (hasCardResult && ChiamaDiPiu(lastNumChiamate)) {
            TRACE_DEBUG("[TRALG - pl %d]PL2nd_R3A_chiama\n", _myIndex);
            hasCardResult = false;
        }
    } else if (same_points_pos != -1 && maxpoints >= 12) {
        result = _vctMyCards[same_points_pos];
        TRACE_DEBUG("[TRALG - pl %d]PL2nd_cand_same_points_pos: %s\n", _myIndex,
                    result.GetName().c_str());
        if (SDL_rand(20) > 5) {
            if (ChiamaDiPiu(lastNumChiamate)) {
                hasCardResult = false;
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R4A_dipiu\n", _myIndex);
            }
        } else if (SDL_rand(20) > 17) {
            if (ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R4B_monte\n", _myIndex);
                hasCardResult = false;
            }
        }
    } else if (pointsFirstCard > maxpoints) {
        // si perde la mano
        if (SDL_rand(20) > 15 || (curr_mano == 3 && _numManiWon == 1) ||
            (curr_mano == 2 && _numManiWon == 0)) {
            if (ChiamaAMonte(lastNumChiamate)) {
                TRACE_DEBUG("[TRALG - pl %d]PL2nd_R5_monte\n", _myIndex);
                hasCardResult = false;
            }
        }
        if (hasCardResult && curr_mano > 1) {
            // Vado dentro
            TRACE_DEBUG(
                "[TRALG - pl %d]PL2nd non prendo, quindi dentro per vergona\n",
                _myIndex);
            doVadoDentro(min_pos);
            return;
        }
    } else if (lastNumChiamate == _numCallsInGiocata && !hasCardResult) {
        TRACE_DEBUG("[TRALG - pl %d]PL2nd_R5_acaso\n", _myIndex);
        SDL_assert(0);
        GiocaACaso();
    }

    if (hasCardResult) {
        _p_CoreGame->PlayCard(_myIndex, result);
    } else {
        TRACE_DEBUG("[TRALG - pl %d]PL2nd non gioca per chiamata pending\n",
                    _myIndex);
    }
}

bool AlgPlayer::IsPlayerFirst() {
    return _vctPlayedCards[_currentManoIndex].size() == 0 ? true : false;
}

// Ritorna: 1,2,3 a seconda della mano
int AlgPlayer::NumMano() { return _currentManoIndex + 1; }

LPErrInApp AlgPlayer::ALG_HaveToPlay() {
    if (IsPlayerFirst()) {
        PlayAsFirst();
    } else {
        PlayAsSecond();
    }
    return NULL;
}

void AlgPlayer::ALG_ManoEnd(I_MatchScore* pScore) {
    _opponentWentDentro = false;
    _currentManoIndex++;

    _lastManoPatada = pScore->IsManoPatada();
    int index = pScore->GetManoRound() - 1;
    SDL_assert(index >= 0 && index < NUM_CARDS_HAND);
    _playerWonHandIndices[index] =
        _lastManoPatada ? -2 : pScore->GetManoWinner();
    _numAmonteCalls = 0;
    if (pScore->GetManoWinner() == _myIndex && !pScore->IsManoPatada()) {
        if (index == 0 || (index == 1 && _lastManoPatada)) {
            _wonFirstHand = true;
        }
        _numManiWon += 1;
    }
}

void AlgPlayer::ALG_GiocataEnd(I_MatchScore* pScore) {
    if (_myIndex == 0) {
        bool bIsPata = pScore->IsGiocataPatada();
        if (bIsPata) {
            // giocata patada
            TRACE_DEBUG("[ALG_GiocataEnd]Giocata patada\n");
        } else if (pScore->IsGiocataMonte()) {
            // giocata a monte
            TRACE_DEBUG("[ALG_GiocataEnd]Giocata a monte\n");
        } else {
            // winner
            int iPlWinner = pScore->GetGiocataWinner();

            TRACE_DEBUG("[ALG_GiocataEnd]Giocata vinta da %d\n", iPlWinner);
            TRACE_DEBUG(
                "[ALG_GiocataEnd]Punteggio player 0: %d, punteggio player 1: "
                "%d\n",
                pScore->GetPointsPlayer(_myIndex),
                pScore->GetPointsPlayer(_opponentIndex));
        }
    }
}

void AlgPlayer::ALG_MatchEnd(I_MatchScore* pScore) {
    int iPlWinner = pScore->GetMatchWinner();
    TRACE_DEBUG("[ALG_MatchEnd]Match vinto da %d\n", iPlWinner);
    TRACE_DEBUG(
        "[ALG_MatchEnd]Punteggio player 0: %d, punteggio player 1: %d\n",
        pScore->GetPointsPlayer(_myIndex),
        pScore->GetPointsPlayer(_opponentIndex));
}

LPErrInApp AlgPlayer::ALG_HaveToRespond() {
    TRACE_DEBUG("[ALG_HaveToRespond] have to respond");
    int lastNumChiamate = _numCallsInGiocata;
    if (_opponentSay == SP_AMONTE && _myResponseSay == eSayPlayer::SP_NO) {
        // a monte was called and it was responded NO, don't change idea
        Chiama(_myResponseSay, lastNumChiamate);
        TRACE_DEBUG("[ALG_HaveToRespond] Say R1");
        return NULL;
    }

    size_t iRndIndex;
    VCT_COMMANDS vct_cmd;
    SDL_assert(_p_CoreGame);

    int pointsFirstCard = -1;
    if (_vctPlayedCards[_currentManoIndex].size() > 0) {
        pointsFirstCard = _vctPlayedCards[_currentManoIndex][0].GetPoints();
    }

    _p_CoreGame->GetAdmittedCommands(vct_cmd, _myIndex);
    size_t iNumCmds = vct_cmd.size();
    int maxpoints = 0, sum_points = 0;
    int min_points = 20;
    for (const CardSpec& card : _vctMyCards) {
        int points = card.GetPoints();
        if (points > maxpoints) {
            maxpoints = points;
        }
        if (points < min_points) {
            min_points = points;
        }
        sum_points += points;
    }
    int curr_mano = NumMano();
    TRACE_DEBUG(
        "[ALG_HaveToRespond] Say: points first card %d, max points %d \n",
        pointsFirstCard, maxpoints);

    if (_opponentSay >= eSayPlayer::SP_INVIDO &&
        _opponentSay <= eSayPlayer::SP_PARTIDA) {
        handleSayPopints(curr_mano, pointsFirstCard, lastNumChiamate, maxpoints,
                         sum_points);
    } else if (_opponentSay == SP_AMONTE) {
        handleSayAmonte(curr_mano, pointsFirstCard, lastNumChiamate, maxpoints,
                        sum_points);
    } else {
        iRndIndex = SDL_rand(iNumCmds);
        Chiama(vct_cmd[iRndIndex], lastNumChiamate);
        TRACE_DEBUG("[ALG_HaveToRespond] Say R18");
    }
    return NULL;
}

void AlgPlayer::handleSayPopints(int curr_mano, int pointsFirstCard,
                                 int lastNumChiamate, int maxpoints,
                                 int sum_points) {
    if (maxpoints == 13 && _lastManoPatada) {
        if (!ChiamaDiPiu(lastNumChiamate)) {
            Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R000");
        } else {
            TRACE_DEBUG("[handleSayPopints] Say Pt R001");
        }
    } else if ((curr_mano == 1 && sum_points > 22 && maxpoints >= 12) ||
               (maxpoints >= 12 && _numManiWon == 1)) {
        if (SDL_rand(40) > 32) {
            if (!ChiamaDiPiu(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R002");
            } else {
                TRACE_DEBUG("[handleSayPopints] Say Pt R003");
            }
        } else {
            if (!_hasCalledPoints && SDL_rand(35) > 28) {
                if (!ChiamaAMonte(lastNumChiamate)) {
                    Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
                    TRACE_DEBUG("[handleSayPopints] Say Pt R004");
                } else {
                    TRACE_DEBUG("[handleSayPopints] Say Pt R005");
                }
            } else {
                Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R006");
            }
        }
    } else if (_playerOnTurn != _myIndex && pointsFirstCard < 10 &&
               curr_mano == 3) {
        if (SDL_rand(2) == 1) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R007");
            } else {
                TRACE_DEBUG("[handleSayPopints] Say Pt R008");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R009");
        }
    } else if (curr_mano > 1 && _numManiWon == 0 && _playerOnTurn == _myIndex &&
               pointsFirstCard > maxpoints) {
        // persa a tutte le maniere
        if (SDL_rand(2) == 1) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R010");
            } else {
                TRACE_DEBUG("[handleSayPopints] Say Pt R011");
            }
        } else if (SDL_rand(40) > 36) {
            // bluff spudorato
            if (!ChiamaDiPiu(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R012");
            } else {
                TRACE_DEBUG("[handleSayPopints] Say Pt R013");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R014");
        }
    } else if (pointsFirstCard < maxpoints && maxpoints >= 12) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[handleSayPopints] Say Pt R015");
    } else if (pointsFirstCard == 11 && curr_mano == 1 &&
               _playerOnTurn == _opponentIndex && maxpoints <= 11) {
        if (SDL_rand(10) > 2) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R016");
            } else {
                TRACE_DEBUG("[handleSayPopints] Say Pt R017");
            }
        } else {
            Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R018");
        }
    } else if (curr_mano == 1 && pointsFirstCard >= 12 &&
               _opponentIndex == _playerOnTurn) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[handleSayPopints] Say Pt R019");
    } else if (curr_mano == 2 && maxpoints >= 12 && _numManiWon >= 1) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[handleSayPopints] Say Pt R020");
    } else if (_lastManoPatada &&
               ((pointsFirstCard < 10 && _opponentIndex == _playerOnTurn) ||
                maxpoints < 10)) {
        if (!ChiamaAMonte(lastNumChiamate)) {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R021");
        } else {
            TRACE_DEBUG("[handleSayPopints] Say Pt R022");
        }
    } else if (_numManiWon == 1 && pointsFirstCard >= 12 &&
               _opponentIndex == _playerOnTurn) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[handleSayPopints] Say Pt R023");
    } else if (_numManiWon == 1 &&
               (pointsFirstCard < maxpoints ||
                (_playerWonHandIndices[0] == _myIndex &&
                 pointsFirstCard == maxpoints)) &&
               _myIndex == _playerOnTurn) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[handleSayPopints] Say Pt R024");
    } else if (_lastManoPatada && pointsFirstCard <= maxpoints &&
               _myIndex == _playerOnTurn) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[handleSayPopints] Say Pt R025");
    } else if (curr_mano == 1 && maxpoints < 11 && pointsFirstCard == -1) {
        if (SDL_rand(10) > 4) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R026");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R027");
        }
    } else if (curr_mano == 1 && maxpoints <= pointsFirstCard &&
               pointsFirstCard < 11 && _playerOnTurn == _opponentIndex) {
        if (SDL_rand(10) > 4) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R028");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R029");
        }
    } else if (curr_mano == 2 &&
               (maxpoints < pointsFirstCard ||
                (pointsFirstCard == -1 && maxpoints < 11)) &&
               _numManiWon == 0) {
        if (SDL_rand(10) > 4) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R030");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R031");
        }
    } else if ((_numManiWon == 1 || _lastManoPatada) && _hasCalledPoints) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[handleSayPopints] Say Pt R032");
    } else if (curr_mano == 1 && pointsFirstCard == -1 && maxpoints >= 30) {
        Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
        TRACE_DEBUG("[handleSayPopints] Say Pt R033");
    } else if (curr_mano == 2 && _numManiWon == 0 && maxpoints < 12 &&
               _playerOnTurn == _opponentIndex) {
        if (!ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[handleSayPopints] Say Pt R034");
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
        } else {
            TRACE_DEBUG("[handleSayPopints] Say Pt R035");
        }
    } else if (curr_mano == 1 && maxpoints < 12 && pointsFirstCard < 12 &&
               _playerOnTurn == _opponentIndex) {
        if (!ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[handleSayPopints] Say Pt R036");
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
        } else {
            TRACE_DEBUG("[handleSayPopints] Say Pt R037");
        }
    } else if (curr_mano == 3 && _playerOnTurn == _opponentIndex &&
               ((pointsFirstCard < 11 && !_wonFirstHand) ||
                (pointsFirstCard < 12 && _wonFirstHand))) {
        if (!ChiamaAMonte(lastNumChiamate)) {
            TRACE_DEBUG("[handleSayPopints] Say Pt R038");
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
        } else {
            TRACE_DEBUG("[handleSayPopints] Say Pt R039");
        }
    } else {
        TRACE_DEBUG(
            "[handleSayPopints] unhandled response rule: curr_mano %d, "
            "maxpoints: %d, "
            "pointsFirstCard %d, _playerOnTurn %d, prima vinta %d, mani "
            "vinte %d \n",
            curr_mano, maxpoints, pointsFirstCard, _playerOnTurn, _wonFirstHand,
            _numManiWon);
        if (SDL_rand(10) > 5) {
            Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R040");
        } else if (SDL_rand(10) > 4) {
            if (!ChiamaAMonte(lastNumChiamate)) {
                Chiama(eSayPlayer::SP_GIOCA, lastNumChiamate);
                TRACE_DEBUG("[handleSayPopints] Say Pt R041");
            } else {
                TRACE_DEBUG("[handleSayPopints] Say Pt R042");
            }
        } else {
            Chiama(eSayPlayer::SP_VADOVIA, lastNumChiamate);
            TRACE_DEBUG("[handleSayPopints] Say Pt R043");
        }
    }
}

void AlgPlayer::handleSayAmonte(int curr_mano, int pointsFirstCard,
                                int lastNumChiamate, int maxpoints,
                                int sum_points) {
    if (curr_mano == 1 && pointsFirstCard == 13 && _playerOnTurn != _myIndex) {
        Chiama(eSayPlayer::SP_NO, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R20No");
    } else if (pointsFirstCard > maxpoints && _numManiWon == 0 &&
               _playerOnTurn == _myIndex) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R13");
    } else if (((maxpoints < 10 && _numManiWon == 0) ||
                (sum_points < 15 && _numManiWon == 0)) &&
               _playerOnTurn == _myIndex) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R14");
    } else if ((_playerOnTurn != _myIndex) && (curr_mano == 3) &&
               (pointsFirstCard < 10) && (maxpoints == 0)) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG(
            "[handleSayAmonte] Say Va bene carta bassa giocata terza mano");
    } else if (curr_mano == 3 && pointsFirstCard == -1 && maxpoints < 9) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R15A");
    } else if (_numManiWon == 1 && _playerOnTurn == _myIndex &&
               pointsFirstCard != -1 && maxpoints > pointsFirstCard) {
        Chiama(eSayPlayer::SP_NO, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R15D");
    } else if (_numManiWon == 1 && _playerOnTurn == _myIndex &&
               pointsFirstCard != -1 && maxpoints == pointsFirstCard &&
               _playerWonHandIndices[0] == _myIndex) {
        Chiama(eSayPlayer::SP_NO, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRispv R15B");
    } else if (_numManiWon == 1 && maxpoints < 10 &&
               _playerOnTurn == _myIndex) {
        if (SDL_rand(20) > 15) {
            Chiama(eSayPlayer::SP_NO, lastNumChiamate);
            TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R15C");
        } else {
            Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
            TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R16");
        }
    } else if (curr_mano == 1 && sum_points < 23 && pointsFirstCard == -1) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R16B");
    } else if (_lastManoPatada &&
               ((pointsFirstCard < 10 && _opponentIndex == _playerOnTurn) ||
                maxpoints < 10)) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R16C");
    } else if (curr_mano > 1 && _numManiWon == 0 && pointsFirstCard == -1 &&
               maxpoints < 12 && _currentScore >= SC_INVIDO) {
        Chiama(eSayPlayer::SP_VABENE, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R16D");
    } else {
        Chiama(eSayPlayer::SP_NO, lastNumChiamate);
        TRACE_DEBUG("[handleSayAmonte] Say MonteRisp R17");
    }
}

void AlgPlayer::ALG_GicataScoreChange(eGiocataScoreState eNewScore) {
    _currentScore = eNewScore;
    if (_myIndex == 0) {
        TRACE_DEBUG("[ALG_GicataScoreChange]Score changed to %d\n", eNewScore);
    }
}

void AlgPlayer::ALG_PlayerSaidWrong(Uint8 playerIx) {}
