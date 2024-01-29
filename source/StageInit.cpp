// Primary Author : Dylan Weber
// 
// Co-authors:
//    Philip Nygard (LoseInit)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/StageInit.h"
#include "../include/Messages.h"
#include "../include/Input.h"
#include "../include/MenuButtons.h"
#include "../include/Waves.h"
#include "../include/Controller.h"
#include "../include/audio_startup.h"
#include "../include/WaveLoader.h"

namespace Engine
{

  namespace StageInit
  {
    /****************************************************************************/
    /*!
    \brief
    Messaging event that is sent on loss to display lose screen

    \param stage
    The lose stage to set running

    \param payload
    The data of message
    */
    /****************************************************************************/
    static void OnLoseStage(Stage * stage, const Packet & payload)
    {
      stage->setStageRunning(payload.getData<bool>());
    }

    static void OnWinStage(Stage * stage, const Packet & payload)
    {
      stage->setStageRunning(payload.getData<bool>());
    }
    /****************************************************************************/
    /*!
    \briefOnLoseGame
    Messaging event that is sent on loss to pause main stage

    \param stage
    The main stage to pause

    \param payload
    The data of message
    */
    /****************************************************************************/
    static void OnLoseGame(Stage * stage, const Packet & payload)
    {
      stage->setStageRunning(!payload.getData<bool>());
      //Sound cues
      int channelFound = -1;
      /* AVOID USING PAUSE FROM NOW ON.
      channelFound = GetAudioEngine()->FindSoundChannel("menumelody_repeat.wav");
      GetAudioEngine()->SetChannelPause(channelFound, true);
      */
      //Determine whether player has lost or won.
      GameInstance* pc = stage->getMessenger().Request<GameInstance *>("PlayerController");
      Controller* control = dynamic_cast<Controller*>(pc->getComponent("Controller"));
      if ( control->isLost() )
      {
        channelFound = GetAudioEngine()->FindSoundChannel("losetheme2.wav");
        if ( channelFound == -1)
        {
          GetAudioEngine()->PlaySounds("losetheme2.wav", Vector3(), 0);
          channelFound = GetAudioEngine()->FindSoundChannel("menumelody_repeat.wav");
          if ( channelFound != -1 )
          {
            GetAudioEngine()->StopChannel(channelFound);
          }
        }
      }
      if ( control->getHP() > 0 )
      {
        control->setWon( true );
      }
      if ( control->isWon() )
      {
        channelFound = GetAudioEngine()->FindSoundChannel("level_complete.wav");
        if (channelFound == -1)
        {
          GetAudioEngine()->PlaySounds("level_complete.wav", Vector3(), 5);
          channelFound = GetAudioEngine()->FindSoundChannel("menumelody_repeat.wav");
          if ( channelFound != -1 )
          {
            GetAudioEngine()->StopChannel(channelFound);
          }
        }
      }
      //END OF SOUND CUES
    }

    /****************************************************************************/
    /*!
    \brief
    Messaging event that is sent on when reset button is pressed

    \param button
    The reset button

    \param gameStage
    The game stage to reset

    \param payload
    The data of the message
    */
    /****************************************************************************/
    static void OnResetButtonClicked(GameInstance * button, Stage * gameStage, const Packet & payload)
    {
      button->getStage()->setStageRunning(false);

      gameStage->setStageReset(true);

      //Reset win/lose conditions.
      GameInstance* pc = gameStage->getMessenger().Request<GameInstance *>("PlayerController");
      Controller* control = dynamic_cast<Controller*>(pc->getComponent("Controller"));
      control->setWon( false );
      control->setLost( false );

      int nChannelId = GetAudioEngine()->FindSoundChannel("menumelody_repeat.wav");
      if ( nChannelId != -1 )
      {
        //GetAudioEngine()->StopChannel(nChannelId);
      }
      else
      {
        GetAudioEngine()->PlaySounds("menumelody_repeat.wav", Vector3(), -15.0f);
      }
    }

    static void OnFullScreenClicked(GameInstance* button, const Packet & payload)
    {
      Display& display = GSM::get().getDisplay();
      display.setFullScreen(!display.isFullScreen()); // toggle fullscreen
    }

