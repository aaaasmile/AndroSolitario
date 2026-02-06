// CardSpec.cpp

#include "CardSpec.h"

CardSpec::CardSpec() { Reset(); }

void CardSpec::SetCardIndex(int itmpIndex) {
    if (itmpIndex >= 0 && itmpIndex <= NUM_CARDS_MAZZBRI) {
        m_CardInfo.byIndex = itmpIndex;
    } else {
        return;
    }

    if (itmpIndex >= 0 && itmpIndex < NUM_CARDS_MAZZBRI) {
        strcpy(m_CardInfo.CardName,
               const_cast<CHAR*>(g_CardsNameX[itmpIndex].c_str()));
    }

    if (m_CardInfo.byIndex >= 0 && m_CardInfo.byIndex < 10) {
        m_CardInfo.eSuit = BASTONI;
    } else if (m_CardInfo.byIndex >= 10 && m_CardInfo.byIndex < 20) {
        m_CardInfo.eSuit = COPPE;
    } else if (m_CardInfo.byIndex >= 20 && m_CardInfo.byIndex < 30) {
        m_CardInfo.eSuit = DENARI;
    } else if (m_CardInfo.byIndex >= 30 && m_CardInfo.byIndex < 40) {
        m_CardInfo.eSuit = SPADE;
    } else {
        m_CardInfo.eSuit = UNDEF;
    }
}

////////////////////////////////////////
//       =
/*! Assign operator
// \param const CardSpec &r :
*/
void CardSpec::operator=(const CardSpec& r) {
    strncpy(m_CardInfo.CardName, r.m_CardInfo.CardName, NUM_BYTE_NAME);
    m_CardInfo.byIndex = r.m_CardInfo.byIndex;
    m_CardInfo.eSuit = r.m_CardInfo.eSuit;
}

void CardSpec::Reset() {
    m_CardInfo.byIndex = NOT_VALID_INDEX;
    m_CardInfo.CardName[0] = '\0';
    m_CardInfo.eSuit = UNDEF;
}

void CardSpec::SetCardInfo(const CARDINFO& Card) {
    strncpy(m_CardInfo.CardName, Card.CardName, NUM_BYTE_NAME);
    m_CardInfo.byIndex = Card.byIndex;
    m_CardInfo.eSuit = Card.eSuit;
}

void CardSpec::FillInfo(CARDINFO* pCardInfo) {
    ASSERT(pCardInfo);
    strncpy(pCardInfo->CardName, m_CardInfo.CardName, NUM_BYTE_NAME);
    pCardInfo->byIndex = m_CardInfo.byIndex;
    pCardInfo->eSuit = m_CardInfo.eSuit;
}

int CardSpec::GetPoints() {
    int ix = GetCardIndex();
    ASSERT(ix >= 0 && ix < 40)
    return g_PointsTable[ix];
}
