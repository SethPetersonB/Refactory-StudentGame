// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#include "../include/Waves.h"
#include "../include/Stage.h"
#include "../include/GSM.h"

namespace Engine
{
  WaveController::WaveController(GameInstance * owner) : Component(owner, "WaveController"),
    currentWave_(0), running_(false)
  {
    waveTimer_.Pause();
  }

  //WaveController::WaveController( std::vector<Wave>* WaveVector, Stage* stage, Grid* grid, int index ) :
  //   wv_(WaveVector), /*stg_(stage), grd_(grid), */ i_( index )
  //{
  //}

  WaveController::~WaveController()
  {
  }

  /* Starts spawning enemies. */
  void WaveController::StartWave(size_t wave)
  {
    currentWave_ = wave;
    running_ = true;
  }

  size_t WaveController::CurrentWave() const
  {
    return currentWave_;
  }

  void WaveController::SetIndex(int index)
  {
    i_ = index;
  }

  int WaveController::GetIndex() const
  {
    return i_;
  }

  void WaveController::SetWave(size_t wave)
  {
    if (wave >= NumWaves())
    {
      currentWave_ = 0;
      running_ = false;
     GSM::get().getMessenger().Post("Win", true);
    }
    else
      currentWave_ = wave;

    // make it so you have to press space to begin next wave
    getParent().getStage()->getMessenger().Post("CurrentWave", currentWave_);
    getParent().getStage()->getMessenger().Post("SetWavesRunning", false);
    getParent().getStage()->getMessenger().Post("SetSpaceVisible", true);
    SetWaveRunning(false);
  }

  void WaveController::NextWave()
  {
    SetWave(currentWave_ + 1);
  }

  bool WaveController::IsWaveOver() const
  {
    return wv_[currentWave_].IsWaveOver();
  }
  void WaveController::UpdateWave()
  {
    if (running_)
    {
      Wave & wave = wv_[currentWave_];

      if (wave.IsWaveOver() && IsWaveDead(currentWave_))
      {
        // Only advance to next wave is previous is dead
        NextWave();
        waveTimer_.Reset();
      }
      else if (!wave.IsWaveOver())
      {
        if (wave.GetDelay() <= waveTimer_.ElapsedTime())
        {
          unsigned long enemyID = wv_[currentWave_].SpawnNext(getParent().getStage());

          enemies_.push_back(std::make_pair(enemyID, currentWave_));
          waveTimer_.Reset();
        }
      }
    }
  }

  void WaveController::SetWaveRunning(bool running)
  {
    if (running && !running_)
      waveTimer_.UnPause();
    else if (!running && running_)
      waveTimer_.Pause();

    running_ = running;
  }
 

  void WaveController::AddWave(const Wave & wave)
  {
    wv_.push_back(wave);
  }

  bool WaveController::IsWaveDead(size_t wave) const
  {
    for (auto & enemy : enemies_)
    {
      if (enemy.second == wave)
      {
        try
        {
          // Only succceeds if instance currently exists
          getParent().getStage()->getInstanceFromID(enemy.first);

          return false;
        }
        catch (const std::out_of_range &) {}
      }
    }

    return true;
  }

  void WaveController::KillWave(size_t wave)
  {
    for (auto & instance : enemies_)
    {
      if (instance.second == wave)
      {
        try
        {
          getParent().getStage()->removeGameInstance(instance.first);
        }
        catch (const std::out_of_range &) {}
      }
    }
  }

  void WaveController::KillAll()
  {
    for (size_t i = 0; i < NumWaves(); i++)
      KillWave(i);
  }
  /* Storage container for all the information required to make a wave of enemies. */
  Wave::Wave(std::vector<float> sd, std::vector<EnemySpawnFlags> sf, std::vector<ObjectSpawnInfo> si, std::vector<int> sp) :
    spawndelay(sd), spawnflags(sf), spawninfo(si), spawnposition(sp), size(sf.size()), currentEnemy_(0)
  {
  }

  size_t WaveController::CurrentWaveEnemy() const
  {
    return wv_[currentWave_].getCurrentEnemy();
  }

  size_t WaveController::WaveSize() const
  {
    return wv_[currentWave_].GetSize();
  }

  Wave::~Wave()
  {
  }

  void Wave::RemoveFirst()
  {
    spawndelay.erase( spawndelay.begin() );
    spawnflags.erase( spawnflags.begin() );
    spawninfo.erase( spawninfo.begin() );
    spawnposition.erase( spawnposition.begin() );
    --size;
  }
  float Wave::GetDelay() const
  {
    return spawndelay[currentEnemy_];
  }
  EnemySpawnFlags* Wave::GetFlags()
  {
    return &spawnflags[currentEnemy_];
  }
  ObjectSpawnInfo* Wave::GetInfo()
  {
    return &spawninfo[currentEnemy_];
  }
  int Wave::GetPosition() const
  {
    return spawnposition[currentEnemy_];
  }
  size_t Wave::GetSize() const
  {
    return size;
  }

  void Wave::ResetWave()
  {
    currentEnemy_ = 0;
  }

  void Wave::SetWavePos(size_t pos)
  {
    currentEnemy_ = pos;
  }

