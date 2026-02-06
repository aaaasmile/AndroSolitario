// cMazzo.cpp

#include <win_type_global.h>

#include "InvidoSettings.h"
#include "cInvidoCore.h"
#include "cMazzo.h"

////////////////////////////////////////////////////////////////
//   *******************      cMazzo CLASS *********************
////////////////////////////////////////////////////////////////

cMazzo::cMazzo() {
    m_lNextCard = 0;
    m_pCoreGame = 0;
    m_iRndSeed = 63200;
}

void cMazzo::Create() {
    m_vctCards.reserve(NUM_CARDS);
    m_vctCards.clear();

    // invido card index
    int aCardIndex[] = {0,  1,  2,  5,  6,  7,  8,  9,  10, 11, 12,
                        15, 16, 17, 18, 19, 20, 21, 22, 25, 26, 27,
                        28, 29, 30, 31, 32, 35, 36, 37, 38, 39};

    for (int i = 0; i < NUM_CARDS; i++) {
        m_vctCards.push_back(aCardIndex[i]);
    }

    m_lNextCard = 0;
}

void cMazzo::SetIndexRaw(int iIndex, long lVal) {
    if (iIndex < (int)m_vctCards.size() && iIndex >= 0) {
        m_vctCards[iIndex] = lVal;
    }
}

bool cMazzo::CloneFrom(cMazzo& Master) {
    m_lNextCard = Master.GetNextCardVal();
    m_vctCards = Master.GetVectorIndexes();

    return true;
}

bool cMazzo::Shuffle() {
    IT_VCTLONG it_tmp;

    m_lNextCard = 0;

    it_tmp = m_vctCards.begin();
    // Leave the deck card to the first position

    // use the rand() function to shuffle the pool (deck is not shuffled)
    std::random_shuffle(it_tmp, m_vctCards.end());

#ifndef SERVER_PRG
    if (g_Options.All.iDebugLevel > 3) {
        Utility::TraceContainer(m_vctCards, "Cards mazzo");
    }
#endif

    // call a callback in python script for shuffle deck
    m_pCoreGame->NotifyScript(SCR_NFY_SHUFFLEDECK);

    return true;
}

long cMazzo::PickNextCard(bool* pbEnd) {
    long lResult = NOT_VALID_INDEX;
    if (m_lNextCard >= (long)m_vctCards.size()) {
        *pbEnd = FALSE;
        return lResult;
    }
    *pbEnd = TRUE;

    lResult = m_vctCards[m_lNextCard];
    m_lNextCard++;

    return lResult;
}

bool cMazzo::PickNextCard(CardSpec* pRes) {
    ASSERT(pRes);

    pRes->Reset();
    bool bValid = FALSE;

    if (m_lNextCard < (long)m_vctCards.size()) {
        bValid = TRUE;
        long lIndex = m_vctCards[m_lNextCard];

        pRes->SetCardIndex(lIndex);

        m_lNextCard++;
    }

    return bValid;
}

long cMazzo::ThrowTableCard() {
    long lResult = NOT_VALID_INDEX;
    if (m_lNextCard >= (long)m_vctCards.size()) {
        return lResult;
    }

    lResult = m_vctCards[m_lNextCard];

    // swap the next card with the last
    long lLast = m_vctCards[NUM_CARDS - 1];
    m_vctCards[NUM_CARDS - 1] = m_vctCards[m_lNextCard];
    m_vctCards[m_lNextCard] = lLast;

    return lResult;
}

void cMazzo::TraceIt() { Utility::TraceContainer(m_vctCards, "Cards mazzo"); }

bool cMazzo::IsMoreCards() {
    bool bRet = false;
    if (m_lNextCard < NUM_CARDS) {
        bRet = true;
    }
    return bRet;
}

long cMazzo::GetIndexNextCard(bool* pbEnd) {
    long lResult = NOT_VALID_INDEX;
    if (m_lNextCard >= NUM_CARDS) {
        *pbEnd = FALSE;
        return lResult;
    }
    *pbEnd = TRUE;

    lResult = m_vctCards[m_lNextCard];

    return lResult;
}
