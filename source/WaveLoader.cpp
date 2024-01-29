// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <fstream>

#include "WaveLoader.h"
#include "json/json.h"
#include "Logger.h"
#include "EnemyLogic.h"

namespace Engine
{
  using namespace Logger;

  struct WaveInfo : Info
  {
    static const char * levelId() { return "WAVE INFO"; };

  };

  struct WaveWarning : Warning
  {
    static const char * levelId() { return "WAVE WARNING"; };
  };

  struct WaveError : Error
  {
    static const char * levelId() { return "WAVE ERROR"; };
  };


  WaveLoader::WaveLoader(const std::string & filePath)
  {
    parseFile(filePath);
  }

  Wave & WaveLoader::getWave(size_t waveNum)
  {
    return waves_[waveNum].second;
  }

  Wave & WaveLoader::getWave(const std::string & waveName)
  {
    for (auto & wave : waves_)
      if (wave.first == waveName)
        return wave.second;
    
    throw std::runtime_error("Invalid wave name");
  }

  std::string WaveLoader::getWaveName(size_t waveNum) const
  {
    return waves_[waveNum].first;
  }

  size_t WaveLoader::size() const
  {
    return waves_.size();
  }

  void WaveLoader::parseFile(const std::string & filePath)
  {

    std::vector<WAVE_DATA> newWaves;

    std::ifstream waveDefs{ filePath };
    Json::Reader reader;
    Json::Value root;
    const Json::Value defValue;

    if (waveDefs.is_open())
    {
      reader.parse(waveDefs, root);

      std::vector<std::string> waveNames{ root.getMemberNames() };

      for (auto & waveName : waveNames)
      {
        // Should be an array of spawn data
        Json::Value waveData = root.get(waveName, defValue);
        std::vector<float> delays;
        std::vector<EnemySpawnFlags> flags;
        std::vector<ObjectSpawnInfo> info;
        std::vector<int> positions;

        // Make sure the data is the correct format
        if (waveData.isArray())
        {
          for (auto spawnInfo : waveData)
          {
            try
            {
              // Load spawining info from the list
              std::string type = spawnInfo.get("type", defValue).asString();
              int position = spawnInfo.get("position", defValue).asInt();
              float delay = spawnInfo.get("delay", defValue).asFloat();

              EnemySpawnFlags enemyType;
              ObjectSpawnInfo objInfo;

              ReturnEnemyArchetype(&objInfo, &enemyType, type);

              delays.push_back(delay);
              flags.push_back(enemyType);
              info.push_back(objInfo);
              positions.push_back(position);
            }
            catch (const std::exception & e)
            {
              Log<WaveWarning>("Failed to load spawn info in %s! What: %s", filePath.c_str(), e.what());
            }
          }

          // Push the new wave to the wave list
          newWaves.push_back(std::make_pair(waveName, Wave{ delays, flags, info, positions }));

          Log <WaveInfo>("Loaded wave %s from file %s", waveName.c_str(), filePath.c_str());
        }
        else
        {
          Log<WaveError>("Wave &s in file %s is malformed", waveName.c_str(), filePath.c_str());
          throw std::runtime_error("Malformed wave data");
        }
      }

      waves_.swap(newWaves);
    }
    else
    {
      Log<WaveError>("Could not open wave definition file %s for reading", filePath.c_str());
      throw std::runtime_error("Could not open wave definition file");
    }
  }
}
