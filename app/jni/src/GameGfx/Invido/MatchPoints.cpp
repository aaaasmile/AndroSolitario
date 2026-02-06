

// MatchPoints.cpp: implementation of the MatchPoints class.
//
//////////////////////////////////////////////////////////////////////

#include "Mano.h"
#include "MatchPoints.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MatchPoints::MatchPoints() {
    m_iNumPlayers = NUM_PLAY_INVIDO_2;
    m_pMano = 0;
    m_bGameAbandoned = FALSE;
    for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
        m_ManoDetailInfo[iManoNum].Reset();
    }
    m_eCurrentScore = SC_CANELA;
}

MatchPoints::~MatchPoints() {}

void MatchPoints::MatchStart(int iNumPlayer) {
    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        m_vctPlayerPoints[i] = 0;
    }
    m_iNumPlayers = iNumPlayer;
    m_iPlayerMatchWin = NOT_VALID_INDEX;
    m_iScoreGoal = SCORE_GOAL;
    m_bMatchInSpecialScore = FALSE;
    m_vctGiocataInfo.clear();
}

void MatchPoints::GiocataStart() {
    CardSpec CardUndef;
    for (int i = 0; i < MAX_NUM_PLAYER; i++) {
        m_vctHandWons[i] = 0;
        m_vctCardPlayed[i].cardSpec = CardUndef;
        m_vctCardPlayed[i].iPlayerIx = NOT_VALID_INDEX;
    }
    m_iNumCardsPlayed = 0;
    m_eCurrentScore = SC_CANELA;
    m_iPlayerWonsHand = NOT_VALID_INDEX;
    m_iPlayerFirstHand = NOT_VALID_INDEX;
    m_iPlayerGiocataWin = NOT_VALID_INDEX;
    m_eIsGiocataEnd = GES_ON_GOING;
    m_bIsManoPatatda = FALSE;
    m_iManoRound = 0;
    m_bOldManoPatada = FALSE;
    for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
        m_ManoDetailInfo[iManoNum].Reset();
    }
    m_iPlayerChangeScore = NOT_VALID_INDEX;
    m_bGameAbandoned = FALSE;
}

void MatchPoints::PlayerPlay(int iPlayerIx, CARDINFO* pCard) {
    CardSpec Card;
    ASSERT(pCard);
    Card.SetCardInfo(*pCard);
    ASSERT(m_iNumCardsPlayed < MAX_NUM_PLAYER && m_iNumCardsPlayed >= 0);
    m_vctCardPlayed[m_iNumCardsPlayed].iPlayerIx = iPlayerIx;
    m_vctCardPlayed[m_iNumCardsPlayed].cardSpec = Card;
    m_iNumCardsPlayed++;
}

void MatchPoints::ManoEnd() {
    ASSERT(m_iNumCardsPlayed == m_iNumPlayers);
    int iManoTerminatedIndex = m_iManoRound;
    ASSERT(iManoTerminatedIndex >= 0 && iManoTerminatedIndex < NUM_CARDS_HAND);
    m_iManoRound++;
    // hand is terminated
    if (m_iNumPlayers == NUM_PLAY_INVIDO_2)  // consistency check
    {
        int iPoints_1 =
            g_PointsTable[m_vctCardPlayed[PLAYER1].cardSpec.GetCardIndex()];
        int iPlayer_1 = m_vctCardPlayed[PLAYER1].iPlayerIx;
        int iPoints_2 =
            g_PointsTable[m_vctCardPlayed[PLAYER2].cardSpec.GetCardIndex()];
        int iPlayer_2 = m_vctCardPlayed[PLAYER2].iPlayerIx;
        m_bIsManoPatatda = FALSE;
        // mano is played
        m_ManoDetailInfo[iManoTerminatedIndex].bIsPlayed = TRUE;

        if (iPoints_1 == iPoints_2) {
            // nobody wins the hand
            m_bIsManoPatatda = TRUE;
            if (m_iPlayerFirstHand != NOT_VALID_INDEX) {
                // the first player who take the mano is the giocata winner
                m_iPlayerGiocataWin = m_iPlayerFirstHand;
                m_eIsGiocataEnd = GES_HAVE_WINNER;
            }
            m_iPlayerWonsHand = NOT_VALID_INDEX;
            m_ManoDetailInfo[iManoTerminatedIndex].bIsPata = TRUE;

            // mark the mano patada, the next who take the trick win
            m_bOldManoPatada = TRUE;
        } else if (iPoints_1 > iPoints_2) {
            // first player wons the mano
            m_vctHandWons[iPlayer_1]++;
            if (m_iPlayerFirstHand == NOT_VALID_INDEX) {
                m_iPlayerFirstHand = iPlayer_1;
            }
            if (m_vctHandWons[iPlayer_1] >= NUM_PLAY_INVIDO_2 ||
                m_bOldManoPatada) {
                // giocata is terminated, giocata winner is the first player in
                // this hand
                m_iPlayerGiocataWin = iPlayer_1;
                m_eIsGiocataEnd = GES_HAVE_WINNER;
            }
            m_iPlayerWonsHand = iPlayer_1;
            m_ManoDetailInfo[iManoTerminatedIndex].iPlayerIndex = iPlayer_1;
        } else {
            // second player catch the mano
            m_vctHandWons[iPlayer_2]++;
            if (m_iPlayerFirstHand == NOT_VALID_INDEX) {
                m_iPlayerFirstHand = iPlayer_2;
            }
            if (m_vctHandWons[iPlayer_2] >= NUM_PLAY_INVIDO_2 ||
                m_bOldManoPatada) {
                // giocata is terminated, giocata winner is the second player in
                // this hand
                m_iPlayerGiocataWin = iPlayer_2;
                m_eIsGiocataEnd = GES_HAVE_WINNER;
            }
            m_iPlayerWonsHand = iPlayer_2;
            m_ManoDetailInfo[iManoTerminatedIndex].iPlayerIndex = iPlayer_2;
        }
        if (m_iManoRound >= NUM_CARDS_HAND) {
            // giocata is terminated
            if (m_bIsManoPatatda && (m_iPlayerFirstHand == NOT_VALID_INDEX)) {
                // strange case all hands was patadi. Giocata is also patada.
                m_eIsGiocataEnd = GES_PATADA;
                m_iPlayerGiocataWin = iPlayer_1;
            } else {
                // giocata winner must be already defined
                ASSERT(m_iPlayerGiocataWin != NOT_VALID_INDEX);
            }
        }
    } else {
        ASSERT(0);
    }
    // reset info about mano
    m_iNumCardsPlayed = 0;
}

