// Primary Author : Dylan Weber
// 
// Co-authors:
//    Philip Nygard (InputSystem clas wrapper)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#pragma once
#include "GameInstance.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <ctime>
#include <unordered_map>
#include "glm/glm/vec2.hpp"

// Forward declaration of luabind classes
namespace luabind
{
  struct scope;
}

namespace Engine
{
  class InputSystem
  {
  public:

    struct MouseStatus
    {
      MouseStatus() : m1Action(false), m2Action(false), m1Down(false), m2Down(false), m3Action(false), m3Down(false), scroll(0) {}

      bool m1Action;
      bool m2Action;
      bool m1Down;
      bool m2Down;
      bool m3Action;
      bool m3Down;

      glm::vec2 mousePos;

      int scroll;
    };

    static const InputSystem & get();
    static void Clean();
    static bool Update(const SDL_Event & e);
    static bool KeyDown(SDL_Scancode key); // check if key has is down
    static bool KeyUp(SDL_Scancode key); // check if key is up
    static bool KeyPressed(SDL_Scancode key);
    static bool KeyReleased(SDL_Scancode key);

    static bool Mouse1Clicked() { return get().mouse_.m1Action && get().mouse_.m1Down; } 
    static bool Mouse1Released() { return get().mouse_.m1Action && !get().mouse_.m1Down; }
    static bool Mouse1Down() { return get().mouse_.m1Down; }
    
    static bool Mouse2Clicked() { return get().mouse_.m2Action && get().mouse_.m2Down; }
    static bool Mouse2Released() { return get().mouse_.m2Action && !get().mouse_.m2Down; }
    static bool Mouse2Down() { return get().mouse_.m2Down; }

    static bool Mouse3Clicked() { return get().mouse_.m3Action && get().mouse_.m3Down; }
    static bool Mouse3Released() { return get().mouse_.m3Action && !get().mouse_.m3Down; }
    static bool Mouse3Down() { return get().mouse_.m3Down; }

    static int MouseScrolled() { return get().mouse_.scroll; }
    
    static glm::vec2 MousePosition() { return get().mouse_.mousePos; }

    static luabind::scope GetLuaRegisters();
    static luabind::scope GetLuaRegistersMouse();


  private:

    static InputSystem & getMutable();

    InputSystem() {}

    InputSystem(const InputSystem &) = delete;
    InputSystem & operator=(const InputSystem &) = delete;

    struct KeyStatus
    {
      KeyStatus() : held(false), actionFrame(false) {}

      bool held;        // Key is being held
      bool actionFrame; // Key has been pressed or released that frame
    };


    // lua panic

    static bool Mouse1ClickedBind(MouseStatus&);
    static bool Mouse1ReleasedBind(MouseStatus&);
    static bool Mouse1DownBind(MouseStatus&);

    static bool Mouse2ClickedBind(MouseStatus&);
    static bool Mouse2ReleasedBind(MouseStatus&);
    static bool Mouse2DownBind(MouseStatus&);

    static bool Mouse3ClickedBind(MouseStatus&);
    static bool Mouse3ReleasedBind(MouseStatus&);
    static bool Mouse3DownBind(MouseStatus&);

    static int MouseScrolledBind(MouseStatus&);
    static glm::vec2 MousePositionBind(MouseStatus&);

    // lua end panic

    static MouseStatus& getMouse();
    static bool& Minimized() { return getMutable().minimized_; }

    MouseStatus mouse_;
    std::unordered_map<SDL_Scancode, KeyStatus> keys_;
    std::vector<SDL_Scancode> changeCache_;

    bool mouseChanged_;
    bool minimized_ = false;
  };
}
