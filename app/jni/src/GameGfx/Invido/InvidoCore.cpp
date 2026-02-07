#include "InvidoCore.h"

#include "Mazzo.h"
#include "TraceService.h"

InvidoCore::InvidoCore() {
    m_lNumPlayers = 0;
    m_pPlHaveToPlay = 0;
    m_pStartPlayer = 0;
    m_pMyMazzo = 0;
    resetCardInfoPlayers();
}

InvidoCore::~InvidoCore() { delete m_pMyMazzo; }

void InvidoCore::Create(Player* pHmiPlayer, int iNumPlayers) {
    m_pTracer = TraceService::Instance();
    // TRACE_DEBUG("InvidoCore is created\n");
    m_pTracer->AddSimpleTrace(2, "InvidoCore is created");

    m_pMyMazzo = new Mazzo();
    m_pMyMazzo->SetCoreEngine(this);
    m_pMyMazzo->Create();

    m_PlayersOnTable.Create(pHmiPlayer, iNumPlayers);

    m_eGameType = LOCAL_TYPE;

    m_Partita.SetCore(this);
    m_Partita.SetGiocata(&m_Giocata);

    m_Giocata.SetCore(this);
    m_Giocata.SetPartita(&m_Partita);
    m_Giocata.SetMano(&m_Mano);

    m_Mano.SetCore(this);
    m_Mano.SetGiocata(&m_Giocata);
    m_Mano.SetScore(&m_MatchPoints);

    m_lNumPlayers = iNumPlayers;

    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        m_vctAlgPlayer[i] = 0;
    }

    m_MatchPoints.SetManoObj(&m_Mano);
}

bool InvidoCore::WhoWonsTheGame(Player** ppPlayer) {
    SDL_assert(ppPlayer);
    SDL_assert(0);

    return true;
}

int InvidoCore::getNewMatchFirstPlayer() { return CASO(m_lNumPlayers); }

void InvidoCore::NewMatch() {
    NotifyScript(SCR_NFY_NEWMATCH);

    // extract the first player
    int iFirstPlayer = getNewMatchFirstPlayer();

    // save players alg in a table
    // don't change level during a match
    for (int i = 0; i < m_lNumPlayers; i++) {
        Player* pPlayer = m_PlayersOnTable.GetPlayerIndex(i);
        m_vctAlgPlayer[i] = pPlayer->GetAlg();
        if (m_vctAlgPlayer[i]) {
            // information before match begin
            m_vctAlgPlayer[i]->ALG_SetPlayerIndex(pPlayer->GetIndex());
            m_vctAlgPlayer[i]->ALG_SetCoreInterface(this);
            m_vctAlgPlayer[i]->ALG_NewMatch(m_lNumPlayers);
            if (i == 0) {
                if (m_lNumPlayers == 2) {
                    // invido 2 player
                    m_vctAlgPlayer[i]->ALG_SetOpponentIndex(1, 0);
                } else if (m_lNumPlayers == 4) {
                    // invido 4 players
                    m_vctAlgPlayer[i]->ALG_SetOpponentIndex(1, 0);
                    m_vctAlgPlayer[i]->ALG_SetOpponentIndex(3, 1);
                } else {
                    SDL_assert(0);
                }
            } else if (i == 1) {
                if (m_lNumPlayers == 2) {
                    // invido 2 player
                    m_vctAlgPlayer[i]->ALG_SetOpponentIndex(0, 0);
                } else {
                    // invido 4 players
                    m_vctAlgPlayer[i]->ALG_SetOpponentIndex(0, 0);
                    m_vctAlgPlayer[i]->ALG_SetOpponentIndex(2, 1);
                }
            } else if (i == 2) {
                m_vctAlgPlayer[i]->ALG_SetOpponentIndex(1, 0);
                m_vctAlgPlayer[i]->ALG_SetOpponentIndex(3, 1);
            } else if (i == 3) {
                m_vctAlgPlayer[i]->ALG_SetOpponentIndex(0, 0);
                m_vctAlgPlayer[i]->ALG_SetOpponentIndex(2, 1);
            } else {
                SDL_assert(0);
            }
        }
    }

    m_Mano.MatchStart();
    m_PlayersOnTable.SetFirstOnMatch(iFirstPlayer);
    m_pStartPlayer = m_PlayersOnTable.GetPlayerIndex(iFirstPlayer);
    m_Partita.NewPartita(iFirstPlayer);
    m_MatchPoints.MatchStart(m_lNumPlayers);
}

