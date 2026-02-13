#ifndef ___CARDSPEC_H_
#define ___CARDSPEC_H_

#include <vector>

class CardSpec {
   public:
    CardSpec();
    void SetCardIndex(Uint8 index);
    int GetCardIndex() { return _index; }
    eSUIT GetSuit() { return _eSuit; }
    std::string GetName() { return _cardName; }
    void operator=(const CardSpec& r);
    bool operator==(const CardSpec& r) { return _index == r._index; }
    bool operator!=(const CardSpec& r) { return _index != r._index; }
    int GetPoints();

   private:
    Uint8 _index;
    eSUIT _eSuit;
    std::string _cardName;
};

typedef std::vector<CardSpec> VCT_CARDSPEC;
#endif