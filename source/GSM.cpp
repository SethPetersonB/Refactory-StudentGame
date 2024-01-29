// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <ctime>
#include <thread>
#include "../include/GSM.h"

#include "../include/DrawUtils.h"
#include "../include/Input.h"
#include "../include/camera.h"
#include "../include/audio_test.h"
//#include "../include/Texture.h"
#include "../include/imgui_impl.h"
#include "../include/audio_startup.h"
#include "../include/audio_test.h"
#include "../include/Logger.h"
//#include "../include/UIFrame.h"
#include "../include/EnemyLogic.h"
#include "../include/Particles.h"
#include "../include/StageInit.h"
//#include "../include/Controller.h"
//#include "../include/MenuButtons.h"
//#include "../include/Waves.h"
#include "../include/Levels.h"
#include <functional>
#include "RMesh.h"


using namespace Logger;


static Timer soundTimer;

namespace Engine
{
  GSM & GetGSM()
  {
    return GSM::get();
  }

  //Display GSM_Display;
  //Shader Color_Shader;
  //Text TestText;

  // Flag used to tell if the game is shutting down
  //static bool GSM_ENDING = false;
  //static Messenger GSM_MESSENGER;

  /*Messenger & GSM_Messenger()
  {
    return GSM_MESSENGER;
  }*/

  /*static void OnGSMEnd(const Packet & payload)
  {
    const std::string & data = dynamic_cast<const Message<std::string> *>(&payload)->data;
    GSM_ENDING = true;
  }*/
  