bool InvidoCore::GetPlayerInPlaying(Player** ppPlayer) {
    SDL_assert(ppPlayer);

    *ppPlayer = m_pPlHaveToPlay;

    return true;
}

void InvidoCore::NextAction() {
    m_Mano.NextAction();
    m_Giocata.NextAction();
    m_Partita.NextAction();
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
        if (m_aCardInfo[iPlayerPosIx] == myCard) {
            pCardSpecRes = &m_aCardInfo[iPlayerPosIx];
            break;
        }
    }

    return pCardSpecRes;
}

void InvidoCore::resetCardInfoPlayers() {
    int iSize = NUM_CARDS_HAND * MAX_NUM_PLAYER;
    for (int i = 0; i < iSize; i++) {
        m_aCardInfo[i].Reset();
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
        if (m_aCardInfo[iPlayerPosIx] == myCard) {
            bRet = true;
            m_aCardInfo[iPlayerPosIx].Reset();
            break;
        }
    }

    return bRet;
}

void InvidoCore::Giocata_Start(long lPlayerIx) {
    TRACE_DEBUG("Nuova giocata\n");

    resetCardInfoPlayers();
    m_pMyMazzo->Shuffle();
    CardSpec tmpCard;
    m_PlayersOnTable.SetFirstOnGiocata(lPlayerIx);

    // 1) first - calculate the table (there is no switch)
    // player that have to play first, is the first that get the cards after
    // shuffle
    int aPlayerDeck[MAX_NUM_PLAYER];
    m_PlayersOnTable.CalcCircleIndex(aPlayerDeck);

    // 2) second - retreive the player that have to play and switch the table
    m_pPlHaveToPlay =
        m_PlayersOnTable.GetPlayerToPlay(PlayersOnTable::SWITCH_TO_NEXT);

    // 3) distribuite cards
    for (int i = 0; i < m_lNumPlayers; i++) {
        int iIxCurrPLayer = aPlayerDeck[i];
        SDL_assert(iIxCurrPLayer >= 0 && iIxCurrPLayer < m_lNumPlayers);

        Player* pCurrPlayer = m_PlayersOnTable.GetPlayerIndex(iIxCurrPLayer);
        CARDINFO CardArray[NUM_CARDS_HAND];

        TRACE_DEBUG("%s => ", pCurrPlayer->GetName());

        for (int j = 0; j < NUM_CARDS_HAND; j++) {
            // distribuite all cards on player
            m_pMyMazzo->PickNextCard(&tmpCard);

            tmpCard.FillInfo(&CardArray[j]);
            TRACE_DEBUG("[%s] , ix: %d, pt: %d", tmpCard.GetName(),
                        tmpCard.GetCardIndex(), tmpCard.GetPoints());

            // store card information for controlling
            int iTmpCdIndex = iIxCurrPLayer * NUM_CARDS_HAND + j;
            m_aCardInfo[iTmpCdIndex] = tmpCard;
        }
        TRACE_DEBUG("\n");
        if (m_vctAlgPlayer[iIxCurrPLayer]) {
            m_vctAlgPlayer[iIxCurrPLayer]->ALG_NewGiocata(
                CardArray, NUM_CARDS_HAND, lPlayerIx);
        }
    }
    m_MatchPoints.GiocataStart();
    // notify script
    NotifyScript(SCR_NFY_NEWGIOCATA);
    // notify mano
    m_Mano.GiocataStart();
}

