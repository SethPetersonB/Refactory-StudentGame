// Primary Author : Dylan Weber
// 
// Co-authors:
//    Philip Nygard (LoseInit)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include "GSM.h"

namespace Engine
{
  namespace StageInit
  {
    void LoseInit(Stage * lose);
    void QuitInit(Stage * quit);
    void PauseInit(Stage * pause);
    void HelpInit(Stage * help);
    void SplashInit(Stage * splash);
    void GameStageInit(Stage * game);
    void WinInit(Stage * win);
    void MenuInit(Stage * menu);
    void OptionsInit(Stage * options);
    void CreditsInit(Stage * credits);
    void MenuConfirmationInit(Stage * stage);
    void MenuQuitConfirmationInit(Stage * stage);
    void RestartInit(Stage* stage);
  }
}