#ifndef _______PLAYER_STATISTIC_H
#define _______PLAYER_STATISTIC_H

#include "AlgPlayerInterface.h"
#include "InvidoCoreEnv.h"

class PlayerStatistic {
   public:
    PlayerStatistic();
    void Clone(PlayerStatistic* pNew);
    void Reset();

   public:
    long lScore;
    long lNumHand;
    long lNumBrisc;
    long lNumCarac;
    long bynStroz[NUM_SEEDS];
    long bynRe[NUM_SEEDS];
    long bynHorse[NUM_SEEDS];
    long bynFanti[NUM_SEEDS];
    long lRemNumBrisc;
    long lRemNumPoints;
    eSUIT eLastPlayed;
    long bynBriscHad;
    long bynPointsHad;
    long bynBriscPointsHad;
    long bynBriscFirstHad;
    long lCaracHad;
    long lInHand_nBris;
    long lInHand_nCarac;
    long lInHand_nLisc;
    long lSegniWons;

    friend std::ostream& operator<<(std::ostream& stream,
                                    const struct tag_cPlayerStatistic& o);
};

inline std::ostream& operator<<(std::ostream& stream,
                                const PlayerStatistic& o) {
    stream << "---------------- new stat ------------------" << std::endl;
    stream << " punti totali = " << o.lScore << std::endl;
    stream << " num mani vinte = " << o.lNumHand << std::endl;
    stream << " num briscole prese = " << o.lNumBrisc << std::endl;
    stream << " num carichi presi = " << o.lNumCarac << std::endl;
    stream << " num briscole avute = " << o.bynBriscHad << std::endl;
    stream << " punti in briscole avuti = " << o.bynBriscPointsHad << std::endl;
    stream << " punti avuti = " << o.bynPointsHad << std::endl;
    stream << " num carichi giocati = " << o.lCaracHad << std::endl;
    stream << " briscole in prima mano = " << o.bynBriscFirstHad << std::endl;
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

#endif
