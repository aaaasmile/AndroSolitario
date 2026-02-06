

// InvidoCoreEngine.h

#ifndef _C_INVIDOCORE_H_
#define _C_INVIDOCORE_H_

#include "Giocata.h"
#include "InvidoCoreEnv.h"
#include "Mano.h"
#include "MatchPoints.h"
#include "Partita.h"
#include "PlayersOnTable.h"
#include "Probality.h"


class Mazzo;
class TraceService;

/////////////////////////////////////////////////////////////////////////////////////
//   *******************  CINVIDOCORE CLASS
//   ***************************************
/////////////////////////////////////////////////////////////////////////////////////

//! class InvidoCore
/** the core class for the invido. Hold all information about game in progress,
level cards and so on.
*/
class InvidoCore : public I_CORE_Game {
   public:
    InvidoCore();
    ~InvidoCore();

    //! create the engine
    void Create(Player* pHmiPlayer, int iNumPlayers);
    //! provides information about whon won the game
    bool WhoWonsTheGame(Player** ppPlayer);
    //! provides the index of the player that have to play
    bool GetPlayerInPlaying(Player** ppPlayer);
    //! set the game type
    void SetGameType(eGameType eVal) { m_eGameType = eVal; }
    //! provides a player
    Player* GetPlayer(int iIndex) {
        return m_PlayersOnTable.GetPlayerIndex(iIndex);
    }
    //! error message, somithing was wrong
    void RaiseError(const std::string& errorMsg);
    //! provides the number of players
    int GetNumOfPlayers() { return m_lNumPlayers; }
    //! provides match points object
    MatchPoints* GetMatchPointsObj() { return &m_MatchPoints; }
    //! provides the player table
    PlayersOnTable* GetTable() { return &m_PlayersOnTable; }

    // functions related to the script engine
    //! notify event to the script engine
    void NotifyScript(eScriptNotification eVal);
    //! script override deck
    void Script_OverrideDeck(int iPlayer, int iC1, int iC2, int iC3);
    //! script say
    void Script_Say(int iPlayer, eSayPlayer eSay);
    //! script play
    void Script_Play(int iPlayer, CardSpec& CardPlayed);
    //! script notification algorithm
    void NotifyScriptAlgorithm(int iPlayerIx, eScriptNotification eVal);
    //! script match end
    void Script_MatchEnd();
    //! set the start player
    void Script_SetStartPlayer(int iPlayer);
    //! script want to check an item
    int Script_CheckResult(int iTypeOfItem, int iParam1, int iExpectedVal);

    // functions called from Mano
    //! a Mano is terminated
    void Mano_End();
    //! wait  player  have to play
    void NtyWaitingPlayer_Toplay(int iPlayerIx);
    //! wait player responce
    void NtyWaitingPlayer_ToResp(int iPlayerIx);
    //! player say something not correct
    void NtyPlayerSayBuiada(int iPlayerIx);
    //! giocata score is changed
    void ChangeGiocataScore(eGiocataScoreState eNewScore);
    //! player abandon current giocata
    void Player_VaVia(int iPlayerIx);
    //! giocata a monte
    void Giocata_AMonte();

    // functions called from Giocata
    //! a giocata is ended
    void Giocata_End();
    //! start a new giocata: shuffle and distribuite cards
    void Giocata_Start(long lPlayerIx);

    // function called from Partita
    //! partita is terminated
    void Partita_End();

    // functions called from HMI
    //! start a new match
    void NewMatch();
    //! next state
    void NextAction();
    //! set Random seed
    void SetRandomSeed(int iVal) { m_pMyMazzo->SetRandomSeed(iVal); }

    // I_CORE_Game: functions called from algorithm
    //! player say something
    bool Player_saySomething(int iPlayerIx, eSayPlayer eSay);
    //! card is played from a player
    bool Player_playCard(int iPlayerIx, const CARDINFO* pCardInfo);
    bool Player_vaDentro(int iPlayerIx, const CARDINFO* pCardInfo);
    //! provides admitted commands
    void GetAdmittedCommands(VCT_COMMANDS& vct_Commands, int iPlayerIndex);
    void GetMoreCommands(VCT_COMMANDS& vct_Commands, int iPlayerIndex);
    //! player abandon the game
    void AbandonGame(int iPlayerIx);

   private:
    CardSpec* isCardInPlayerHand(int iPlayerIx, const CARDINFO* pCardInfo);
    CardSpec* checkValidCardPlayed(int iPlayerIx, const CARDINFO* pCardInfo);
    void resetCardInfoPlayers();
    bool resetCard(int iPlayerIx, CARDINFO* pCardInfo);
    int getNewMatchFirstPlayer();

   private:
    //! players on table
    PlayersOnTable m_PlayersOnTable;
    //! Game type
    eGameType m_eGameType;
    //! number of players
    long m_lNumPlayers;
    //! player index that must play now
    Player* m_pPlHaveToPlay;
    //! player index that has started the game
    Player* m_pStartPlayer;
    //! level of the game
    eGameLevel m_eGameLevel;
    //! match statistic
    MATCH_STATISTIC m_Match_Stat;
    //! deck
    Mazzo* m_pMyMazzo;
    //! partita
    Partita m_Partita;
    //! giocata
    Giocata m_Giocata;
    //! mano
    Mano m_Mano;
    //! players algorithm
    I_ALG_Player* m_vctAlgPlayer[MAX_NUM_PLAYER];
    //! points handler in match
    MatchPoints m_MatchPoints;
    //! store cards of all players
    CardSpec m_aCardInfo[NUM_CARDS_HAND * MAX_NUM_PLAYER];
    //! tracer service
    TraceService* m_pTracer;
};

#endif