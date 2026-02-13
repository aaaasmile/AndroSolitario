#include "InvidoCore.h"

#include "Mazzo.h"
#include "TraceService.h"

using namespace invido;

InvidoCore::InvidoCore() {
    _numPlayers = 0;
    _p_PlHaveToPlay = 0;
    _p_StartPlayer = 0;
    _p_MyMazzo = 0;
    resetCardInfoPlayers();
}

InvidoCore::~InvidoCore() { delete _p_MyMazzo; }

void InvidoCore::Create(Player* pHmiPlayer, int iNumPlayers) {
    TRACE_DEBUG("InvidoCore is created\n");
    _p_MyMazzo = new Mazzo();
    _p_MyMazzo->SetCoreEngine(this);
    _p_MyMazzo->Create();

    _playersOnTable.Create(pHmiPlayer, iNumPlayers);

    _eGameType = LOCAL_TYPE;

    _partita.SetCore(this);
    _partita.SetGiocata(&_giocata);

    _giocata.SetCore(this);
    _giocata.SetPartita(&_partita);
    _giocata.SetMano(&_mano);

    _mano.SetCore(this);
    _mano.SetGiocata(&_giocata);
    _mano.SetScore(&_matchPoints);

    _numPlayers = iNumPlayers;

    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        _vctpAlgPlayer[i] = 0;
    }

    _matchPoints.SetManoObj(&_mano);
}

bool InvidoCore::WhoWonsTheGame(Player** ppPlayer) {
    SDL_assert(ppPlayer);
    SDL_assert(0);

    return true;
}

int InvidoCore::getNewMatchFirstPlayer() { return SDL_rand(_numPlayers); }

void InvidoCore::NewMatch() {
    NotifyScript(SCR_NFY_NEWMATCH);

    // extract the first player
    int iFirstPlayer = getNewMatchFirstPlayer();

    // save players alg in a table
    // don't change level during a match
    for (int i = 0; i < _numPlayers; i++) {
        Player* pPlayer = _playersOnTable.GetPlayerIndex(i);
        _vctpAlgPlayer[i] = pPlayer->GetAlg();
        if (_vctpAlgPlayer[i]) {
            // information before match begin
            _vctpAlgPlayer[i]->ALG_SetPlayerIndex(pPlayer->GetIndex());
            _vctpAlgPlayer[i]->ALG_SetCoreInterface(this);
            _vctpAlgPlayer[i]->ALG_NewMatch(_numPlayers);
            if (i == 0) {
                if (_numPlayers == 2) {
                    // invido 2 player
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(1, 0);
                } else if (_numPlayers == 4) {
                    // invido 4 players
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(1, 0);
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(3, 1);
                } else {
                    SDL_assert(0);
                }
            } else if (i == 1) {
                if (_numPlayers == 2) {
                    // invido 2 player
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(0, 0);
                } else {
                    // invido 4 players
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(0, 0);
                    _vctpAlgPlayer[i]->ALG_SetOpponentIndex(2, 1);
                }
            } else if (i == 2) {
                _vctpAlgPlayer[i]->ALG_SetOpponentIndex(1, 0);
                _vctpAlgPlayer[i]->ALG_SetOpponentIndex(3, 1);
            } else if (i == 3) {
                _vctpAlgPlayer[i]->ALG_SetOpponentIndex(0, 0);
                _vctpAlgPlayer[i]->ALG_SetOpponentIndex(2, 1);
            } else {
                SDL_assert(0);
            }
        }
    }

    _mano.MatchStart();
    _playersOnTable.SetFirstOnMatch(iFirstPlayer);
    _p_StartPlayer = _playersOnTable.GetPlayerIndex(iFirstPlayer);
    _partita.NewPartita(iFirstPlayer);
    _matchPoints.MatchStart(_numPlayers);
}

bool InvidoCore::GetPlayerInPlaying(Player** ppPlayer) {
    SDL_assert(ppPlayer);

    *ppPlayer = _p_PlHaveToPlay;

    return true;
}

void InvidoCore::NextAction() {
    _mano.NextAction();
    _giocata.NextAction();
    _partita.NextAction();
}

