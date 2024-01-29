// Primary Author : Dylan Weber
// 
// Co-authors:
//    Philip Nygard (InputSystem clas wrapper)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <luabind/luabind.hpp>

#include "../include/Input.h"
#include "../include/display.h"
#include "../include/GSM.h"
#include "../include/Logger.h"
#include "../include/UIFrame.h"
#include "../include/audio_startup.h"

#define AR_WIDTH 16.f
#define AR_HEIGHT 9.f

using namespace Logger;

namespace Engine
{
  /****************************************************************************/
  /*!
  \brief
  Gets the static input system object

  \return
  input system

  */
  /****************************************************************************/
  const InputSystem & InputSystem::get()
  {
    static InputSystem is;

    return is;
  }

  void InputSystem::Clean()
  {
    InputSystem & is = getMutable();

    if (!is.changeCache_.empty())
    {
      for (auto input : is.changeCache_)
      {
        is.keys_[input].actionFrame = false;
      }

      is.changeCache_.clear();
    }
    // reset mouse data
    if (is.mouseChanged_)
    {
      is.mouse_.m1Action = false;
      is.mouse_.m2Action = false;
      is.mouse_.m3Action = false;
      is.mouse_.scroll = 0;
      is.mouseChanged_ = false;
    }
  }
  /****************************************************************************/
  /*!
  \brief
  Input system update function that loops through keyboard input

  */
  /****************************************************************************/
  bool InputSystem::Update(const SDL_Event & event)
  {
    InputSystem & is = getMutable();



    switch (event.type)
    {
      // Window exit
    case SDL_QUIT:
    {
      if (!GSM::get().isLoading())
      {
        GSM::get().getMessenger().Post("QUIT_CONFIRM", true);
        if (Minimized())
        {
          SDL_RestoreWindow(GSM::get().getDisplay().GetWindow());
        }

        //Stage::GetStage("QuitStage").setStageRunning(true);
        //GSM::get().getMessenger().Post("GSM_END", Message<std::string>("Window closed"));
      }
    }
    break;

    // Key pressed
    case SDL_KEYDOWN:
    {

      if (!is.keys_[event.key.keysym.scancode].held)
      {
        is.keys_[event.key.keysym.scancode].held = true;
        is.keys_[event.key.keysym.scancode].actionFrame = true;
        is.changeCache_.push_back(event.key.keysym.scancode);
      }
    }
    break;

    // Key released
    case SDL_KEYUP:
    {
      is.keys_[event.key.keysym.scancode].held = false;
      is.keys_[event.key.keysym.scancode].actionFrame = true;
      is.changeCache_.push_back(event.key.keysym.scancode);

    }
    break;

    // Mouse button pressed
    case SDL_MOUSEBUTTONDOWN:
    {
      if (event.button.button == SDL_BUTTON_LEFT)
      {
        is.mouse_.m1Down = true;
        is.mouse_.m1Action = true;
        is.mouseChanged_ = true;
      }
      else if (event.button.button == SDL_BUTTON_MIDDLE)
      {
        is.mouse_.m3Down = true;
        is.mouse_.m3Action = true;
        is.mouseChanged_ = true;
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        is.mouse_.m2Down = true;
        is.mouse_.m2Action = true;
        is.mouseChanged_ = true;
      }
    }
    break;

    // Mouse scrolled
    case SDL_MOUSEWHEEL:
    {
      is.mouse_.scroll = event.wheel.y;
      is.mouseChanged_ = true;
    }
    break;


    // Mouse button released
    case SDL_MOUSEBUTTONUP:
    {
      if (event.button.button == SDL_BUTTON_LEFT)
      {
        is.mouse_.m1Down = false;
        is.mouse_.m1Action = true;
        is.mouseChanged_ = true;
      }
      else if (event.button.button == SDL_BUTTON_MIDDLE)
      {
        is.mouse_.m3Down = false;
        is.mouse_.m3Action = true;
        is.mouseChanged_ = true;
      }
      else if (event.button.button == SDL_BUTTON_RIGHT)
      {
        is.mouse_.m2Down = false;
        is.mouse_.m2Action = true;
        is.mouseChanged_ = true;
      }
    }
    break;

    // Mouse moved
    case SDL_MOUSEMOTION:
    {
      is.mouse_.mousePos = glm::vec2(event.motion.x, event.motion.y);
    }
    break;

    // Window events
    case SDL_WINDOWEVENT:
    {
      if (event.window.event == SDL_WINDOWEVENT_RESIZED)
      {
        glm::vec2 size{ event.window.data1, event.window.data2 };
        DrawSystem & disp = GSM::get().getRenderer();
        //if (size.x >= size.y)
        // size.x = size.y * (16.0f / 9.0f);
        //else
        float ar = size.x / size.y;
        static const float LETTER_AR = AR_WIDTH / AR_HEIGHT;
        size_t vWidth;
        size_t vHeight;

        if (ar >= LETTER_AR)
        {
          vWidth = size.y * LETTER_AR;
          vHeight = size.y;
        }
        else
        {
          vHeight = size.x * (1 / LETTER_AR);
          vWidth = size.x;
        }

        size_t wCenterX = size.x / 2;
        size_t wCentexY = size.y / 2;

        size_t offsetX = wCenterX - (vWidth / 2);
        size_t offsetY = wCentexY - (vHeight / 2);

        disp.resize(offsetX, offsetY, vWidth, vHeight);

        glm::vec2 viewSize{ disp.getViewWidth(), disp.getViewHeight() };

        GSM::get().getMessenger().Post("SCREEN_SIZE_CHANGED", viewSize);
        GSM::get().getCamera().SetScreenSize((int)viewSize.x, (int)viewSize.y);
      }
      if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) // minimizing
      {
        Minimized() = true;

        if (!GSM::get().isLoading())
        {
          Stage& stage = Stage::GetStage("TestStage1");
          if (stage.isStageRunning())
          {
            stage.setStageRunning(false);
            Stage& pause = Stage::GetStage("TestMenu1");
            pause.setStageRunning(true);
          }
        }
        GetAudioEngine()->SetAllChannelsMute(true);
      }
      if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
      {
        Minimized() = false;
        GetAudioEngine()->SetAllChannelsMute(false);
      }
      break;
    }
    break;

    default:
      // theres a bunch of other events we don't care about
      return false; // Event wasn't handled (assuming no fallthrough
    }