void InvidoCore::Mano_End() {
    m_MatchPoints.ManoEnd();
    int iPlayer = m_MatchPoints.GetManoWinner();

    if (m_MatchPoints.IsManoPatada()) {
        // trick patada
        // set the same player that have start
        long lPlayer = m_PlayersOnTable.GetFirstOnTrick();
        m_PlayersOnTable.SetFirstOnTrick(lPlayer);

    } else {
        // update the table, the trick winner have to play first
        m_PlayersOnTable.SetFirstOnTrick(iPlayer);
    }
    m_pPlHaveToPlay =
        m_PlayersOnTable.GetPlayerToPlay(PlayersOnTable::SWITCH_TO_NEXT);
    m_Giocata.Update_Giocata(m_pPlHaveToPlay->GetIndex(), &m_MatchPoints);

    for (int i = 0; i < m_lNumPlayers; i++) {
        if (m_vctAlgPlayer[i]) {
            m_vctAlgPlayer[i]->ALG_ManoEnd(&m_MatchPoints);
        }
    }
    // notify script
    NotifyScript(SCR_NFY_ALGMANOEND);
}

void InvidoCore::Giocata_AMonte() {
    m_MatchPoints.AMonte();
    m_Giocata.Update_Giocata(NOT_VALID_INDEX, &m_MatchPoints);
}

void InvidoCore::Player_VaVia(int iPlayerIx) {
    // SDL_assert(0);
    m_MatchPoints.PlayerVaVia(iPlayerIx);

    m_Giocata.Update_Giocata(iPlayerIx, &m_MatchPoints);
}

void InvidoCore::ChangeGiocataScore(eGiocataScoreState eNewScore) {
    for (int i = 0; i < m_lNumPlayers; i++) {
        if (m_vctAlgPlayer[i]) {
            m_vctAlgPlayer[i]->ALG_GicataScoreChange(eNewScore);
        }
    }
}

void InvidoCore::Giocata_End() {
    // calculate points
    m_MatchPoints.GiocataEnd();
    // update match state machine
    m_Partita.Update_Partita(&m_MatchPoints);

    for (int i = 0; i < m_lNumPlayers; i++) {
        if (m_vctAlgPlayer[i]) {
            m_vctAlgPlayer[i]->ALG_GiocataEnd(&m_MatchPoints);
        }
    }

    NotifyScript(SCR_NFY_ALGGIOCATAEND);
}

void InvidoCore::Partita_End() {
    for (int i = 0; i < m_lNumPlayers; i++) {
        if (m_vctAlgPlayer[i]) {
            m_vctAlgPlayer[i]->ALG_MatchEnd(&m_MatchPoints);
        }
    }
    NotifyScript(SCR_NFY_ALGMATCHEND);
}

void InvidoCore::AbandonGame(int iPlayerIx) {
    int iNextPlayer;
    int aPlayerDeck[MAX_NUM_PLAYER];
    m_PlayersOnTable.CalcCircleIndex_Cust(aPlayerDeck, iPlayerIx);
    // give the victory to the next player
    iNextPlayer = aPlayerDeck[1];

    m_MatchPoints.SetTheWinner(iNextPlayer);

    for (int i = 0; i < m_lNumPlayers; i++) {
        if (m_vctAlgPlayer[i]) {
            m_vctAlgPlayer[i]->ALG_MatchEnd(&m_MatchPoints);
        }
    }
}

void InvidoCore::NtyWaitingPlayer_Toplay(int iPlayerIx) {
    SDL_assert(m_pPlHaveToPlay);
    SDL_assert(iPlayerIx == m_pPlHaveToPlay->GetIndex());

    I_ALG_Player* pAlg = m_vctAlgPlayer[iPlayerIx];
    SDL_assert(pAlg);

    pAlg->ALG_Play();
}

void InvidoCore::NtyWaitingPlayer_ToResp(int iPlayerIx) {
    I_ALG_Player* pAlg = m_vctAlgPlayer[iPlayerIx];
    SDL_assert(pAlg);

    pAlg->ALG_HaveToRespond();
}

