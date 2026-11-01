#include "InvidoCore.h"

#include "Mazzo.h"
#include "Player.h"
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

// trait Partita - Start

void fncBind_PartitaEnd(void* self) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->PartitaEnded();
}
void fncBind_PartitaStarted(void* self, Uint8 playerStartIx) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->PartitaStarted(playerStartIx);
}
void fncBind_GiocataStarted(void* self, Uint8 playerStartIx) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->Giocata_Start(playerStartIx);
}
void fncBind_GiocataEndedWinner(void* self, Uint8) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->Giocata_End();
}
void fncBind_GiocataEndDueced(void* self) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->Giocata_End();
}
void fncBind_ManoEnd(void* self) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->Mano_End();
}
void fncBind_NtyWaitingPlayer_Toplay(void* self, Uint8 playerIx) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->NtyWaitingPlayer_Toplay(playerIx);
}
void fncBind_NtyWaitingPlayer_ToResp(void* self, Uint8 playerIx) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->NtyWaitingPlayer_ToResp(playerIx);
}
void fncBind_NtyPlayerSayBuiada(void* self, Uint8 playerIx) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->NtyPlayerSayBuiada(playerIx);
}
void fncBind_ChangeGiocataScore(void* self, eGiocataScoreState eNewScore) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->ChangeGiocataScore(eNewScore);
}
void fncBind_Player_VaVia(void* self, Uint8 playerIx) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->Player_VaVia(playerIx);
}
void fncBind_Giocata_AMonte(void* self) {
    InvidoCore* pCore = (InvidoCore*)self;
    return pCore->Giocata_AMonte();
}

ManoCb InvidoCore::prepManoDelegator() {
    static VManoCb const tc = {
        .Mano_End = (&fncBind_ManoEnd),
        .NtyWaitingPlayer_Toplay = (&fncBind_NtyWaitingPlayer_Toplay),
        .NtyWaitingPlayer_ToResp = (&fncBind_NtyWaitingPlayer_ToResp),
        .NtyPlayerSayBuiada = (&fncBind_NtyPlayerSayBuiada),
        .ChangeGiocataScore = (&fncBind_ChangeGiocataScore),
        .Player_VaVia = (&fncBind_Player_VaVia),
        .Giocata_AMonte = (&fncBind_Giocata_AMonte)};

    return (ManoCb){.tc = &tc, .self = this};
}

PartitaCb InvidoCore::prepPartitaDelegator() {
    // Use only static otherwise you loose it (dangling pointer on stack
    // variable) it works because InvidoCore is active only on one instance at
    // the same time
    static VPartitaCb const tc = {.PartitaStarted = (&fncBind_PartitaStarted),
                                  .PartitaEnded = (&fncBind_PartitaEnd)};

    return (PartitaCb){.tc = &tc, .self = this};
}

GiocataCb InvidoCore::prepGiocataDelegator() {
    // The callback table must outlive this function because Giocata stores it.
    static VGiocataCb const tc = {
        .GiocataStarted = (&fncBind_GiocataStarted),
        .GiocataEndedWinner = (&fncBind_GiocataEndedWinner),
        .GiocataEndDueced = (&fncBind_GiocataEndDueced)};

    return (GiocataCb){.tc = &tc, .self = this};
}

// trait Partita - End