    return true;  // Event was handled (assuming no fallthrough)
  }

  /****************************************************************************/
  /*!
  \brief
  Checks if a key is held down

  \param key
  The key to check

  \return
  if it is held

  */
  /****************************************************************************/
  bool InputSystem::KeyDown(SDL_Scancode key)
  {
    const Uint8* keyboard = SDL_GetKeyboardState(NULL); // array of keyboard states (pressed or not)
    if (keyboard[key])
      return true;
    else return false;
  }

  /****************************************************************************/
  /*!
  \brief
  Checks if a key is not held down

  \param key
  The key to check

  \return
  if it is not held

  */
  /****************************************************************************/
  bool InputSystem::KeyUp(SDL_Scancode key)
  {
    const Uint8* keyboard = SDL_GetKeyboardState(NULL); // array of keyboard states (pressed or not)
    if (!keyboard[key])
      return true;
    else return false;
  }

  /****************************************************************************/
  /*!
  \brief
  Checks if a key is pressed

  \param key
  The key to check

  \return
  if it is pressed

  */
  /****************************************************************************/
  bool InputSystem::KeyPressed(SDL_Scancode key)
  {
    return getMutable().keys_[key].held && getMutable().keys_[key].actionFrame;
  }

  /****************************************************************************/
  /*!
  \brief
  Checks if a key is not pressed

  \param key
  The key to check

  \return
  if it is not pressed

  */
  /****************************************************************************/
  bool InputSystem::KeyReleased(SDL_Scancode key)
  {
    return !getMutable().keys_[key].held && getMutable().keys_[key].actionFrame;
  }

  static bool lua_KeyDown(int key)
  {
    return InputSystem::KeyDown(SDL_Scancode(key));
  }

  static bool KeyPressedBind(const InputSystem &, SDL_Scancode code)
  {
    return InputSystem::KeyPressed(code);
  }

  static bool KeyReleasedBind(const InputSystem &, SDL_Scancode code)
  {
    return InputSystem::KeyReleased(code);
  }

  static bool KeyDownBind(const InputSystem &, SDL_Scancode code)
  {
    return InputSystem::KeyDown(code);
  }

  luabind::scope InputSystem::GetLuaRegisters()
  {
    using namespace luabind;
    return class_<InputSystem>("InputSystem")
      //.def("KeyDown", &lua_KeyDown)
      .scope[
        def("getSystem", &InputSystem::get),
          class_<SDL_Keycode>("KeyCode")
          .enum_("Keys")[

            // Letters
            value("A", SDL_SCANCODE_A),
              value("B", SDL_SCANCODE_B),
              value("C", SDL_SCANCODE_C),
              value("D", SDL_SCANCODE_D),
              value("E", SDL_SCANCODE_E),
              value("F", SDL_SCANCODE_F),
              value("G", SDL_SCANCODE_G),
              value("H", SDL_SCANCODE_H),
              value("I", SDL_SCANCODE_I),
              value("J", SDL_SCANCODE_J),
              value("K", SDL_SCANCODE_K),
              value("L", SDL_SCANCODE_L),
              value("M", SDL_SCANCODE_M),
              value("N", SDL_SCANCODE_N),
              value("O", SDL_SCANCODE_O),
              value("P", SDL_SCANCODE_P),
              value("Q", SDL_SCANCODE_Q),
              value("R", SDL_SCANCODE_R),
              value("S", SDL_SCANCODE_S),
              value("T", SDL_SCANCODE_T),
              value("U", SDL_SCANCODE_U),
              value("V", SDL_SCANCODE_V),
              value("W", SDL_SCANCODE_W),
              value("X", SDL_SCANCODE_X),
              value("Y", SDL_SCANCODE_Y),
              value("Z", SDL_SCANCODE_Z),

              // Numbers
              value("0", SDL_SCANCODE_0),
              value("1", SDL_SCANCODE_1),
              value("2", SDL_SCANCODE_2),
              value("3", SDL_SCANCODE_3),
              value("4", SDL_SCANCODE_4),
              value("5", SDL_SCANCODE_5),
              value("6", SDL_SCANCODE_6),
              value("7", SDL_SCANCODE_7),
              value("8", SDL_SCANCODE_8),
              value("9", SDL_SCANCODE_9),

              // Function
              value("F1", SDL_SCANCODE_F1),
              value("F2", SDL_SCANCODE_F2),
              value("F3", SDL_SCANCODE_F3),
              value("F4", SDL_SCANCODE_F4),
              value("F5", SDL_SCANCODE_F5),
              value("F6", SDL_SCANCODE_F6),
              value("F7", SDL_SCANCODE_F7),
              value("F8", SDL_SCANCODE_F8),
              value("F9", SDL_SCANCODE_F9),
              value("F10", SDL_SCANCODE_F10),
              value("F11", SDL_SCANCODE_F11),
              value("F12", SDL_SCANCODE_F12),

              // Modifiers
              value("LShift", SDL_SCANCODE_LSHIFT),
              value("RShift", SDL_SCANCODE_RSHIFT),
              value("LCtrl", SDL_SCANCODE_LCTRL),
              value("RCtrl", SDL_SCANCODE_RCTRL),
              value("LAlt", SDL_SCANCODE_LALT),
              value("RAlt", SDL_SCANCODE_RALT),
              value("Enter", SDL_SCANCODE_RETURN),
              value("Return", SDL_SCANCODE_RETURN), // Use both just in case
              value("Backspace", SDL_SCANCODE_BACKSPACE),
              value("Tab", SDL_SCANCODE_TAB),
              value("Escape", SDL_SCANCODE_ESCAPE),
              value("Space", SDL_SCANCODE_SPACE),
              value("Left", SDL_SCANCODE_LEFT),
              value("Right", SDL_SCANCODE_RIGHT),
              value("Up", SDL_SCANCODE_UP),
              value("Down", SDL_SCANCODE_DOWN),

              // Symbols/Misc
              value("Comma", SDL_SCANCODE_COMMA),
              value("Period", SDL_SCANCODE_PERIOD),
              value("Slash", SDL_SCANCODE_SLASH),
              value("Apostrophe", SDL_SCANCODE_APOSTROPHE),
              value("Plus", SDL_SCANCODE_RETURN),
              value("Minus", SDL_SCANCODE_MINUS),
              value("Backslash", SDL_SCANCODE_BACKSLASH)


          ]
      ]

      .def("KeyDown", &KeyDownBind)
              .def("KeyPressed", &KeyPressedBind)
              .def("KeyReleased", &KeyReleasedBind);
  }

  InputSystem & InputSystem::getMutable()
  {
    return const_cast<InputSystem &>(get());
  }

  InputSystem::MouseStatus& InputSystem::getMouse()
  {
    return getMutable().mouse_;
  }

  bool InputSystem::Mouse1ClickedBind(MouseStatus&)
  { 
    return InputSystem::get().mouse_.m1Action && get().mouse_.m1Down; 
  }

  bool InputSystem::Mouse1ReleasedBind(MouseStatus&)
  {
    return InputSystem::get().mouse_.m1Action && !get().mouse_.m1Down;
  }

  bool InputSystem::Mouse1DownBind(MouseStatus&)
  {
    return InputSystem::get().mouse_.m1Down;
  }

  bool InputSystem::Mouse2ClickedBind(MouseStatus&)
  {
    return InputSystem::get().mouse_.m2Action && get().mouse_.m2Down;
  }

  bool InputSystem::Mouse2ReleasedBind(MouseStatus&)
  {
    return InputSystem::get().mouse_.m2Action && !get().mouse_.m2Down;
  }

  bool InputSystem::Mouse2DownBind(MouseStatus&)
  {
    return InputSystem::get().mouse_.m2Down;
  }


  bool InputSystem::Mouse3ClickedBind(MouseStatus&)
  {
    return InputSystem::get().mouse_.m3Action && get().mouse_.m3Down;
  }

  bool InputSystem::Mouse3ReleasedBind(MouseStatus&)
  {
    return InputSystem::get().mouse_.m3Action && !get().mouse_.m3Down;
  }

  bool InputSystem::Mouse3DownBind(MouseStatus&)
  {
    return InputSystem::get().mouse_.m3Down;
  }


  int InputSystem::MouseScrolledBind(MouseStatus&)
  {
    return get().mouse_.scroll;
  }

  glm::vec2 InputSystem::MousePositionBind(MouseStatus&)
  {
    return get().mouse_.mousePos;
  }


  luabind::scope InputSystem::GetLuaRegistersMouse()
  {
    using namespace luabind;
    return class_<InputSystem::MouseStatus>("Mouse")
      .scope[def("getSystem", &InputSystem::getMouse)]
      .def("Mouse1Clicked", &InputSystem::Mouse1ClickedBind)
      .def("Mouse2Clicked", &InputSystem::Mouse2ClickedBind)
      .def("Mouse3Clicked", &InputSystem::Mouse3ClickedBind)
      .def("Mouse1Released", &InputSystem::Mouse1ReleasedBind)
      .def("Mouse2Released", &InputSystem::Mouse2ReleasedBind)
      .def("Mouse3Released", &InputSystem::Mouse3ReleasedBind)
      .def("Mouse1Down", &InputSystem::Mouse1DownBind)
      .def("Mouse2Down", &InputSystem::Mouse2DownBind)
      .def("Mouse3Down", &InputSystem::Mouse3DownBind)
      .property("scroll", &InputSystem::MouseScrolledBind)
      .property("position", &InputSystem::MousePositionBind);
  }
};