CardSpec* InvidoCore::isCardInPlayerHand(int iPlayerIx,
                                         const CARDINFO* pCardInfo) {
    CardSpec* pCardSpecRes = NULL;
    SDL_assert(pCardInfo);
    SDL_assert(iPlayerIx >= 0 && iPlayerIx < MAX_NUM_PLAYER);

    CardSpec myCard;
    myCard.SetCardInfo(*pCardInfo);

    for (int i = 0; i < NUM_CARDS_HAND; i++) {
        int iPlayerPosIx = iPlayerIx * NUM_CARDS_HAND + i;
        if (_cardInfos[iPlayerPosIx] == myCard) {
            pCardSpecRes = &_cardInfos[iPlayerPosIx];
            break;
        }
    }

    return pCardSpecRes;
}

void InvidoCore::resetCardInfoPlayers() {
    int iSize = NUM_CARDS_HAND * MAX_NUM_PLAYER;
    for (int i = 0; i < iSize; i++) {
        _cardInfos[i].Reset();
    }
}

bool InvidoCore::resetCard(int iPlayerIx, CARDINFO* pCardInfo) {
    bool bRet = false;
    SDL_assert(pCardInfo);
    SDL_assert(iPlayerIx >= 0 && iPlayerIx < MAX_NUM_PLAYER);

    CardSpec myCard;
    myCard.SetCardInfo(*pCardInfo);

    for (int i = 0; i < NUM_CARDS_HAND; i++) {
        int iPlayerPosIx = iPlayerIx * NUM_CARDS_HAND + i;
        if (_cardInfos[iPlayerPosIx] == myCard) {
            bRet = true;
            _cardInfos[iPlayerPosIx].Reset();
            break;
        }
    }

    return bRet;
}

void InvidoCore::Giocata_Start(long lPlayerIx) {
    TRACE_DEBUG("Nuova giocata\n");

    resetCardInfoPlayers();
    _p_MyMazzo->Shuffle();
    CardSpec tmpCard;
    _playersOnTable.SetFirstOnGiocata(lPlayerIx);

    // 1) first - calculate the table (there is no switch)
    // player that have to play first, is the first that get the cards after
    // shuffle
    int aPlayerDeck[MAX_NUM_PLAYER];
    _playersOnTable.CalcCircleIndex(aPlayerDeck);

    // 2) second - retreive the player that have to play and switch the table
    _p_PlHaveToPlay =
        _playersOnTable.GetPlayerToPlay(PlayersOnTable::SWITCH_TO_NEXT);

    // 3) distribuite cards
    for (int i = 0; i < _numPlayers; i++) {
        int iIxCurrPLayer = aPlayerDeck[i];
        SDL_assert(iIxCurrPLayer >= 0 && iIxCurrPLayer < _numPlayers);

        Player* pCurrPlayer = _playersOnTable.GetPlayerIndex(iIxCurrPLayer);
        CARDINFO CardArray[NUM_CARDS_HAND];

        TRACE_DEBUG("%s => ", pCurrPlayer->GetName());

        for (int j = 0; j < NUM_CARDS_HAND; j++) {
            // distribuite all cards on player
            _p_MyMazzo->PickNextCard(&tmpCard);

            tmpCard.FillInfo(&CardArray[j]);
            TRACE_DEBUG("[%s] , ix: %d, pt: %d", tmpCard.GetName(),
                        tmpCard.GetCardIndex(), tmpCard.GetPoints());

            // store card information for controlling
            int iTmpCdIndex = iIxCurrPLayer * NUM_CARDS_HAND + j;
            _cardInfos[iTmpCdIndex] = tmpCard;
        }
        TRACE_DEBUG("\n");
        if (_vctpAlgPlayer[iIxCurrPLayer]) {
            _vctpAlgPlayer[iIxCurrPLayer]->ALG_NewGiocata(
                CardArray, NUM_CARDS_HAND, lPlayerIx);
        }
    }
    _matchPoints.GiocataStart();
    // notify script
    NotifyScript(SCR_NFY_NEWGIOCATA);
    // notify mano
    _mano.GiocataStart();
}

