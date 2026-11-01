#ifndef ___CARDSPEC_H_
#define ___CARDSPEC_H_

#include <SDL3/SDL.h>

#include <vector>

#include "Config.h"


namespace invido {
class CardSpec {
   public:
    CardSpec();
    void SetCardIndex(Uint8 index);
    int GetCardIndex() const { return _index; }
    eSUIT GetSuit() const { return _eSuit; }
    std::string GetName() const { return _cardName; }
    bool operator==(const CardSpec& r) const { return _index == r._index; }
    bool operator!=(const CardSpec& r) const { return _index != r._index; }
    int GetPoints() const;

   private:
    Uint8 _index;
    eSUIT _eSuit;
    std::string _cardName;
};

typedef std::vector<CardSpec> VCT_CARDSPEC;
}  // namespace invido
#endif