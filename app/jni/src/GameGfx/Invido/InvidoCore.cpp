#include "InvidoCore.h"

#include "Mazzo.h"
#include "TraceService.h"

using namespace invido;

InvidoCore::InvidoCore() {
    _numPlayers = 0;
    _p_PlHaveToPlay = NULL;
    _p_StartPlayer = NULL;
    _p_MyMazzo = NULL;
    _p_PlayersOnTable = NULL;
}

InvidoCore::~InvidoCore() {}

void InvidoCore::Init(PlayersOnTable* pPlayersOnTable, Mazzo* pMazzo) {
    TRACE_DEBUG("InvidoCore initialize\n");

    SDL_assert(pPlayersOnTable);
    SDL_assert(pMazzo);

    _p_MyMazzo = pMazzo;
    _p_PlayersOnTable = pPlayersOnTable;

    _eGameType = LOCAL_TYPE;

    _partita.SetCore(this);
    _partita.SetGiocata(&_giocata);

    _giocata.SetCore(this);
    _giocata.SetPartita(&_partita);
    _giocata.SetMano(&_mano);

    _mano.SetCore(this);
    _mano.SetGiocata(&_giocata);
    _mano.SetScore(&_matchPoints);

    _numPlayers = _p_PlayersOnTable->GetNumOfPlayers();

    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        _vctpAlgPlayer[i] = NULL;
    }

    _matchPoints.SetManoInstance(&_mano);
}

void InvidoCore::AddPlayer(Player& player) {
    SDL_assert(_p_PlayersOnTable);
    _p_PlayersOnTable->AddPlayer(player);
    _numPlayers = _p_PlayersOnTable->GetNumOfPlayers();
}

Uint8 InvidoCore::getNewMatchFirstPlayer() { return SDL_rand(_numPlayers); }

Player* InvidoCore::GetPlayer(Uint8 iIndex) {
    SDL_assert(_p_PlayersOnTable);
    return _p_PlayersOnTable->GetPlayerOnIndex(iIndex);
}

void InvidoCore::NewMatch() {
    NotifyScript(SCR_NFY_NEWMATCH);

    // extract the first player
    Uint8 firstPlayerIx = getNewMatchFirstPlayer();

    // save players alg in a table
    // don't change level during a match
    for (int i = 0; i < _numPlayers; i++) {
        Player* pPlayer = _p_PlayersOnTable->GetPlayerOnIndex(i);
        _vctpAlgPlayer[i] = pPlayer->GetAlg();
        if (_vctpAlgPlayer[i]) {
            // information before match begin
            _vctpAlgPlayer[i]->ALG_SetPlayerIndex(pPlayer->GetIndex());
            _vctpAlgPlayer[i]->ALG_SetCoreInterface(this);
            _vctpAlgPlayer[i]->ALG_NewMatch(_numPlayers);
            if (i == 0) {
                if (_numPlayers == 2) {
                    // invido 2 player
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(1);
                } else if (_numPlayers == 4) {
                    // invido 4 players
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(1);
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(3);
                } else {
                    SDL_assert(0);
                }
            } else if (i == 1) {
                if (_numPlayers == 2) {
                    // invido 2 player
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(0);
                } else {
                    // invido 4 players
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(0);
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(2);
                }
            } else if (i == 2) {
                _vctpAlgPlayer[i]->ALG_SetOpponentIndex(1);
                _vctpAlgPlayer[i]->ALG_SetOpponentIndex(3);
            } else if (i == 3) {
                _vctpAlgPlayer[i]->ALG_SetOpponentIndex(0);
                _vctpAlgPlayer[i]->ALG_SetOpponentIndex(2);
            } else {
                SDL_assert(0);
            }
        }
    }

    _mano.MatchStart();
    _p_PlayersOnTable->SetFirstOnMatch(firstPlayerIx);
    _p_StartPlayer = _p_PlayersOnTable->GetPlayerOnIndex(firstPlayerIx);
    _partita.NewPartita(firstPlayerIx);
    _matchPoints.MatchStart(_numPlayers);
}

