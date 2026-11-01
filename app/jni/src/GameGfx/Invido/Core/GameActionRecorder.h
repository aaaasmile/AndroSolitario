#ifndef INVIDO_GAME_ACTION_RECORDER_H
#define INVIDO_GAME_ACTION_RECORDER_H

#include <string>
#include <vector>

#include "../../../Lib/Json/Json.h"
#include "InvidoCoreDef.h"

namespace invido {

class PlayersOnTable;

class GameActionRecorder {
   public:
    GameActionRecorder();

    bool Initialize(const std::vector<Uint8>& cardIndices,
                    PlayersOnTable& playersOnTable,
                    const std::string& settingsDirectory,
                    const std::string& gameName, std::string& errorMessage);
    bool InitializeAtPath(const std::vector<Uint8>& cardIndices,
                          PlayersOnTable& playersOnTable,
                          const std::string& filePath,
                          std::string& errorMessage);
    void RecordAction(const std::string& source, const std::string& action,
                      const invjson::Value::Object& details =
                          invjson::Value::Object());
    bool Finalize(const std::string& outcome, std::string& errorMessage);
    const std::string& GetFilePath() const;
    const std::vector<invjson::Value>& GetActions() const;
    void RecordGiocataDeck(const std::vector<Uint8>& cardIndices);

   private:
    bool WriteDocument(std::string& errorMessage);

    invjson::Value _document;
    std::vector<invjson::Value> _vctActions;
    std::string _filePath;
    bool _isInitialized;
    bool _isFinalized;
};

}  // namespace invido

#endif