  void GSM::OnEnd(const Packet & payload)
  {
    GSM & gsm = GSM::get();

    Log<Info>("GSM Ending. Exit message: %s", static_cast<const Message<std::string> &>(payload).data.c_str());

    gsm.ending_ = true;
  }

  
  /****************************************************************************/
  /*!
    \brief
      Initialization for the GSM. Use to initialize all systems necessary for
      the game to run
  */
  /****************************************************************************/
  void GSM::Init()
  {
    using namespace DrawUtils;

    StartGameAudioSystem();
    Audio_Engine* AEngine = GetAudioEngine();


    //InitializeComponentHandlers();
    disp_.Initialize(1280, 720, "Refactory");
    renderer_ = std::make_unique<DrawSystem>(disp_.GetWindow(), 0, 0, 1280, 720);

    disp_.setEventHandler(&InputSystem::Update);

    R_InitLayers(*renderer_);
    R_InitShaders(*renderer_);
    R_LoadMeshes(*renderer_);

    cam_.Init();

    ImGui_ImplSdlGL3_Init(disp_.GetWindow());

    mess_.Subscribe(mess_, "GSM_END", GSM::OnEnd);

    GenerateParsedObjects("Objects/Objects.json");

    // Load textures


    //Stage & Snrl = Stage::New("NetResultLogo");
    struct LoadLock
    {
    public:
      void done(DrawSystem::RES_MAP<std::string, Texture>::iterator first)
      {
        std::lock_guard<std::mutex> lock{ locker_ };
        currLoaded_ = first;
        done_ = true;
      }

      bool isLoading() const
      {
        std::lock_guard<std::mutex> lock{ locker_ };
        return !done_;
      }

      DrawSystem::RES_MAP<std::string, Texture>::iterator & curr()
      {
        if (isLoading())
          throw std::logic_error("Attempting to upload textures before done loading");

        return currLoaded_;
      }

    private:
      DrawSystem::RES_MAP<std::string, Texture>::iterator currLoaded_;
      mutable std::mutex locker_;
      bool done_ = false;
    };

    LoadLock lod;
    
    std::vector<IMGINFO> order = DrawUtils::R_GetPreloads(*renderer_);

    size_t current = 0;

    // LOAD-THREADING START
    std::thread texLoader{ 
      [](DrawSystem * sys, LoadLock & lod) 
    {
      R_LoadTextures(sys, "Objects/Textures.json");
      lod.done(sys->firstTexture());
    },
      renderer_.get(), std::ref(lod)};
   
    // Splash Screens and loading
    {
      Timer elapsed;
      Display & disp = getDisplay();
      float dispTime = 7.0f;

      bool lodStart = false;

      bool doneLoading = false;
      
      while (lod.isLoading() || 
        (current < order.size()) || 
        (!doneLoading))
      {
        if (doneLoading == false && !lod.isLoading())
        {
          doneLoading = (renderer_->loadNextTexture(lod.curr()));
        }

        if (!lodStart && doneLoading)
        {
          AEngine->PlaySounds("startup.wav", Vector3(), -15.0f);
          soundTimer.Reset();
          //++current;
          lodStart = true;
          elapsed.Reset();
        }

        InputSystem::Clean();
        getDisplay().Update();

        if (doneLoading && current < order.size())
        {
          if (InputSystem::KeyPressed(SDL_SCANCODE_SPACE) || InputSystem::KeyPressed(SDL_SCANCODE_RETURN) || InputSystem::KeyPressed(SDL_SCANCODE_ESCAPE)
            || InputSystem::Mouse1Clicked() || InputSystem::Mouse1Clicked())
            break;
          else if (elapsed.ElapsedTime() > order[current].lifetime)
          {

            ++current;

            if (current >= order.size())
              continue;

            order[current - 1].token.setVisible(false);
            order[current].frameTimer.Reset();
            order[current].token.setFrame(0);
            order[current].token.setVisible(true);
            order[current].token.setShade({ 1, 1, 1, 1 });
            elapsed.Reset();

            //if (current >= order.size() - 1)
            //  elapsed.Pause();
          }
        }

        if( order[current].numFrames > 0 && order[current].frameTime <= order[current].frameTimer.ElapsedTime())
        {
          IMGINFO & info = order[current];
          info.currFrame = ++info.currFrame % info.numFrames;
          info.token.setFrame(info.currFrame);
          info.frameTimer.Reset();
        }

        if (doneLoading && current < order.size())
        {
          float et = elapsed.ElapsedTime();
          float alpha = 1;
          IMGINFO & info = order[current];

          if (et - (info.lifetime - info.fadeOutTime) >= 0)
            alpha = 1 - (et - (info.lifetime - info.fadeOutTime)) / info.fadeOutTime;
          else if (et < info.fadeInTime && current != 0)
            alpha = et / info.fadeInTime;

          info.token.setShade({ 1, 1, 1, alpha });
        }

        //renderer_->makeCurrent();
        //renderer_->resize(0, 0, disp.GetWidth(), disp.GetHeight());

        //AEngine->Update();
        //  2.85 seconds -- done fading out
        //  2 seconds -- start fading out

        //stg.update();
        //renderer.makeCurrent();
        renderer_->update();
        renderer_->swap(0, 0, 0);
      }
    }

    //renderer_.reset(old);
    renderer_->swap(0, 0, 0);

    texLoader.join();

    // LOAD-THREADING END

    Log<Info>("%d", GenerateLevels("Objects/Levels.json"));

    
    Stage & testStage = Stage::GetStage("TestStage1");
    // Stage initializer sets game stage to running, but the first time we don't want that
    testStage.setStageRunning(false);
    
    if(testStage.isStageToggling())
      Stage::ToggleRunning(testStage);

    loading_ = false; // done loading
  }

  
  /****************************************************************************/
  /*!
    \brief
      Main loop for the GSM. Runs until exit conditions are met and updates
      all active stages
  */
  /****************************************************************************/