void InvidoCore::NextAction() {
    _mano.NextAction();
    _giocata.NextAction();
    _partita.NextAction();
}

void InvidoCore::resetCardInfoPlayers() {
    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        _cardInfos[i].clear();
    }
}

bool InvidoCore::resetCard(Uint8 playerIx, const CardSpec& cardSpec) {
    bool bRet = false;
    SDL_assert(playerIx >= 0 && playerIx < MAX_NUM_PLAYER);

    auto it = _cardInfos[playerIx].begin();
    while (it != _cardInfos[playerIx].end()) {
        if (it->GetCardIndex() == cardSpec.GetCardIndex()) {
            it = _cardInfos[playerIx].erase(it);  // Erase and get next
        } else {
            ++it;
        }
    }

    return bRet;
}

void InvidoCore::Giocata_Start(Uint8 playerIx) {
    TRACE_DEBUG("Giocata_Start\n");

    resetCardInfoPlayers();
    // 1) first - calculate the table (there is no switch)
    // player that have to play first, is the first that get the cards after
    // shuffle

    _p_MyMazzo->Shuffle();
    NotifyScript(SCR_NFY_SHUFFLEDECK);
    _p_PlayersOnTable->SetFirstOnGiocata(playerIx);

    // 2) second - retreive the player that have to play and switch the table
    _p_PlHaveToPlay = _p_PlayersOnTable->SwitchToNextPlayer();
    Player* pCurrPlayer = _p_PlHaveToPlay;
    Uint8 currPlayerIx = pCurrPlayer->GetIndex();
    // 3) distribuite cards
    for (int i = 0; i < _numPlayers; i++) {
        if (i > 0) {
            pCurrPlayer = _p_PlayersOnTable->PeekNextPlayerToIx(currPlayerIx);
            currPlayerIx = pCurrPlayer->GetIndex();
        }

        VCT_CARDSPEC vctCardArray;

        TRACE_DEBUG("%s => ", pCurrPlayer->GetName());

        for (int j = 0; j < NUM_CARDS_HAND; j++) {
            bool isValid;
            CardSpec nextCard;
            isValid = _p_MyMazzo->PickNextCard(&nextCard);
            if (isValid) {
                vctCardArray.push_back(nextCard);  // CardSpec has no pointers,
                                                   // this is perfectly fine
                TRACE_DEBUG("[%s] , ix: %d, pt: %d", nextCard.GetName(),
                            nextCard.GetCardIndex(), nextCard.GetPoints());

                _cardInfos[playerIx].push_back(nextCard);
            }
        }
        TRACE_DEBUG("\n");
        if (_vctpAlgPlayer[playerIx]) {
            _vctpAlgPlayer[playerIx]->ALG_NewGiocata(vctCardArray, playerIx);
        }
    }
    _matchPoints.GiocataStart();
    NotifyScript(SCR_NFY_NEWGIOCATA);
    _mano.GiocataStart();
}

void InvidoCore::Mano_End() {
    _matchPoints.ManoEnd();

    if (_matchPoints.IsManoPatada()) {
        Uint8 playerIx = _p_PlayersOnTable->GetFirstOnTrick();
        _p_PlayersOnTable->SetCurrentAndFirstOnTrick(playerIx);

    } else {
        Uint8 winnerPlayerIx = _matchPoints.GetManoWinner();
        _p_PlayersOnTable->SetCurrentAndFirstOnTrick(winnerPlayerIx);
    }
    _p_PlHaveToPlay = _p_PlayersOnTable->SwitchToNextPlayer();
    _giocata.Update_Giocata(_p_PlHaveToPlay->GetIndex(), &_matchPoints);

    for (int i = 0; i < _numPlayers; i++) {
        if (_vctpAlgPlayer[i]) {
            _vctpAlgPlayer[i]->ALG_ManoEnd(&_matchPoints);
        }
    }
    NotifyScript(SCR_NFY_ALGMANOEND);
}