void InvidoCore::Mano_End() {
    _matchPoints.ManoEnd();
    int iPlayer = _matchPoints.GetManoWinner();

    if (_matchPoints.IsManoPatada()) {
        // trick patada
        // set the same player that have start
        long lPlayer = _playersOnTable.GetFirstOnTrick();
        _playersOnTable.SetFirstOnTrick(lPlayer);

    } else {
        // update the table, the trick winner have to play first
        _playersOnTable.SetFirstOnTrick(iPlayer);
    }
    _p_PlHaveToPlay =
        _playersOnTable.GetPlayerToPlay(PlayersOnTable::SWITCH_TO_NEXT);
    _giocata.Update_Giocata(_p_PlHaveToPlay->GetIndex(), &_matchPoints);

    for (int i = 0; i < _numPlayers; i++) {
        if (_vctpAlgPlayer[i]) {
            _vctpAlgPlayer[i]->ALG_ManoEnd(&_matchPoints);
        }
    }
    // notify script
    NotifyScript(SCR_NFY_ALGMANOEND);
}

void InvidoCore::Giocata_AMonte() {
    _matchPoints.AMonte();
    _giocata.Update_Giocata(NOT_VALID_INDEX, &_matchPoints);
}

void InvidoCore::Player_VaVia(int iPlayerIx) {
    // SDL_assert(0);
    _matchPoints.PlayerVaVia(iPlayerIx);

    _giocata.Update_Giocata(iPlayerIx, &_matchPoints);
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

void InvidoCore::AbandonGame(int iPlayerIx) {
    int iNextPlayer;
    int aPlayerDeck[MAX_NUM_PLAYER];
    _playersOnTable.CalcCircleIndex_Cust(aPlayerDeck, iPlayerIx);
    // give the victory to the next player
    iNextPlayer = aPlayerDeck[1];

    _matchPoints.SetTheWinner(iNextPlayer);

    for (int i = 0; i < _numPlayers; i++) {
        if (_vctpAlgPlayer[i]) {
            _vctpAlgPlayer[i]->ALG_MatchEnd(&_matchPoints);
        }
    }
}

void InvidoCore::NtyWaitingPlayer_Toplay(int iPlayerIx) {
    SDL_assert(_p_PlHaveToPlay);
    SDL_assert(iPlayerIx == _p_PlHaveToPlay->GetIndex());

    I_ALG_Player* pAlg = _vctpAlgPlayer[iPlayerIx];
    SDL_assert(pAlg);

    pAlg->ALG_Play();
}

void InvidoCore::NtyWaitingPlayer_ToResp(int iPlayerIx) {
    I_ALG_Player* pAlg = _vctpAlgPlayer[iPlayerIx];
    SDL_assert(pAlg);

    pAlg->ALG_HaveToRespond();
}

void InvidoCore::NtyPlayerSayBuiada(int iPlayerIx) {
    for (int i = 0; i < _numPlayers; i++) {
        if (_vctpAlgPlayer[i]) {
            _vctpAlgPlayer[i]->ALG_PlayerSaidWrong(iPlayerIx);
        }
    }
}

void InvidoCore::RaiseError(const std::string& errorMsg) {
    SDL_assert(0);
    TRACE_DEBUG(const_cast<char*>(errorMsg.c_str()));
    TRACE_DEBUG("\n");
}

CardSpec* InvidoCore::checkValidCardPlayed(int iPlayerIx,
                                           const CARDINFO* pCardInfo) {
    CardSpec cardUndef;
    if (cardUndef.GetCardIndex() == pCardInfo->byIndex) {
        SDL_assert(0);
    }

    CardSpec* pCardplayed = isCardInPlayerHand(iPlayerIx, pCardInfo);

    return pCardplayed;
}

bool InvidoCore::Player_vaDentro(int iPlayerIx, const CARDINFO* pCardInfo) {
    CardSpec* pCardplayed = checkValidCardPlayed(iPlayerIx, pCardInfo);
    if (pCardplayed == NULL) {
        return false;
    }

    bool bRes = false;
    // change mano state
    if (_mano.Player_Play(iPlayerIx, true)) {
        // next player is on game
        _p_PlHaveToPlay =
            _playersOnTable.GetPlayerToPlay(PlayersOnTable::SWITCH_TO_NEXT);

        // update match points
        int ixCardVaDentro = 3;
        CARDINFO cardVadodentro;  // quattro di bastoni, carta sempre perdente
        cardVadodentro.byIndex = ixCardVaDentro;  // TODO check this
        cardVadodentro.eSuit = eSUIT::BASTONI;

        _matchPoints.PlayerPlay(iPlayerIx, &cardVadodentro);

        for (int i = 0; i < _numPlayers; i++) {
            // notify all players that a card was played
            if (_vctpAlgPlayer[i]) {
                _vctpAlgPlayer[i]->ALG_PlayerHasVadoDentro(iPlayerIx);
            }
        }

        // reset info about played card
        resetCard(iPlayerIx, pCardplayed->GetCardInfo());
        bRes = true;
    }
    return bRes;
}

void InvidoCore::SetRandomSeed(int iVal) { _p_MyMazzo->SetRandomSeed(iVal); }

bool InvidoCore::PlayCard(int iPlayerIx, const CardSpec& cardSpec) {
    CardSpec* pCardplayed = checkValidCardPlayed(iPlayerIx, cardSpec);
    if (pCardplayed == NULL) {
        return false;
    }

    bool bRes = false;
    // change mano state
    if (_mano.Player_Play(iPlayerIx, false)) {
        // next player is on game
        _p_PlHaveToPlay =
            _playersOnTable.GetPlayerToPlay(PlayersOnTable::SWITCH_TO_NEXT);

        // update match points
        _matchPoints.PlayerPlay(iPlayerIx, pCardplayed->GetCardInfo());

        for (int i = 0; i < _numPlayers; i++) {
            // notify all players that a card was played
            if (_vctpAlgPlayer[i]) {
                _vctpAlgPlayer[i]->ALG_PlayerHasPlayed(
                    iPlayerIx, pCardplayed->GetCardInfo());
            }
        }

        resetCard(iPlayerIx, pCardplayed->GetCardInfo());

        bRes = true;
    }
    return bRes;
}

bool InvidoCore::Say(int iPlayerIx, eSayPlayer eSay) {
    bool bRes = false;
    if (_mano.Player_Say(iPlayerIx, eSay)) {
        //  what he said is acceptable on the game
        for (int i = 0; i < _numPlayers; i++) {
            // notify all players that a card was played
            if (_vctpAlgPlayer[i]) {
                _vctpAlgPlayer[i]->ALG_PlayerHasSaid(iPlayerIx, eSay);
            }
        }
        bRes = true;
    }

    return bRes;
}

void InvidoCore::GetAdmittedCommands(VCT_COMMANDS& vct_Commands,
                                     int iPlayerIndex) {
    _mano.GetAdmittedCommands(vct_Commands, iPlayerIndex);
}

void InvidoCore::GetMoreCommands(VCT_COMMANDS& vct_Commands, int iPlayerIndex) {
    _mano.GetMoreCommands(vct_Commands, iPlayerIndex);
}

void InvidoCore::NotifyScript(eScriptNotification eVal) {
    TRACE_DEBUG("Event %d\n", eVal);
}

void InvidoCore::NotifyScriptAlgorithm(int iPlayerIx,
                                       eScriptNotification eVal) {
    TRACE_DEBUG("Algorithm %d, %d\n", iPlayerIx, eVal);
}

////////////////////////////////////////// functions called by Script

void InvidoCore::Script_OverrideDeck(int iPlayer, int iC1, int iC2, int iC3) {
    Player* pCurrPlayer =
        _playersOnTable.GetPlayerToPlay(PlayersOnTable::NO_SWITCH);
    // the player that have to play is the player that becomes the first 3 card
    int iRefPlayerIndex = pCurrPlayer->GetIndex();
    int iBegPos = 0;
    if (iRefPlayerIndex != iPlayer) {
        int iDist = _playersOnTable.CalcDistance(iRefPlayerIndex, iPlayer);
        iBegPos = NUM_CARDS_HAND * iDist;
    }

    _p_MyMazzo->SetIndexRaw(iBegPos, iC1);
    _p_MyMazzo->SetIndexRaw(iBegPos + 1, iC2);
    _p_MyMazzo->SetIndexRaw(iBegPos + 2, iC3);
}

void InvidoCore::Script_SetStartPlayer(int iPlayer) {}

void InvidoCore::Script_Say(int iPlayer, eSayPlayer eSay) {
    Player_saySomething(iPlayer, eSay);
}

void InvidoCore::Script_Play(int iPlayer, CardSpec& CardPlayed) {
    Player_playCard(iPlayer, CardPlayed.GetCardInfo());
}

void InvidoCore::Script_MatchEnd() {
    // reset state machines
    _partita.Reset();
    _giocata.Reset();
    _mano.Reset();
}

int InvidoCore::Script_CheckResult(int iTypeOfItem, int iParam1,
                                   int iExpectedVal) {
    int iRes = 0;

    return iRes;
}
