#ifndef ___CARDSPEC_H_
#define ___CARDSPEC_H_

#include "Config.h"
#include "InvidoCoreEnv.h"

typedef struct _CardInfo {
    unsigned char byIndex;
    eSUIT eSuit;
    std::string CardName;
} CARDINFO;

class CardSpec {
   public:
    CardSpec();
    virtual ~CardSpec() {};
    void SetCardIndex(int itmpIndex);
    int GetCardIndex() { return m_CardInfo.byIndex; }
    eSUIT GetSuit() { return m_CardInfo.eSuit; }
    LPCSTR GetName() { return m_CardInfo.CardName.c_str(); }
    void operator=(const CardSpec& r);
    bool operator==(const CardSpec& r) {
        return m_CardInfo.byIndex == r.m_CardInfo.byIndex;
    }
    bool operator!=(const CardSpec& r) {
        return m_CardInfo.byIndex != r.m_CardInfo.byIndex;
    }
    void Reset();
    void SetSymbol(int iVal) { m_CardInfo.byIndex = iVal; }
    int GetSymbol() { return m_CardInfo.byIndex; }
    CARDINFO* GetCardInfo() { return &m_CardInfo; }
    void SetCardInfo(const CARDINFO& Card);
    void FillInfo(CARDINFO* pCardInfo);
    int GetPoints();

   private:
    CARDINFO m_CardInfo;
};

typedef std::vector<CardSpec> VCT_CARDSPEC;
typedef std::vector<char> VCT_SINGLECARD;
typedef std::vector<char>::iterator IT_SINGLE_CARD;
typedef std::vector<VCT_SINGLECARD>::iterator IT_SINGLE_HAND;

#endif