void InvidoCore::Giocata_AMonte() {
    _matchPoints.AMonte();
    _giocata.Update_Giocata(NOT_VALID_INDEX, &_matchPoints);
}

void InvidoCore::Player_VaVia(Uint8 playerIx) {
    // SDL_assert(0);
    _matchPoints.PlayerVaVia(playerIx);

    _giocata.Update_Giocata(playerIx, &_matchPoints);
}

void InvidoCore::ChangeGiocataScore(eGiocataScoreState eNewScore) {
    for (int i = 0; i < _numPlayers; i++) {
        if (_vctpAlgPlayer[i]) {
            _vctpAlgPlayer[i]->ALG_GicataScoreChange(eNewScore);
        }
    }
}

void InvidoCore::Giocata_End() {
    // calculate points
    _matchPoints.GiocataEnd();
    // update match state machine
    _partita.Update_Partita(&_matchPoints);

    for (int i = 0; i < _numPlayers; i++) {
        if (_vctpAlgPlayer[i]) {
            _vctpAlgPlayer[i]->ALG_GiocataEnd(&_matchPoints);
        }
    }

    NotifyScript(SCR_NFY_ALGGIOCATAEND);
}

void InvidoCore::Partita_End() {
    for (int i = 0; i < _numPlayers; i++) {
        if (_vctpAlgPlayer[i]) {
            _vctpAlgPlayer[i]->ALG_MatchEnd(&_matchPoints);
        }
    }
    NotifyScript(SCR_NFY_ALGMATCHEND);
}

void InvidoCore::AbandonGame(Uint8 playerIx) {
    Player* pPlayer = _p_PlayersOnTable->PeekNextPlayerToIx(playerIx);
    SDL_assert(pPlayer);
    // give the victory to the next player
    Uint8 nextPlayerIx = pPlayer->GetIndex();

    _matchPoints.SetTheWinner(nextPlayerIx);

    for (int i = 0; i < _numPlayers; i++) {
        if (_vctpAlgPlayer[i]) {
            _vctpAlgPlayer[i]->ALG_MatchEnd(&_matchPoints);
        }
    }
}

void InvidoCore::NtyWaitingPlayer_Toplay(Uint8 playerIx) {
    SDL_assert(_p_PlHaveToPlay);
    SDL_assert(playerIx == _p_PlHaveToPlay->GetIndex());

    I_ALG_Player* pAlg = _vctpAlgPlayer[playerIx];
    SDL_assert(pAlg);

    pAlg->ALG_HaveToPlay();
}

void InvidoCore::NtyWaitingPlayer_ToResp(Uint8 playerIx) {
    I_ALG_Player* pAlg = _vctpAlgPlayer[playerIx];
    SDL_assert(pAlg);

    pAlg->ALG_HaveToRespond();
}

void InvidoCore::NtyPlayerSayBuiada(Uint8 playerIx) {
    for (int i = 0; i < _numPlayers; i++) {
        if (_vctpAlgPlayer[i]) {
            _vctpAlgPlayer[i]->ALG_PlayerSaidWrong(playerIx);
        }
    }
}

CardSpec* InvidoCore::checkValidCardPlayed(Uint8 playerIx,
                                           const CardSpec& cardSpec) {
    SDL_assert(playerIx >= 0 && playerIx < MAX_NUM_PLAYER);
    CardSpec* pCardplayed = NULL;
    for (CardSpec& cardInHand : _cardInfos[playerIx]) {
        if (cardInHand == cardSpec) {
            pCardplayed = &cardInHand;
        }
    }

    return pCardplayed;
}

bool InvidoCore::VaDentro(Uint8 playerIx, const CardSpec& cardSpec) {
    CardSpec* pCardplayed = checkValidCardPlayed(playerIx, cardSpec);
    if (pCardplayed == NULL) {
        return false;
    }

    bool bRes = false;
    // change mano state
    if (_mano.Player_Play(playerIx, true)) {
        // next player is on game
        _p_PlHaveToPlay = _p_PlayersOnTable->SwitchToNextPlayer();

        _matchPoints.VaDentro(playerIx);

        for (int i = 0; i < _numPlayers; i++) {
            // notify all players that a card was played
            if (_vctpAlgPlayer[i]) {
                _vctpAlgPlayer[i]->ALG_PlayerHasVadoDentro(playerIx);
            }
        }

        // reset info about played card
        resetCard(playerIx, *pCardplayed);
        bRes = true;
    }
    return bRes;
}

