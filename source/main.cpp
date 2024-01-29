// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <windows.h>

#include "../include/GSM.h"
#include "../include/Logger.h"

#ifndef NDEBUG
// Overrides WINAPI macro for debug mode
#define WINAPI

// Allows console to show in debug mode by using default main function instead of WinMain
#define WinMain main

#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifdef NDEBUG
  Logger::Set_Priority(Logger::LOG_OFF);
#else
  Logger::Set_Priority(Logger::LOG_INFO);
#endif // NDEBUG

  //Sandbox test("scripts/sandbox.lua");
  
  Engine::GSM & GameStageManager = Engine::GSM::get();

  GameStageManager.Init();
  GameStageManager.Loop();
  GameStageManager.Unload();

  return 0;
}