void InvidoCore::Init(PlayersOnTable* pPlayersOnTable, Mazzo* pMazzo) {
    TRACE_DEBUG("InvidoCore initialize\n");

    SDL_assert(pPlayersOnTable);
    SDL_assert(pMazzo);

    _vctpAlgPlayer.clear();
    _vctCardInfos.clear();
    _p_PlHaveToPlay = NULL;
    _p_StartPlayer = NULL;

    _p_MyMazzo = pMazzo;
    _p_PlayersOnTable = pPlayersOnTable;

    _eGameType = LOCAL_TYPE;

    PartitaCb delegatorCb = prepPartitaDelegator();
    _partita.SetPartitaCB(delegatorCb);
    //_partita.SetGiocata(&_giocata);

    GiocataCb giocataCb = prepGiocataDelegator();
    _giocata.SetGiocataCB(giocataCb);

    //_giocata.SetPartita(&_partita);
    //_giocata.SetMano(&_mano);

    ManoCb manoCb = prepManoDelegator();
    _mano.SetManoCB(manoCb);
    _mano.SetScore(&_matchStateWithScore);

    _numPlayers = _p_PlayersOnTable->GetNumOfPlayers();

    //_matchStateWithScore.SetManoInstance(&_mano);
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

    // Keep algorithm and hand vectors aligned with player indices.
    _vctpAlgPlayer.clear();
    _vctpAlgPlayer.reserve(_numPlayers);
    _vctCardInfos.assign(_numPlayers, VCT_CARDSPEC{});

    // don't change level during a match
    for (int i = 0; i < _numPlayers; i++) {
        Player* pPlayer = _p_PlayersOnTable->GetPlayerOnIndex(i);
        I_ALG_Player* pIAlgPlayer = pPlayer->GetAlg();
        if (pIAlgPlayer) {
            // information before match begin
            pIAlgPlayer->ALG_SetPlayerIndex(pPlayer->GetIndex());
            pIAlgPlayer->ALG_SetCoreInterface(this);
            if (i == 0) {
                if (_numPlayers == 2) {
                    // invido 2 player
                    pIAlgPlayer->ALG_SetOpponentIndex(1);
                } else if (_numPlayers == 4) {
                    // invido 4 players
                    pIAlgPlayer->ALG_SetOpponentIndex(1);
                    pIAlgPlayer->ALG_SetOpponentIndex(3);
                } else {
                    SDL_assert(0);
                }
            } else if (i == 1) {
                if (_numPlayers == 2) {
                    // invido 2 player
                    pIAlgPlayer->ALG_SetOpponentIndex(0);
                } else {
                    // invido 4 players
                    pIAlgPlayer->ALG_SetOpponentIndex(0);
                    pIAlgPlayer->ALG_SetOpponentIndex(2);
                }
            } else if (i == 2) {
                pIAlgPlayer->ALG_SetOpponentIndex(1);
                pIAlgPlayer->ALG_SetOpponentIndex(3);
            } else if (i == 3) {
                pIAlgPlayer->ALG_SetOpponentIndex(0);
                pIAlgPlayer->ALG_SetOpponentIndex(2);
            } else {
                SDL_assert(0);
            }
            _vctpAlgPlayer.push_back(pIAlgPlayer);
        }
    }

    _mano.Init(
        _numPlayers);  // TODO set Num players, _mano is not for match state
    _p_PlayersOnTable->SetFirstOnMatch(firstPlayerIx);
    _p_StartPlayer = _p_PlayersOnTable->GetPlayerOnIndex(firstPlayerIx);
    _partita.NewPartita(firstPlayerIx);
    _matchStateWithScore.MatchStart(_numPlayers);  // TODO set Num players, _matchStateWithScore
                                           // is not for match state
}

bool InvidoCore::NextAction() {
    const bool manoActionTaken = _mano.NextAction();
    const bool giocataActionTaken = _giocata.NextAction();
    const bool partitaActionTaken = _partita.NextAction();

    return manoActionTaken || giocataActionTaken || partitaActionTaken;
}

void InvidoCore::resetCardInfoPlayers() {
    for (VCT_CARDSPEC& cardInfos : _vctCardInfos) {
        cardInfos.clear();
    }
}

bool InvidoCore::resetCard(Uint8 playerIx, const CardSpec& cardSpec) {
    SDL_assert(playerIx < _vctCardInfos.size());
    if (playerIx >= _vctCardInfos.size()) {
        return false;
    }

    const int cardIndex = cardSpec.GetCardIndex();
    auto& playerCards = _vctCardInfos[playerIx];
    for (auto cardIt = playerCards.begin(); cardIt != playerCards.end();
         ++cardIt) {
        if (cardIt->GetCardIndex() == cardIndex) {
            playerCards.erase(cardIt);
            return true;
        }
    }

    return false;
}

