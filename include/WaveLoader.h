// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <string>
#include <utility>

#include "Waves.h"


namespace Engine
{
  class WaveLoader
  {
  public:

    WaveLoader(const std::string & filePath);

    Wave & getWave(size_t waveNum);
    Wave & getWave(const std::string & waveName);

    std::string getWaveName(size_t waveNum) const;
    size_t size() const;

  private:
    using WAVE_DATA = std::pair<std::string, Wave>;

    void parseFile(const std::string & filePath);

    std::vector<WAVE_DATA> waves_;
  };
}
