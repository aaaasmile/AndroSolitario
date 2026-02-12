#ifndef ___CARDSPEC_H_
#define ___CARDSPEC_H_

class CardSpec {
   public:
    CardSpec();
    void SetCardIndex(Uint8 index);
    int GetCardIndex() { return _index; }
    eSUIT GetSuit() { return _eSuit; }
    LPCSTR GetName() { return _cardName.c_str(); }
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
typedef std::vector<char> VCT_SINGLECARD;
typedef std::vector<char>::iterator IT_SINGLE_CARD;
typedef std::vector<VCT_SINGLECARD>::iterator IT_SINGLE_HAND;

#endif