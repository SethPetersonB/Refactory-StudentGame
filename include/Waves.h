// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include "EnemyLogic.h"
#include "GameInstance.h"
#include "Timer.h"

namespace Engine
{
  class Wave;

  // Unsigned long for enemy instance ID, size_t for wave number
  typedef std::vector<std::pair<unsigned long, size_t>> ENEMY_LIST;

  class WaveController : public Component
  {
  public:
    WaveController(GameInstance * owner);
    WaveController( std::vector<Wave>* WaveVector, Stage * stage, Grid * grid, int index = 0 );
    ~WaveController();

    size_t CurrentWave() const;
    size_t NumWaves() const { return wv_.size(); }

    size_t CurrentWaveEnemy() const;
    size_t WaveSize() const;

    bool IsWaveOver() const;
    void StartWave(size_t waveNum);
    void SetWave(size_t wave);
    void NextWave();

    void SetWaveRunning(bool running);

    bool IsWaveRunning() const { return running_; }

    void AddWave(const Wave & wave);

    void UpdateWave();
    void SetIndex( int index );
    int GetIndex() const;

    void KillWave(size_t wave);
    void KillAll();
    bool IsWaveDead(size_t wave) const;

    void OnStagePaused(const Packet & payload);

  private:
    std::vector<Wave> wv_;
    ENEMY_LIST enemies_;
    size_t currentWave_;
    Timer waveTimer_;
    bool running_;
    //Stage* stg_;
    //Grid* grd_;
    int i_;

  };

  class WaveControllerHandler : public ComponentHandler
  {
  public:
    WaveControllerHandler(Stage * owner);

    void update();
    void ConnectEvents(Component * base_sub);

  };

  class Wave
  {
  public:
    Wave( std::vector<float> sd, std::vector<EnemySpawnFlags> sf, std::vector<ObjectSpawnInfo> si, std::vector<int> sp );
    ~Wave();

    float GetDelay() const;
    EnemySpawnFlags* GetFlags();
    ObjectSpawnInfo* GetInfo();
    int GetPosition() const;
    size_t GetSize() const;

    bool IsWaveOver() const { return currentEnemy_ >= size; }

    void ResetWave();
    void SetWavePos(size_t pos);

    void RemoveFirst();
    unsigned long SpawnNext(Stage * stage);

    size_t getCurrentEnemy() const { return currentEnemy_; }

  private:
    size_t currentEnemy_;
    std::vector<float> spawndelay;
    std::vector<EnemySpawnFlags> spawnflags;
    std::vector<ObjectSpawnInfo> spawninfo;
    std::vector<int> spawnposition;
    size_t size;
  };
  
}