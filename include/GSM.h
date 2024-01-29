// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <memory>

#include "display.h"
//#include "sprite.h"
#include "camera.h"
#include "DrawSystem.h"
//#include "Transform.h"
//#include "Physics.h"
//#include "GraphicsPipeline.h"
//#include "grid.h"
#include "Stage.h"

namespace Engine
{
  //void GSM_Init();
  //void GSM_Loop();
  //void GSM_Unload();
  //void GSM_InitShaders();
  
  class Stage;  // Forward declaraion of Stage class

  /* Stages: 
    - Each stage has it's own sets of GameInstances (and component handlers?)
    
    - Can be ordered with a command like setOrder or bringToFront or something
     like that that affects primarily what order a stage is drawn
     and/or updated
    
    - Can either be in a running or paused state
    
    - All currently running stages are updated each frame. Non-rendered stages
      should not be drawn, but logic can can still be ran on them
    
    - Can be reset
    
    - Ideally logic would be ran through external scripts (like Lua), but as is,
      different stages (HUD, gamespace, menus) will likely need to be derrived 
      from the base stage class

    - Alternatively, stages can be specified with necessary elements in a 
      Json file and loaded in as needed. This means not needing to derrive
      stages, but a good parser for stages would be needed, and possibly an 
      editor

    - Needs initialize, update, and free functions (Load/Unload are essentially
      the constructor/destructor)

    - Destructor will always call free, but the constructor doesn't necessarily
      need to call initialize

    - List order should be reverse of the order they're updated in (order 0 is
      drawn and updated last, and appears on top of all other elements)
  */
  class GSM;

  GSM & GetGSM();

  // Singleton GSM class
  class GSM
  {
  public:

    GSM(const GSM &) = delete;
    GSM & operator=(const GSM &) = delete;

    static GSM & get();

    void Init();
    void Loop();
    void Unload();

    Messenger & getMessenger();
    Display & getDisplay();
    Camera & getCamera();
    DrawSystem & getRenderer();
    bool isLoading() const { return loading_; }

  private:

    GSM();

    static void OnEnd(const Packet & payload);

    //void InitShaders();

    bool ending_;
    bool loading_ = true;
    Display disp_;
    std::unique_ptr<DrawSystem> renderer_;
    Messenger mess_;
    Camera cam_;
    //GFXPipe & gfxPipe_;
	  //Grid grid_;
  };
}