    static void OnMuteAudioClicked(GameInstance* button, const Packet & payload)
    {
      Audio_Engine* AE = GetAudioEngine();
      AE->SetAllChannelsMute(!getVolumeMute());
      //setVolumeMute(!getVolumeMute()); // toggle muted audio
    }

    static void onMusicClicked(GameInstance* button, const Packet& payload) //TODO: make this work
    {
      /*
      Audio_Engine* AE = GetAudioEngine();
      static bool mute = false;
      int nChannelId = AE->FindSoundChannel("menumelody_repeat.wav");
      mute = !mute;
      AE->SetChannelMute(nChannelId, mute);
      */
      setMelodyMute(!getMelodyMute());
    }

    static void onQuitConfirm(GameInstance* button, const Packet& payload)
    {
      Stage& quit = Stage::GetStage("QuitStage");
      quit.setStageRunning(false);
      Stage* stage = Stage::lastRunning_;
      if (!stage) stage = &Stage::GetStage("TestMenu1"); // failsafe
      stage->setStageRunning(true);
    }

    static void onQuit(GameInstance* button, const Packet& payload)
    {
      GSM::get().getMessenger().Post("QUIT_CONFIRM", true);
    }

    /****************************************************************************/
    /*!
    \brief
    Initializes data for the lose stage

    \param lose
    The lose stage

    */
    /****************************************************************************/
    void LoseInit(Stage * lose)
    {
      Stage & gameStage = Stage::GetStage("TestStage1");
      // add objects to lose screen

      GameInstance & menu = lose->getFirstInstanceByName("HorizontalMenu");

      GameInstance & resetButton = lose->getFirstInstanceByName("ResetButton");
      using namespace std::placeholders;

      // subscribe to messages
      SUBSCRIBER_ACTION loseFunc = std::bind(OnLoseStage, lose, std::placeholders::_1);
      SUBSCRIBER_ACTION reset = std::bind(OnResetButtonClicked, &resetButton, &gameStage, std::placeholders::_1);

      lose->getMessenger().Subscribe(GSM::get().getMessenger(), "Lose", loseFunc);
      resetButton.getMessenger().Subscribe(resetButton.getMessenger(), "Clicked", reset);

      menu.PostMessage("AddButton", resetButton.getId());
      lose->setStageRunning(false);

      //GSM::get().getMessenger().Post("Lose", true);
    }

    void WinInit(Stage * win)
    {
      Stage & gameStage = Stage::GetStage("TestStage1");
      // add objects to lose screen

      GameInstance & menu = win->getFirstInstanceByName("HorizontalMenu");

      GameInstance & resetButton = win->getFirstInstanceByName("ResetButton");
      using namespace std::placeholders;

      // subscribe to messages
      SUBSCRIBER_ACTION loseFunc = std::bind(OnWinStage, win, std::placeholders::_1);
      SUBSCRIBER_ACTION reset = std::bind(OnResetButtonClicked, &resetButton, &gameStage, std::placeholders::_1);

      win->getMessenger().Subscribe(GSM::get().getMessenger(), "Win", loseFunc);
      resetButton.getMessenger().Subscribe(resetButton.getMessenger(), "Clicked", reset);

      menu.PostMessage("AddButton", resetButton.getId());
      win->setStageRunning(false);

      //GSM::get().getMessenger().Post("Win", true);
    }