void InvidoCore::Giocata_Start(Uint8 playerIx) {
    TRACE_DEBUG("Giocata_Start\n");

    resetCardInfoPlayers();
    // 1) first - calculate the table (there is no switch)
    // player that have to play first, is the first that get the cards after
    // shuffle

    _p_MyMazzo->Reset();
    _p_MyMazzo->Shuffle();
    NotifyScript(SCR_NFY_SHUFFLEDECK);
    _p_PlayersOnTable->SetFirstOnGiocata(playerIx);

    // 2) second - retreive the player that have to play and switch the table
    _p_PlHaveToPlay = _p_PlayersOnTable->SwitchToNextPlayer();
    // 3) distribute cards starting from the player who will play first.
    const Uint8 firstPlayerIx = _p_PlHaveToPlay->GetIndex();
    for (size_t playerOffset = 0; playerOffset < _numPlayers; ++playerOffset) {
        const Uint8 playerIx =
            static_cast<Uint8>((firstPlayerIx + playerOffset) % _numPlayers);
        Player* pCurrPlayer = _p_PlayersOnTable->GetPlayerOnIndex(playerIx);

        VCT_CARDSPEC vctCardArray;

        TRACE_DEBUG("%s => ", pCurrPlayer->GetName().c_str());

        for (int j = 0; j < NUM_CARDS_HAND; j++) {
            bool isValid;
            CardSpec nextCard;
            isValid = _p_MyMazzo->PickNextCard(&nextCard);
            if (isValid) {
                vctCardArray.push_back(nextCard);  // CardSpec has no pointers,
                                                   // this is perfectly fine
                TRACE_DEBUG("[%s] , ix: %d, pt: %d", nextCard.GetName().c_str(),
                            nextCard.GetCardIndex(), nextCard.GetPoints());

                _vctCardInfos[playerIx].push_back(nextCard);
            }
        }
        TRACE_DEBUG("\n");
        if (_vctpAlgPlayer[playerIx]) {
            _vctpAlgPlayer[playerIx]->ALG_NewGiocata(vctCardArray, playerIx);
        }
    }
    _matchStateWithScore.GiocataStart();
    NotifyScript(SCR_NFY_NEWGIOCATA);
    _mano.GiocataStart();
    LPErrInApp manoError = _mano.NewMano(firstPlayerIx);
    SDL_assert(manoError == NULL);
}

void InvidoCore::Mano_End() {
    _matchStateWithScore.ManoEnd();

    const bool giocataEnded = _matchStateWithScore.IsGiocataEnd();
    if (giocataEnded) {
        if (_matchStateWithScore.IsGiocataPatada()) {
            _giocata.GiocataPatada();
        } else if (_matchStateWithScore.IsGiocataMonte()) {
            _giocata.GiocataAMonte();
        } else {
            Uint8 giocataWinnerIx = _matchStateWithScore.GetGiocataWinner();
            _giocata.PlayerGiocataWins(giocataWinnerIx);
        }
    } else {
        if (_matchStateWithScore.IsManoPatada()) {
            Uint8 playerIx = _p_PlayersOnTable->GetFirstOnTrick();
            _p_PlayersOnTable->SetCurrentAndFirstOnTrick(playerIx);
        } else {
            Uint8 winnerPlayerIx = _matchStateWithScore.GetManoWinner();
            _p_PlayersOnTable->SetCurrentAndFirstOnTrick(winnerPlayerIx);
        }
        _p_PlHaveToPlay = _p_PlayersOnTable->SwitchToNextPlayer();
        LPErrInApp manoError = _mano.NewMano(_p_PlHaveToPlay->GetIndex());
        SDL_assert(manoError == NULL);
    }
    for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
        if (pIAlgPlayer) {
            pIAlgPlayer->ALG_ManoEnd(&_matchStateWithScore);
        }
    }
    NotifyScript(SCR_NFY_ALGMANOEND);
}

void InvidoCore::Giocata_AMonte() {
    _matchStateWithScore.GiocataAMonte();
    _giocata.GiocataAMonte();
}

void InvidoCore::Player_VaVia(Uint8 playerIx) {
    Player* pPlayer = _p_PlayersOnTable->PeekNextPlayerToIx(playerIx);
    Uint8 playerWinnerIx = pPlayer->GetIndex();
    _matchStateWithScore.GiocataEndWithWinner(playerWinnerIx);
    _giocata.PlayerGiocataWins(playerWinnerIx);
}

void InvidoCore::ChangeGiocataScore(eGiocataScoreState eNewScore) {
    for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
        if (pIAlgPlayer) {
            pIAlgPlayer->ALG_GicataScoreChange(eNewScore);
        }
    }
}