  unsigned long Wave::SpawnNext(Stage * stage)
  {
    if (!IsWaveOver())
    {
      glm::vec2 spawnPos = ReturnEnemySpawnLocation(&stage->GetGrid(), GetPosition());
      EnemySpawnFlags & spawnFlags = *GetFlags();
      ObjectSpawnInfo & spawnInfo = *GetInfo();
      ++currentEnemy_;
      return SpawnEnemy(*stage, spawnInfo, spawnFlags, spawnPos);
    }
    else
      return 0;
  }


  WaveControllerHandler::WaveControllerHandler(Stage * owner) : ComponentHandler(owner, "WaveController")
  {}

  void WaveControllerHandler::update()
  {

    for (auto & component : componentList_)
    {
      WaveController * waveCont = static_cast<WaveController *>(component);

      waveCont->UpdateWave();
    }
  }


  // Events
  static void OnWavePaused(WaveController * sub, const Packet & payload)
  {
    sub->SetWaveRunning(!payload.getData<bool>());

    if (sub->IsWaveRunning())
      sub->getParent().getStage()->getMessenger().Post("CurrentWave", sub->CurrentWave());
  }

  static void OnWaveSet(WaveController * sub, const Packet & payload)
  {
    sub->KillWave(sub->CurrentWave());
    sub->SetWave(payload.getData<size_t>());
  }

  static void OnWaveAdd(WaveController * sub, const Packet & payload)
  {
    sub->AddWave(*payload.getData<Wave *>());
  }

  static void OnWaveKill(WaveController * sub, const Packet & payload)
  {
    sub->KillWave(payload.getData<size_t>());
  }

  static void OnKillAll(WaveController * sub, const Packet &)
  {
    sub->KillAll();
  }

  // Requests
  static void RequestWave(const WaveController * sub, Packet & payload)
  {
    payload.setData(sub->CurrentWave());
  }

  static void RequestNumWaves(const WaveController * sub, Packet & payload)
  {
    payload.setData(sub->NumWaves());
  }

  static void RequestCurrentEnemy(const WaveController * sub, Packet & payload)
  {
    payload.setData(sub->CurrentWaveEnemy());
  }

  static void RequestWaveSize(const WaveController * sub, Packet & payload)
  {
    payload.setData(sub->WaveSize());
  }

  void WaveController::OnStagePaused(const Packet & payload)
  {
    bool paused = payload.getData<bool>();

    if (running_)
    {
      if (paused)
        waveTimer_.Pause();
      else
        waveTimer_.UnPause();
    }

  }
  void WaveControllerHandler::ConnectEvents(Component * base_sub)
  {
    WaveController * sub = static_cast<WaveController *>(base_sub);
    Messenger & objMessenger = base_sub->getParent().getMessenger();
    Messenger & stageMessenger = getStage()->getMessenger();

    using namespace std::placeholders;

    SUBSCRIBER_ACTION onPause = std::bind(OnWavePaused, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onStagePause = std::bind(&WaveController::OnStagePaused, sub, std::placeholders::_1);

    SUBSCRIBER_ACTION onWaveSet = std::bind(OnWaveSet, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onWaveAdd = std::bind(OnWaveAdd, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onWaveKilled = std::bind(OnWaveKill, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onAllKilled = std::bind(OnKillAll, sub, std::placeholders::_1);

    REQUEST_ACTION waveReq = std::bind(RequestWave, sub, std::placeholders::_1);
    REQUEST_ACTION waveNums = std::bind(RequestNumWaves, sub, std::placeholders::_1);
    REQUEST_ACTION currEnemy = std::bind(RequestCurrentEnemy, sub, std::placeholders::_1);
    REQUEST_ACTION numEnemies = std::bind(RequestWaveSize, sub, std::placeholders::_1);

    // Events
    objMessenger.Subscribe(stageMessenger, "STAGE_PAUSED", onStagePause);
    objMessenger.Subscribe(stageMessenger, "PauseWave", onPause);
    objMessenger.Subscribe(objMessenger, "PauseWave", onPause);

    objMessenger.Subscribe(stageMessenger, "SetWave", onWaveSet);
    objMessenger.Subscribe(objMessenger, "SetWave", onWaveSet);

    objMessenger.Subscribe(objMessenger, "AddWave", onWaveAdd);

    objMessenger.Subscribe(objMessenger, "KillWave", onWaveKilled);

    objMessenger.Subscribe(stageMessenger, "KillAllWaves", onAllKilled);
    objMessenger.Subscribe(objMessenger, "KillAllWaves", onAllKilled);


    // Requests
    stageMessenger.SetupRequest("CurrentWave", waveReq);
    objMessenger.SetupRequest("CurentWave", waveReq);

    stageMessenger.SetupRequest("TotalWaves", waveNums);
    objMessenger.SetupRequest("TotalWaves", waveNums);

    stageMessenger.SetupRequest("EnemiesDown", currEnemy);
    objMessenger.SetupRequest("EnemiesDown", currEnemy);

    stageMessenger.SetupRequest("TotalEnemies", numEnemies);
    objMessenger.SetupRequest("TotalEnemies", numEnemies);
  }
}