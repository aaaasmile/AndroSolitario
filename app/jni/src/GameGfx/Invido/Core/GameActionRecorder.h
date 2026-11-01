#ifndef INVIDO_GAME_ACTION_RECORDER_H
#define INVIDO_GAME_ACTION_RECORDER_H

#include <string>
#include <vector>

#include "../../../Lib/Json/Json.h"
#include "ErrorInfo.h"
#include "InvidoCoreDef.h"

namespace invido {

class PlayersOnTable;

class GameActionRecorder {
   public:
    GameActionRecorder();

    LPErrInApp Initialize(PlayersOnTable& playersOnTable,
                          const std::string& settingsDirectory,
                          const std::string& gameName);
    LPErrInApp InitializeAtPath(PlayersOnTable& playersOnTable,
                                const std::string& filePath);
    void RecordAction(const std::string& source, const std::string& action,
                      const invjson::Value::Object& details =
                          invjson::Value::Object());
    LPErrInApp FlashActions();
    LPErrInApp Finalize(const std::string& outcome);
    bool IsInitialized() const;
    const std::string& GetFilePath() const;
    const std::vector<invjson::Value>& GetActions() const;
    LPErrInApp RecordGiocataDeck(const std::vector<Uint8>& cardIndices);

   private:
    LPErrInApp WriteDocument();

    invjson::Value _document;
    std::vector<invjson::Value> _vctActions;
    std::string _filePath;
    bool _isInitialized;
    bool _isFinalized;
};

}  // namespace invido

#endif
