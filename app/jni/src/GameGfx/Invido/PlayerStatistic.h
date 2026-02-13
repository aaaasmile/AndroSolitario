#ifndef _______PLAYER_STATISTIC_H
#define _______PLAYER_STATISTIC_H

#include <iostream>

#include "CardGfx.h"

namespace invido {

class PlayerStatistic {
   public:
    PlayerStatistic();
    void Clone(PlayerStatistic* pNew);
    void Reset();

   public:
    long lScore;
    long lNumHand;
    eSUIT eLastPlayed;
    long bynPointsHad;

    friend std::ostream& operator<<(std::ostream& stream,
                                    const struct tag_cPlayerStatistic& o);
};

inline std::ostream& operator<<(std::ostream& stream,
                                const PlayerStatistic& o) {
    stream << "---------------- new stat ------------------" << std::endl;
    stream << " punti totali = " << o.lScore << std::endl;
    stream << " num mani vinte = " << o.lNumHand << std::endl;
    stream << " punti avuti = " << o.bynPointsHad << std::endl;
    stream << std::endl;

    return stream;
}
typedef struct tag_MATCH_STATISTIC {
    long lTotMatchWons;
    long lTotMatchLose;
    long lTotPoints;
    friend std::ostream& operator<<(std::ostream& stream,
                                    const struct tag_MATCH_STATISTIC& o);
} MATCH_STATISTIC;

inline std::ostream& operator<<(std::ostream& stream,
                                const tag_MATCH_STATISTIC& o) {
    stream << "---------------- new stat ------------------" << std::endl;
    stream << " partite vinte = " << o.lTotMatchWons << std::endl;
    stream << " partite perse = " << o.lTotMatchLose << std::endl;
    stream << " punti fatti = " << o.lTotPoints << std::endl;
    stream << std::endl;

    return stream;
}

}

#endif