void InvidoCore::SetRandomSeed(int iVal) { _p_MyMazzo->SetRandomSeed(iVal); }

bool InvidoCore::PlayCard(Uint8 playerIx, const CardSpec& cardSpec) {
    CardSpec* pCardplayed = checkValidCardPlayed(playerIx, cardSpec);
    if (pCardplayed == NULL) {
        return false;
    }

    bool bRes = false;
    // change mano state
    if (_mano.Player_Play(playerIx, false)) {
        // next player is on game
        _p_PlHaveToPlay = _p_PlayersOnTable->SwitchToNextPlayer();

        // update match points
        _matchPoints.PlayerPlay(playerIx, *pCardplayed);

        for (int i = 0; i < _numPlayers; i++) {
            // notify all players that a card was played
            if (_vctpAlgPlayer[i]) {
                _vctpAlgPlayer[i]->ALG_PlayerHasPlayed(playerIx, *pCardplayed);
            }
        }

        resetCard(playerIx, *pCardplayed);

        bRes = true;
    }
    return bRes;
}

bool InvidoCore::Say(Uint8 playerIx, eSayPlayer eSay) {
    bool bRes = false;
    if (_mano.Player_Say(playerIx, eSay)) {
        //  what he said is acceptable on the game
        for (int i = 0; i < _numPlayers; i++) {
            // notify all players that a card was played
            if (_vctpAlgPlayer[i]) {
                _vctpAlgPlayer[i]->ALG_PlayerHasSaid(playerIx, eSay);
            }
        }
        bRes = true;
    }

    return bRes;
}

void InvidoCore::GetAdmittedCommands(VCT_COMMANDS& vct_Commands,
                                     Uint8 playerIx) {
    _mano.GetAdmittedCommands(vct_Commands, playerIx);
}

void InvidoCore::GetMoreCommands(VCT_COMMANDS& vct_Commands, Uint8 playerIx) {
    _mano.GetMoreCommands(vct_Commands, playerIx);
}

void InvidoCore::NotifyScript(eScriptNotification eVal) {
    TRACE_DEBUG("Event %d\n", eVal);
}

void InvidoCore::NotifyScriptAlgorithm(Uint8 playerIx,
                                       eScriptNotification eVal) {
    TRACE_DEBUG("Algorithm %d, %d\n", playerIx, eVal);
}

////////////////////////////////////////// functions called by Script

void InvidoCore::Script_OverrideDeck(Uint8 playerIx, int iC1, int iC2,
                                     int iC3) {
    Player* pCurrPlayer = _p_PlayersOnTable->GetPlayerToPlay();
    // the player that have to play is the player that becomes the first 3 card
    int iRefPlayerIndex = pCurrPlayer->GetIndex();
    int iBegPos = 0;
    if (iRefPlayerIndex != playerIx) {
        int iDist = _p_PlayersOnTable->CalcDistance(iRefPlayerIndex, playerIx);
        iBegPos = NUM_CARDS_HAND * iDist;
    }

    _p_MyMazzo->SetIndexRaw(iBegPos, iC1);
    _p_MyMazzo->SetIndexRaw(iBegPos + 1, iC2);
    _p_MyMazzo->SetIndexRaw(iBegPos + 2, iC3);
}

void InvidoCore::Script_Say(Uint8 playerIx, eSayPlayer eSay) {
    Say(playerIx, eSay);
}

void InvidoCore::Script_Play(Uint8 playerIx, CardSpec& cardPlayed) {
    PlayCard(playerIx, cardPlayed);
}

void InvidoCore::Script_MatchEnd() {
    _partita.Reset();
    _giocata.Reset();
    _mano.Reset();
}
