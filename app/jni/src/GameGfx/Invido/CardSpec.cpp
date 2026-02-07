#include "CardSpec.h"

#include "CardGfx.h"
#include "InvidoCoreEnv.h"

static const std::string g_CardsNameX[] = {
    "Asso di bastoni",    "Due di bastoni",     "Tre di bastoni",
    "Quattro di bastoni", "Cinque di bastoni",  "Sei di bastoni",
    "Sette di bastoni",   "Fante di bastoni",   "Cavallo di bastoni",
    " Re di bastoni",     "Asso di  coppe",     "Due di  coppe",
    "Tre di  coppe",      "Quattro di  coppe",  "Cinque di  coppe",
    "Sei di  coppe",      "Sette di  coppe",    "Fante di  coppe",
    "Cavallo di  coppe",  " Re di  coppe",      "Asso di  denari",
    "Due di  denari",     "Tre di  denari",     "Quattro di  denari",
    "Cinque di  denari",  "Sei di  denari",     "Sette di  denari",
    "Fante di  denari",   "Cavallo di  denari", " Re di  denari",
    "Asso di  spade",     "Due di  spade",      "Tre di  spade",
    "Quattro di  spade",  "Cinque di  spade",   "Sei di  spade",
    "Sette di  spade",    "Fante di  spade",    "Cavallo di  spade",
    " Re di  spade"};

CardSpec::CardSpec() { Reset(); }

void CardSpec::SetCardIndex(int itmpIndex) {
    SDL_assert(itmpIndex >= 0 && itmpIndex <= eGameConst::NUM_CARDS_MAZZBRI);
    if (itmpIndex >= 0 && itmpIndex <= eGameConst::NUM_CARDS_MAZZBRI) {
        m_CardInfo.byIndex = itmpIndex;
    } else {
        return;
    }

    if (itmpIndex >= 0 && itmpIndex < eGameConst::NUM_CARDS_MAZZBRI) {
        m_CardInfo.CardName = g_CardsNameX[itmpIndex];
    }

    if (m_CardInfo.byIndex >= 0 && m_CardInfo.byIndex < 10) {
        m_CardInfo.eSuit = eSUIT::BASTONI;
    } else if (m_CardInfo.byIndex >= 10 && m_CardInfo.byIndex < 20) {
        m_CardInfo.eSuit = eSUIT::COPPE;
    } else if (m_CardInfo.byIndex >= 20 && m_CardInfo.byIndex < 30) {
        m_CardInfo.eSuit = eSUIT::DENARI;
    } else if (m_CardInfo.byIndex >= 30 && m_CardInfo.byIndex < 40) {
        m_CardInfo.eSuit = eSUIT::SPADE;
    }
}

void CardSpec::operator=(const CardSpec& r) {
    m_CardInfo.CardName = r.m_CardInfo.CardName;
    m_CardInfo.byIndex = r.m_CardInfo.byIndex;
    m_CardInfo.eSuit = r.m_CardInfo.eSuit;
}

void CardSpec::Reset() {
    m_CardInfo.byIndex = eGameConst::NOT_VALID_INDEX;
    m_CardInfo.CardName = "";
    m_CardInfo.eSuit = eSUIT::BASTONI;
}

void CardSpec::SetCardInfo(const CARDINFO& Card) {
    m_CardInfo.CardName = Card.CardName;
    m_CardInfo.byIndex = Card.byIndex;
    m_CardInfo.eSuit = Card.eSuit;
}

void CardSpec::FillInfo(CARDINFO* pCardInfo) {
    SDL_assert(pCardInfo);
    pCardInfo->CardName = m_CardInfo.CardName;
    pCardInfo->byIndex = m_CardInfo.byIndex;
    pCardInfo->eSuit = m_CardInfo.eSuit;
}

int CardSpec::GetPoints() {
    int ix = GetCardIndex();
    SDL_assert(ix >= 0 && ix < 40);
    return g_PointsTable[ix];
}