void InvidoCore::Giocata_End() {
    for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
        if (pIAlgPlayer) {
            pIAlgPlayer->ALG_GiocataEnd(&_matchStateWithScore);
        }
    }

    NotifyScript(SCR_NFY_ALGGIOCATAEND);
    _mano.Reset();

    if (_matchStateWithScore.IsMatchEnd()) {
        _partita.PartitaEnd();
        return;
    }

    Uint8 nextStartPlayerIx = _matchStateWithScore.GetGiocataWinner();
    if (nextStartPlayerIx == NOT_VALID_INDEX) {
        nextStartPlayerIx = _p_PlayersOnTable->GetPlayerToPlay()->GetIndex();
    }
    LPErrInApp giocataError = _giocata.NewGiocata(nextStartPlayerIx);
    SDL_assert(giocataError == NULL);
}

// Partita CB - start
void InvidoCore::PartitaEnded() {
    for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
        if (pIAlgPlayer) {
            pIAlgPlayer->ALG_MatchEnd(&_matchStateWithScore);
        }
    }
    NotifyScript(SCR_NFY_ALGMATCHEND);
}

void InvidoCore::PartitaStarted(Uint8 playerStartIx) {
    for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
        if (pIAlgPlayer) {
            pIAlgPlayer->ALG_NewMatch(_numPlayers);
        }
    }
    _giocata.NewGiocata(playerStartIx);
}
// Partita CB  - end

void InvidoCore::AbandonGame(Uint8 playerIx) {
    Player* pWinnerPlayer = _p_PlayersOnTable->PeekNextPlayerToIx(playerIx);
    SDL_assert(pWinnerPlayer);
    // give the victory to the next player
    Uint8 winnerPlayerIx = pWinnerPlayer->GetIndex();

    _matchStateWithScore.SetTheWinnerBecauseAbandon(winnerPlayerIx);

    for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
        if (pIAlgPlayer) {
            pIAlgPlayer->ALG_MatchEnd(&_matchStateWithScore);
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
    for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
        if (pIAlgPlayer) {
            pIAlgPlayer->ALG_PlayerSaidWrong(playerIx);
        }
    }
}

CardSpec* InvidoCore::checkValidCardPlayed(Uint8 playerIx,
                                           const CardSpec& cardSpec) {
    if (playerIx >= _vctCardInfos.size()) {
        return NULL;
    }

    for (CardSpec& cardInHand : _vctCardInfos[playerIx]) {
        if (cardInHand == cardSpec) {
            return &cardInHand;
        }
    }

    return NULL;
}

bool InvidoCore::VaDentro(Uint8 playerIx, const CardSpec& cardSpec) {
    CardSpec* pCardplayed = checkValidCardPlayed(playerIx, cardSpec);
    if (pCardplayed == NULL) {
        return false;
    }

    bool bRes = false;
    // change mano state
    if (_mano.Player_PlayACard(playerIx, true)) {
        // next player is on game
        _p_PlHaveToPlay = _p_PlayersOnTable->SwitchToNextPlayer();

        _matchStateWithScore.VaDentro(playerIx);

        for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
            // notify all players that a card was played
            if (pIAlgPlayer) {
                pIAlgPlayer->ALG_PlayerHasVadoDentro(playerIx);
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
    SDL_assert(pCardplayed);
    if (pCardplayed == NULL) {
        return false;
    }

    bool bRes = false;
    // change mano state
    if (_mano.Player_PlayACard(playerIx, false)) {
        // next player is on game
        _p_PlHaveToPlay = _p_PlayersOnTable->SwitchToNextPlayer();

        // update match points
        _matchStateWithScore.PlayerPlay(playerIx, *pCardplayed);

        for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
            // notify all players that a card was played
            if (pIAlgPlayer) {
                pIAlgPlayer->ALG_PlayerHasPlayed(playerIx, *pCardplayed);
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
        for (I_ALG_Player* pIAlgPlayer : _vctpAlgPlayer) {
            // notify all players that a card was played
            if (pIAlgPlayer) {
                pIAlgPlayer->ALG_PlayerHasSaid(playerIx, eSay);
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
    // TRACE_DEBUG("Event %d\n", eVal);
}

void InvidoCore::NotifyScriptAlgorithm(Uint8 playerIx,
                                       eScriptNotification eVal) {
    // TRACE_DEBUG("Algorithm %d, %d\n", playerIx, eVal);
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