    /****************************************************************************/
    /*!
    \brief
    Initializes data for the quit stage

    \param quitStage
    The quit stage

    */
    /****************************************************************************/
    void  QuitInit(Stage * quitStage)
    {
      GameInstance & menu = quitStage->getFirstInstanceByName("HorizontalMenu");

      Stage & pauseManu = Stage::GetStage("TestMenu1");
      Stage & gameStage = Stage::GetStage("TestStage1");

      // Set up button connections
      // yes button on quit screen
      GameInstance& yes = quitStage->getFirstInstanceByName("Yesbutton");
      //SUBSCRIBER_ACTION y = std::bind(MenuButtons::ButtonClickedEvent, &yes, quitStage, quitStage, std::placeholders::_1);
      //SUBSCRIBER_ACTION invis_y = std::bind(MenuButtons::InvisibleEvent, &yes, Message<bool>(gameStage.isStageRunning()));
      yes.getMessenger().Subscribe(yes.getMessenger(), "Clicked", 
        [&yes](const Packet &) 
      {
        GSM::get().getMessenger().Post<std::string>("GSM_END", "Quit Button pressed"); 
      });
      //yes.getMessenger().Subscribe(gameStage.getMessenger(), "STAGE_PAUSED", invis_y);

      // no button on quit screen
      GameInstance& no = quitStage->getFirstInstanceByName("Nobutton");
      SUBSCRIBER_ACTION n = std::bind(&onQuitConfirm, &no, std::placeholders::_1);
     // SUBSCRIBER_ACTION invis_n = std::bind(MenuButtons::InvisibleEvent, &no, Message<bool>(gameStage.isStageRunning()));
      no.getMessenger().Subscribe(no.getMessenger(), "Clicked", n);
    //  no.getMessenger().Subscribe(gameStage.getMessenger(), "STAGE_PAUSED", invis_n);

      menu.PostMessage("AddButton", yes.getId());
      menu.PostMessage("AddButton", no.getId());

      quitStage->setStageRunning(false);

    }

    /****************************************************************************/
    /*!
    \brief
    Initializes data for the pause stage

    \param pause
    The pause stage

    */
    /****************************************************************************/
    void PauseInit(Stage * pause)
    {
      Stage& gameStage = Stage::GetStage("TestStage1");
      Stage & quitStage = Stage::GetStage("QuitStage");
      Stage & howToPlay = Stage::GetStage("HowToPlay");
      Stage& mainMenu = Stage::GetStage("MainMenu");
      Stage& confirm = Stage::GetStage("MenuConfirmation");
      Stage& options = Stage::GetStage("Options");
      Stage& credits = Stage::GetStage("Credits");
      Stage& restart = Stage::GetStage("ResetConfirmation");

      GameInstance & menu = pause->getFirstInstanceByName("VerticalMenu");
      // resume game button

      GameInstance& resumeButton = pause->getFirstInstanceByName("Resumegame");
      SUBSCRIBER_ACTION resume = std::bind(MenuButtons::ButtonClickedEvent, &resumeButton, pause, &gameStage, std::placeholders::_1);
      resumeButton.getMessenger().Subscribe(resumeButton.getMessenger(), "Clicked", resume);
      resumeButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.4, 0.2));
      resumeButton.PostMessage("PositionSet", glm::vec3(0, 0.85, 1));
     