void InvidoCore::NtyPlayerSayBuiada(int iPlayerIx) {
    for (int i = 0; i < m_lNumPlayers; i++) {
        if (m_vctAlgPlayer[i]) {
            m_vctAlgPlayer[i]->ALG_PlayerSaidWrong(iPlayerIx);
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
    if (m_Mano.Player_Play(iPlayerIx, TRUE)) {
        // next player is on game
        m_pPlHaveToPlay =
            m_PlayersOnTable.GetPlayerToPlay(PlayersOnTable::SWITCH_TO_NEXT);

        // update match points
        int ixCardVaDentro = 3;
        CARDINFO cardVadodentro;  // quattro di bastoni, carta sempre perdente
        cardVadodentro.byIndex = ixCardVaDentro;
        strncpy(cardVadodentro.CardName, g_CardsNameX[ixCardVaDentro].c_str(),
                NUM_BYTE_NAME);
        cardVadodentro.eSuit = BASTONI;

        m_MatchPoints.PlayerPlay(iPlayerIx, &cardVadodentro);

        for (int i = 0; i < m_lNumPlayers; i++) {
            // notify all players that a card was played
            if (m_vctAlgPlayer[i]) {
                m_vctAlgPlayer[i]->ALG_PlayerHasVadoDentro(iPlayerIx);
            }
        }

        // reset info about played card
        resetCard(iPlayerIx, pCardplayed->GetCardInfo());
        bRes = true;
    }
    return bRes;
}

bool InvidoCore::Player_playCard(int iPlayerIx, const CARDINFO* pCardInfo) {
    CardSpec* pCardplayed = checkValidCardPlayed(iPlayerIx, pCardInfo);
    if (pCardplayed == NULL) {
        return false;
    }

    bool bRes = false;
    // change mano state
    if (m_Mano.Player_Play(iPlayerIx, FALSE)) {
        // next player is on game
        m_pPlHaveToPlay =
            m_PlayersOnTable.GetPlayerToPlay(PlayersOnTable::SWITCH_TO_NEXT);

        // update match points
        m_MatchPoints.PlayerPlay(iPlayerIx, pCardplayed->GetCardInfo());

        for (int i = 0; i < m_lNumPlayers; i++) {
            // notify all players that a card was played
            if (m_vctAlgPlayer[i]) {
                m_vctAlgPlayer[i]->ALG_PlayerHasPlayed(
                    iPlayerIx, pCardplayed->GetCardInfo());
            }
        }

        // reset info about played card
        resetCard(iPlayerIx, pCardplayed->GetCardInfo());

        bRes = true;
    }
    return bRes;
}

bool InvidoCore::Player_saySomething(int iPlayerIx, eSayPlayer eSay) {
    bool bRes = false;
    if (m_Mano.Player_Say(iPlayerIx, eSay)) {
        //  what he said is acceptable on the game
        for (int i = 0; i < m_lNumPlayers; i++) {
            // notify all players that a card was played
            if (m_vctAlgPlayer[i]) {
                m_vctAlgPlayer[i]->ALG_PlayerHasSaid(iPlayerIx, eSay);
            }
        }
        bRes = true;
    }

    return bRes;
}

void InvidoCore::GetAdmittedCommands(VCT_COMMANDS& vct_Commands,
                                     int iPlayerIndex) {
    m_Mano.GetAdmittedCommands(vct_Commands, iPlayerIndex);
}

void InvidoCore::GetMoreCommands(VCT_COMMANDS& vct_Commands, int iPlayerIndex) {
    m_Mano.GetMoreCommands(vct_Commands, iPlayerIndex);
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
        m_PlayersOnTable.GetPlayerToPlay(PlayersOnTable::NO_SWITCH);
    // the player that have to play is the player that becomes the first 3 card
    int iRefPlayerIndex = pCurrPlayer->GetIndex();
    int iBegPos = 0;
    if (iRefPlayerIndex != iPlayer) {
        int iDist = m_PlayersOnTable.CalcDistance(iRefPlayerIndex, iPlayer);
        iBegPos = NUM_CARDS_HAND * iDist;
    }

    m_pMyMazzo->SetIndexRaw(iBegPos, iC1);
    m_pMyMazzo->SetIndexRaw(iBegPos + 1, iC2);
    m_pMyMazzo->SetIndexRaw(iBegPos + 2, iC3);
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
    m_Partita.Reset();
    m_Giocata.Reset();
    m_Mano.Reset();
}

int InvidoCore::Script_CheckResult(int iTypeOfItem, int iParam1,
                                   int iExpectedVal) {
    int iRes = 0;

    return iRes;
}
