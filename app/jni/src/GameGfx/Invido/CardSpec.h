#ifndef ___CARDSPEC_H_
#define ___CARDSPEC_H_

#include "CardGfx.h"
#include "Config.h"

typedef struct _CardInfo {
    int byIndex;
    eSUIT eSuit;
    std::string CardName;
} CARDINFO;

class CardSpec {
   public:
    CardSpec();
    virtual ~CardSpec() {};
    void SetCardIndex(int itmpIndex);
    int GetCardIndex() { return _cardInfo.byIndex; }
    eSUIT GetSuit() { return _cardInfo.eSuit; }
    LPCSTR GetName() { return _cardInfo.CardName.c_str(); }
    void operator=(const CardSpec& r);
    bool operator==(const CardSpec& r) {
        return _cardInfo.byIndex == r._cardInfo.byIndex;
    }
    bool operator!=(const CardSpec& r) {
        return _cardInfo.byIndex != r._cardInfo.byIndex;
    }
    void Reset();
    void SetSymbol(int iVal) { _cardInfo.byIndex = iVal; }
    int GetSymbol() { return _cardInfo.byIndex; }
    CARDINFO* GetCardInfo() { return &_cardInfo; }
    void SetCardInfo(const CARDINFO& Card);
    void FillInfo(CARDINFO* pCardInfo);
    int GetPoints();

   private:
    CARDINFO _cardInfo;
};

typedef std::vector<CardSpec> VCT_CARDSPEC;
typedef std::vector<char> VCT_SINGLECARD;
typedef std::vector<char>::iterator IT_SINGLE_CARD;
typedef std::vector<VCT_SINGLECARD>::iterator IT_SINGLE_HAND;

#endif