      // how to play button
      GameInstance& helpButton = pause->getFirstInstanceByName("Howtoplaybutton");
      SUBSCRIBER_ACTION help = std::bind(MenuButtons::ButtonClickedEvent, &helpButton, pause, &howToPlay, std::placeholders::_1);
      helpButton.getMessenger().Subscribe(helpButton.getMessenger(), "Clicked", help);
      helpButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.4, 0.2));
      helpButton.PostMessage("PositionSet", glm::vec3(0, 0.6, 1));

      // reset button
      GameInstance& resetButton = pause->getFirstInstanceByName("ResetButton");
      SUBSCRIBER_ACTION reset = std::bind(MenuButtons::ButtonClickedEvent, &resetButton, pause, &restart, std::placeholders::_1);
      resetButton.getMessenger().Subscribe(resetButton.getMessenger(), "Clicked", reset);
      resetButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.4, 0.2));
      resetButton.PostMessage("PositionSet", glm::vec3(0, 0.35, 1));

      // return button
      GameInstance& returnButton = pause->getFirstInstanceByName("ReturnButton");
      SUBSCRIBER_ACTION returnfunc = std::bind(MenuButtons::ButtonClickedEvent, &returnButton, pause, &confirm, std::placeholders::_1);
      returnButton.getMessenger().Subscribe(returnButton.getMessenger(), "Clicked", returnfunc);
      returnButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.4, 0.2));
      returnButton.PostMessage("PositionSet", glm::vec3(0, 0.10, 1));

      // options button
      GameInstance& optionButton = pause->getFirstInstanceByName("OptionsButton");
      SUBSCRIBER_ACTION option = std::bind(MenuButtons::ButtonClickedEvent, &optionButton, pause, &options, std::placeholders::_1);
      optionButton.getMessenger().Subscribe(optionButton.getMessenger(), "Clicked", option);
      optionButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.4, 0.2));
      optionButton.PostMessage("PositionSet", glm::vec3(0, -0.15, 1));


      // credits button
      GameInstance& creditsButton = pause->getFirstInstanceByName("CreditsButton");
      SUBSCRIBER_ACTION c =
        [pause, &credits, &creditsButton](const Packet & data)
      {
        credits.setStageReset(true);
        MenuButtons::ButtonClickedEvent(&creditsButton, pause, &credits, data);
      };

      creditsButton.getMessenger().Subscribe(creditsButton.getMessenger(), "Clicked", c);
      creditsButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.4, 0.2));
      creditsButton.PostMessage("PositionSet", glm::vec3(0, -0.40, 1));

      // quit game button
      GameInstance& quitButton = pause->getFirstInstanceByName("Quitgame");
      SUBSCRIBER_ACTION quit = std::bind(&onQuit, &quitButton, std::placeholders::_1);
      /*  [pause, &quitStage](const Packet& data)
      {
        pause->setStageRunning(false);
        quitStage.setStageRunning(true);
      };*/
      quitButton.getMessenger().Subscribe(quitButton.getMessenger(), "Clicked", quit);
      quitButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.4, 0.2));
      quitButton.PostMessage("PositionSet", glm::vec3(0, -0.65, 1));

      menu.PostMessage("AddButton", resumeButton.getId());
      menu.PostMessage("AddButton", helpButton.getId());
      menu.PostMessage("AddButton", resetButton.getId());
      menu.PostMessage("AddButton", returnButton.getId());
      menu.PostMessage("AddButton", optionButton.getId());
      menu.PostMessage("AddButton", creditsButton.getId());
      menu.PostMessage("AddButton", quitButton.getId());
     

      pause->setStageRunning(false);
    }

    /****************************************************************************/
    /*!
    \brief
    Initializes data for the help stage

    \param help
    The help stage

    */
    /****************************************************************************/
    void HelpInit(Stage * help)
    {
      help->setStageRunning(false);

    }

    /****************************************************************************/
    /*!
    \brief
    Initializes data for the splash stage

    \param splash
    The splash stage

    */
    /****************************************************************************/
    void SplashInit(Stage * splash)
    {

      GameInstance& logo = splash->getFirstInstanceByName("SplashScreen");
      SUBSCRIBER_ACTION logo_invis = std::bind(MenuButtons::InvisibleEvent, &logo, Message<bool>(splash->isStageRunning()));
      logo.getMessenger().Subscribe(splash->getMessenger(), "STAGE_PAUSED", logo_invis);

    }

    /****************************************************************************/
    /*!
    \brief
    Initializes data for the game stage

    \param game
    The game stage

    */
    /****************************************************************************/
    void GameStageInit(Stage* game)
    {

      GameInstance & pc = game->getFirstInstanceByName("PlayerController");
      game->getFirstInstanceByName("Background").getMessenger().Unsubscribe(game->getMessenger(), "STAGE_PAUSED");
      GameInstance & HP = game->getFirstInstanceByName("HealthUI");
      GameInstance & RES = game->getFirstInstanceByName("ResourceUI");
      GameInstance & PRO = game->getFirstInstanceByName("ProgressUI");
      GameInstance & waveText = game->getFirstInstanceByName("WaveText");
      GameInstance & waveCount = game->getFirstInstanceByName("WaveCount");
      GameInstance & wavCont = game->getFirstInstanceByName("WaveController");
      GameInstance & beginTxt = game->getFirstInstanceByName("SpaceBegin");

      WaveLoader load{ "Waves/L1Waves.json" };

      for (size_t waveNum = 0; waveNum < load.size(); waveNum++)
      {
        Wave & wave = load.getWave(waveNum);

        wavCont.PostMessage("AddWave", &wave);
      }

      pc.PostMessage("SetNumWaves", load.size());

      using namespace std::placeholders;

      SUBSCRIBER_ACTION loseFunc = std::bind(OnLoseGame, game, std::placeholders::_1);
      game->getMessenger().Subscribe(GSM::get().getMessenger(), "Lose", loseFunc);
      game->getMessenger().Subscribe(GSM::get().getMessenger(), "Win", loseFunc);

      pc.PostMessage("SetHPCounter", HP.getId());
      pc.PostMessage("SetResourceCounter", RES.getId());
      pc.PostMessage("SetProgressCounter", PRO.getId());
      pc.PostMessage("SetWaveText", waveText.getId());
      pc.PostMessage("SetWaveCounter", waveCount.getId());
      pc.PostMessage("SetWaveBeginText", beginTxt.getId());

     /* Grid& grid_ = GSM::get().getGrid();
      Grid grid;
      grid_ = grid;
      int gridX = 10;
      int gridY = 10;
      grid_.SetGridDimensions(gridX, gridY);
      grid_.SetMaxBuildHeight(100);
      DrawTiles(grid_);

      game->setStageRunning(true);*/

      Grid grid(game, 11, 10);
      game->SetGrid(grid);
      game->GetGrid().DrawGrid();

      game->GetGrid().LoadStructureData("Objects/Structures.json");

      // Load lua sandbox environment
      game->setStageRunning(true);
    }

    void MenuInit(Stage * menu)
    {
      Stage& gameStage = Stage::GetStage("TestStage1");


      GameInstance & v = menu->getFirstInstanceByName("HorizontalMenu");

      // resume game button
      GameInstance& resumeButton = menu->getFirstInstanceByName("StartButton");

      SUBSCRIBER_ACTION resume = 
        [& resumeButton, & gameStage, menu](const Packet & data) 
      {
        gameStage.setStageReset(true);

        MenuButtons::ButtonClickedEvent(&resumeButton, menu, &gameStage, data);
      };

      resumeButton.getMessenger().Subscribe(resumeButton.getMessenger(), "Clicked", resume);
      resumeButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.75, 0.375));
      resumeButton.PostMessage("PositionSet", glm::vec3(-0.3, -0.3, 1));

      // quit game button
      GameInstance& quitButton = menu->getFirstInstanceByName("Quitgame");
      SUBSCRIBER_ACTION quit = std::bind(&onQuit, &quitButton, std::placeholders::_1);
      quitButton.getMessenger().Subscribe(quitButton.getMessenger(), "Clicked", quit);
      quitButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.75, 0.375));
      quitButton.PostMessage("PositionSet", glm::vec3(0.3, -0.3, 1));

    
      v.PostMessage("AddButton", quitButton.getId());
      v.PostMessage("AddButton", resumeButton.getId());
      
    }

    void OptionsInit(Stage * options)
    {
      Stage& menu = Stage::GetStage("TestMenu1");
     
      GameInstance & v = options->getFirstInstanceByName("VerticalMenu");
     
      // return button
      GameInstance& quitButton = options->getFirstInstanceByName("ReturnButton");
      SUBSCRIBER_ACTION quit = std::bind(MenuButtons::ButtonClickedEvent, &quitButton, options, &menu, std::placeholders::_1);
      quitButton.getMessenger().Subscribe(quitButton.getMessenger(), "Clicked", quit);
      quitButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.6, 0.3));
      quitButton.PostMessage("PositionSet", glm::vec3(0, 0.5, 1));
      
      // fullscreen button
      GameInstance& fullscreenButton = options->getFirstInstanceByName("FullScreenButton");
      SUBSCRIBER_ACTION fullscreen = std::bind(&OnFullScreenClicked, &quitButton, std::placeholders::_1); // toggle fullscreen
      fullscreenButton.getMessenger().Subscribe(fullscreenButton.getMessenger(), "Clicked", fullscreen);
      fullscreenButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.6, 0.3));
      fullscreenButton.PostMessage("PositionSet", glm::vec3(0, 0.1, 1));

      // toggle all audio button
      GameInstance& allaudioButton = options->getFirstInstanceByName("AudioButton");
      SUBSCRIBER_ACTION mute_audio = std::bind(&OnMuteAudioClicked, &quitButton, std::placeholders::_1); // toggle mute all audio
      allaudioButton.getMessenger().Subscribe(allaudioButton.getMessenger(), "Clicked", mute_audio);
      allaudioButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.6, 0.3));
      allaudioButton.PostMessage("PositionSet", glm::vec3(0, -0.3, 1));

      // toggle music button
      GameInstance& musicButton = options->getFirstInstanceByName("MusicButton");
      SUBSCRIBER_ACTION music = std::bind(&onMusicClicked, &musicButton, std::placeholders::_1);
      musicButton.getMessenger().Subscribe(musicButton.getMessenger(), "Clicked", music);
      musicButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.6, 0.3));
      musicButton.PostMessage("PositionSet", glm::vec3(0, -0.7, 1));

      v.PostMessage("AddButton", quitButton.getId());
      v.PostMessage("AddButton", fullscreenButton.getId());
      v.PostMessage("AddButton", allaudioButton.getId());
      v.PostMessage("AddButton", musicButton.getId());
      options->setStageRunning(false);
    }

    void CreditsInit(Stage * credits)
    {
      Stage& menu = Stage::GetStage("MainMenu");
      Stage& pause = Stage::GetStage("TestMenu1");
      GameInstance & v = credits->getFirstInstanceByName("HorizontalMenu");

      // quit game button
      GameInstance& quitButton = credits->getFirstInstanceByName("ReturnButton");
      SUBSCRIBER_ACTION quit = std::bind(MenuButtons::ButtonClickedEvent, &quitButton, credits, &pause, std::placeholders::_1);
      quitButton.getMessenger().Subscribe(quitButton.getMessenger(), "Clicked", quit);
      quitButton.RequestData<DrawToken>("Graphic").setScale(glm::vec2(0.5, 0.25));
      quitButton.PostMessage("PositionSet", glm::vec3(0.75, 0.85, 1));

      v.PostMessage("AddButton", quitButton.getId());
      credits->setStageRunning(false);
    }

    void MenuConfirmationInit(Stage * stage)
    {
      GameInstance & menu = stage->getFirstInstanceByName("HorizontalMenu");

       Stage & pauseManu = Stage::GetStage("TestMenu1");
      //Stage & gameStage = Stage::GetStage("TestStage1");
      Stage& mainMenu = Stage::GetStage("MainMenu");

      // Set up button connections
      // yes button on quit screen
      GameInstance& yes = stage->getFirstInstanceByName("Yesbutton");
      SUBSCRIBER_ACTION y = std::bind(MenuButtons::ButtonClickedEvent, &yes, stage, &mainMenu, std::placeholders::_1);
      yes.getMessenger().Subscribe(yes.getMessenger(), "Clicked", y);

      // no button on quit screen
      GameInstance& no = stage->getFirstInstanceByName("Nobutton");
      SUBSCRIBER_ACTION n = std::bind(MenuButtons::ButtonClickedEvent, &no, stage, &pauseManu, std::placeholders::_1);
      no.getMessenger().Subscribe(no.getMessenger(), "Clicked", n);

      menu.PostMessage("AddButton", yes.getId());
      menu.PostMessage("AddButton", no.getId());

      stage->setStageRunning(false);
    }

    void RestartInit(Stage* stage)
    {
      GameInstance& menu = stage->addGameInstance("HorizontalMenu");
      Stage& pauseMenu = Stage::GetStage("TestMenu1");
      Stage& gameStage = Stage::GetStage("TestStage1");

      // Set up button connections
      // yes button on quit screen
      GameInstance& yes = stage->getFirstInstanceByName("Yesbutton");
      SUBSCRIBER_ACTION reset = std::bind(OnResetButtonClicked, &yes, &gameStage, std::placeholders::_1);
      yes.getMessenger().Subscribe(yes.getMessenger(), "Clicked", reset);

      // no button on quit screen
      GameInstance& no = stage->getFirstInstanceByName("Nobutton");
      SUBSCRIBER_ACTION n = std::bind(MenuButtons::ButtonClickedEvent, &no, stage, &pauseMenu, std::placeholders::_1);
      no.getMessenger().Subscribe(no.getMessenger(), "Clicked", n);

      menu.PostMessage("AddButton", yes.getId());
      menu.PostMessage("AddButton", no.getId());

      stage->setStageRunning(false);
    }
  }
}