void MatchPoints::PlayerVaVia(int iPlayerIx) {
    m_eIsGiocataEnd = GES_HAVE_WINNER;
    if (iPlayerIx == PLAYER1) {
        m_iPlayerGiocataWin = PLAYER2;
    } else {
        m_iPlayerGiocataWin = PLAYER1;
    }
}

void MatchPoints::GiocataEnd() {
    if (m_eIsGiocataEnd == GES_HAVE_WINNER) {
        ASSERT(m_iPlayerGiocataWin != NOT_VALID_INDEX);
        // update the score
        m_vctPlayerPoints[m_iPlayerGiocataWin] += m_eCurrentScore;
        m_vctGiocataInfo.push_back(
            GiocataInfo(m_iPlayerGiocataWin, m_eCurrentScore));

        if (m_vctPlayerPoints[m_iPlayerGiocataWin] >= m_iScoreGoal) {
            // match is terminated
            m_iPlayerMatchWin = m_iPlayerGiocataWin;
        } else if (m_vctPlayerPoints[PLAYER1] == SPECIAL_SCORE &&
                   m_vctPlayerPoints[PLAYER2] == SPECIAL_SCORE) {
            // special condition both player are on 23 to 23 or after 7 to 7....
            beginSpecialTurn();
        } else if (m_bMatchInSpecialScore &&
                   (m_vctPlayerPoints[PLAYER1] == SCORE_SEVEN &&
                    m_vctPlayerPoints[PLAYER2] == SCORE_SEVEN)) {
            beginSpecialTurn();
        }
    } else {
        // pata or monte
        m_vctGiocataInfo.push_back(GiocataInfo(NOT_VALID_INDEX, SC_AMONTE));
    }
    for (int iManoNum = 0; iManoNum < NUM_CARDS_HAND; iManoNum++) {
        m_ManoDetailInfo[iManoNum].Reset();
    }
}

bool MatchPoints::IsGiocatEnd() {
    bool bRet = FALSE;

    if (m_eIsGiocataEnd == GES_AMONTE || m_eIsGiocataEnd == GES_PATADA ||
        m_eIsGiocataEnd == GES_HAVE_WINNER) {
        bRet = TRUE;
    }
    return bRet;
}

void MatchPoints::AMonte() { m_eIsGiocataEnd = GES_AMONTE; }

void MatchPoints::beginSpecialTurn() {
    m_iScoreGoal = SPECIAL_SCORE_GOAL;
    m_vctPlayerPoints[PLAYER1] = 0;
    m_vctPlayerPoints[PLAYER2] = 0;
    m_bMatchInSpecialScore = TRUE;
}

void MatchPoints::GetManoInfo(int iManoNum, int* piPlayerIx, bool* pbIsPlayed,
                               bool* pbIsPata) {
    ASSERT(pbIsPata);
    ASSERT(pbIsPlayed);
    ASSERT(piPlayerIx);
    if (iManoNum >= 0 && iManoNum < NUM_CARDS_HAND) {
        *pbIsPata = m_ManoDetailInfo[iManoNum].bIsPata;
        *pbIsPlayed = m_ManoDetailInfo[iManoNum].bIsPlayed;
        *piPlayerIx = m_ManoDetailInfo[iManoNum].iPlayerIndex;

    } else {
        ASSERT(0);
    }
}

void MatchPoints::GetGiocataInfo(int iNumGiocata, GiocataInfo* pGiocInfo) {
    ASSERT(pGiocInfo);
    if (iNumGiocata >= 0 && iNumGiocata < (int)m_vctGiocataInfo.size()) {
        *pGiocInfo = m_vctGiocataInfo[iNumGiocata];
    }
}

void MatchPoints::ChangeCurrentScore(eGiocataScoreState eVal, int iPlayer) {
    if (eVal > 0) {
        ASSERT(iPlayer != m_iPlayerChangeScore);
    }
    m_eCurrentScore = eVal;
    m_iPlayerChangeScore = iPlayer;
}

void MatchPoints::SetTheWinner(int iPlayerIx) {
    m_iPlayerMatchWin = iPlayerIx;
    m_bGameAbandoned = TRUE;
}