  void GSM::Loop()
  {
    // Main game loop
	  time_t start = time(NULL);
    // Start sound
	  bool showMenu = true; // set to true to display main menu
    Timer time;
	  bool showSplash = false; // set to true to display splash screen
	  Stage& testStage = Stage::GetStage("TestStage1");
    Stage& mainMenu = Stage::GetStage("MainMenu");
    mainMenu.setStageRunning(true);

	  //GameInstance& wall = testStage.addGameInstance("Wall");
    //wall.PostMessage("CreateParticles", Message<bool>(true)); // data isn't actually used

    bool soundflag = true;

    while (!ending_ && !disp_.IsClosed())
    {
      InputSystem::Clean();
      disp_.Update();

      if(soundflag && soundTimer.ElapsedTime() >= /*sound length*/ 12)
      {
        /* Do stuff */
        Audio_Engine* AEngine = GetAudioEngine();
        AEngine->LoadSound("menumelody_repeat.wav", false, true );
        AEngine->PlaySounds("menumelody_repeat.wav", Vector3(), -15.0f);
        int nChannelId = AEngine->FindSoundChannel("menumelody_repeat.wav");
        AEngine->SetChannelPriority(nChannelId, 0);
        soundflag = false;
      }
  

      // Pre-stage logic 

      Audio_Engine* AEngine = GetAudioEngine();
      AEngine->Update();
      cam_.Update();

      UpdateImGui(disp_.GetWindow());

      /*

      Update input, display, etc
      (non-stage systems)

      */
      // Stage loop
      auto i = Stage::StageList.begin();
      while (i != Stage::StageList.end())
      {
        auto stage = i->second.begin();
        while (stage != i->second.end())
        {
          (*stage)->update();
          Stage::CleanStage(**stage);

          ///* SETH: Testing out enemy spawning! */
          ///* Oh god, what have I done!? */
          //static Timer spawndelay;

          //static bool startspawn = false;
          //static int col = 1;
          //if (spawndelay.ElapsedTime() >= 12.0)
          //{
          //  startspawn = true;
          //}

          //WaveController wc( &WaveVector, *stage, &grid_, 0 );

          //if ( WaveVector[0].GetSize() > wc.GetIndex() )
          //{
          //  if (startspawn && spawndelay.ElapsedTime() >= WaveVector[0].GetDelay(wc.GetIndex()))
          //  {
          //    wc.Start();
          //    wc.SetIndex(wc.GetIndex() + 1);
          //    spawndelay.Reset();
          //  }
          //}


#if 0
          if (startspawn && spawndelay.ElapsedTime() >= 2.0)
          {
            if ((*stage)->getStageName() == "TestStage1")
            {
              ObjectSpawnInfo spawninfobj;
              EnemySpawnFlags spawninfoenem;
              glm::vec2 pos;
              ReturnEnemyArchetype(&spawninfobj, &spawninfoenem, "Fast");
              pos = ReturnEnemySpawnLocation(&grid_, col);

              SpawnEnemy((**stage), spawninfobj, spawninfoenem, pos);
              spawndelay.Reset();
              ++col;
              if ( col > grid_.GetGridHeight() )
              {
                col = 1;
              }
            }
          }
#endif

         
          if (!getVolumeMute()) // if volume not muted
          {
            // Get quieter when entering pause.
            if (!testStage.isStageRunning())
            {
              if (!isVisitedPause())
              {
                int nChannelId = GetAudioEngine()->FindSoundChannel("menumelody_repeat.wav");
                float fVolumedB = GetAudioEngine()->GetChannelVolume(nChannelId);
                GetAudioEngine()->SetChannelVolume(nChannelId, fVolumedB - 12.0f);
                setVisitedPause(true);
              }
            }

            // Get louder when leaving pause.
            else
            {
              if (isVisitedPause())
              {
                int nChannelId = GetAudioEngine()->FindSoundChannel("menumelody_repeat.wav");
                float fVolumedB = GetAudioEngine()->GetChannelVolume(nChannelId);
                GetAudioEngine()->SetChannelVolume(nChannelId, fVolumedB + 12.0f);
                setVisitedPause(false);
              }
            }
          }
       

        // If the stage is ending
        if ((*stage)->isStageEnding())
        {
          Stage::DestroyStage((*stage)->getStageName(), i->first);
          i->second.erase(stage++);
        }
        else
        {
          if ((*stage)->isStageResetting())
          {
            Stage::ResetStage(*stage);
          }

          if ((*stage)->isStageToggling())
          {
            Stage::ToggleRunning(**stage);
          }
            ++stage;
          }
        }

        if (i->second.size() <= 0)
        {
          Stage::StageList.erase(i++);
        }
        else
        {
          ++i;
        }
      }


      // Post-stage logic
      renderer_->update();
      ImGui::Render();
      renderer_->swap(0, 0, 0);

      //InputSystem::Update(); // I'm the one who does input around here, BUCKO! // Not today, BUSTER!!
      //TestText.RenderText();
      // Post-stage logic
    }
  }

  /****************************************************************************/
  /*!
    \brief
      Unloads the GSM and frees all systems from memory that need to be freed
  */
  /****************************************************************************/
  void GSM::Unload()
  {
    Stage::FlushStageList();
    disp_.Destroy();
    FlushParsedObjects();
    Audio_Engine* AEngine = GetAudioEngine();
    AEngine->Shutdown();
  }

  Messenger & GSM::getMessenger()
  {
    return mess_;
  }

  Display & GSM::getDisplay()
  {
    return disp_;
  }

  Camera & GSM::getCamera()
  {
    return cam_;
  }

  DrawSystem & GSM::getRenderer()
  {
    return *renderer_;
  }

 /// Grid & GSM::getGrid()
  //{
	//  return grid_;
 // }

  GSM & GSM::get()
  {
    static GSM gsm;

    return gsm;
  }

  GSM::GSM()
  {}
}
