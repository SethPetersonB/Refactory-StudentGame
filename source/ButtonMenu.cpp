// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/ButtonMenu.h"
#include "../include/GSM.h"
#include "../include/Input.h"
#include "../include/audio_startup.h"
#include <algorithm>
#include "DrawToken.h"

#define COLORSHIFT 128
namespace Engine
{

  // Event fired when the mouse enters a button. Sets the button to active
  void ButtonMenu::OnMouseEnterButton(GameInstance * obj, const Packet & paylaod)
  {
    unsigned long id = obj->getId();

    for (unsigned i = 0; i < buttons_.size(); i++)
    {
      if (buttons_[i] == id && i != current_)
      {
        SetButton(i);
      }
    }
  }

  //Event fired when the mouse exits a button. Deselects the button
  void ButtonMenu::OnMouseExitButton(GameInstance * obj, const Packet & payload)
  {
    unsigned long id = obj->getId();

    for (unsigned i = 0; i < buttons_.size(); i++)
    {
      if (buttons_[i] == id && i == current_)
        SetButton(-1);
    }
  }

  // Default constructor for button menus. Sets type to vertical 
  ButtonMenu::ButtonMenu(GameInstance * owner) :
    Component(owner, "ButtonMenu"),
    type_(VERTICAL), current_(-1)
  {}

  // Object constructor for button menus
  ButtonMenu::ButtonMenu(GameInstance * owner, const ParsedObject & obj) :
    Component(owner, "ButtonMenu"), current_(-1)
  {
    std::string type = obj.getComponentProperty<std::string>("ButtonMenu", "type");

    if (type == "Horizontal")
      type_ = HORIZONTAL;
    else
      type_ = VERTICAL;
  }

  // Adds a new button to the button menu and sets its sprite to the down position
  void ButtonMenu::AddNewButton(unsigned long id)
  {
    GameInstance & obj = getParent().getStage()->getInstanceFromID(id);

    DrawToken item = obj.RequestData<DrawToken>("Graphic");

    item.setFrame(1);

    buttons_.push_back(id);

    Messenger & objMessenger = obj.getMessenger();

    objMessenger.Unsubscribe(objMessenger, "MouseEntered");
    objMessenger.Unsubscribe(objMessenger, "MouseExit");
    
    using namespace std::placeholders;

    SUBSCRIBER_ACTION onEnter = std::bind(&ButtonMenu::OnMouseEnterButton, this, &obj, std::placeholders::_1);
    SUBSCRIBER_ACTION onExit = std::bind(&ButtonMenu::OnMouseExitButton, this, &obj, std::placeholders::_1);

    objMessenger.Subscribe(objMessenger, "MouseEntered", onEnter);
    objMessenger.Subscribe(objMessenger, "MouseExit", onExit);
  }

  // Sets the current active button to the given index. Sets none to active if given negative number
  void ButtonMenu::SetButton(unsigned button)
  {
    if (current_ >= 0)
      DeselectButton();

    current_ = button;

    if (current_ >= 0)
      SelectButton();
  }

  // "Clicks" a button. Fired when enter key is pressed
  void ButtonMenu::Click()
  {
    if (current_ >= 0)
    {
      try
      {
        GameInstance & obj = getParent().getStage()->getInstanceFromID(buttons_[current_]);

        obj.PostMessage("Clicked", glm::vec3(0, 0, 1));
      }

      catch (const std::out_of_range &) {}
    }
  }

  // Sets the next button on the menu to active
  void ButtonMenu::NextButton()
  {
    if (current_ < 0)
      SetButton(0);
    else
      SetButton((current_ + 1) % buttons_.size());
  }

  // Sets the previous button on the menu to active
  void ButtonMenu::PrevButton()
  {
    if (current_ < 0)
      SetButton(buttons_.size() - 1);
    else
    {
      int current = (current_ - 1);
      if (current < 0)
        current = buttons_.size() - 1;
      SetButton(current);
    }
  }

  // Selects the current button, changing it's sprite to "down"
  void ButtonMenu::SelectButton()
  {
    if (current_ >= 0 && buttons_.size() != 0)
    {
      try
      {
        GameInstance & obj = getParent().getStage()->getInstanceFromID(buttons_[current_]);


        DrawToken item = obj.RequestData<DrawToken>("Graphic");

        /* Due to poor implementation, this will stop the other sound but this sound won't get played. */
        int channelFound = GetAudioEngine()->FindSoundChannel("button_sound_fast.wav");
        if ( channelFound != -1 )
        {
          GetAudioEngine()->StopChannel( channelFound );
          GetAudioEngine()->PlaySounds("button_sound_fast.wav", Vector3(), -20.0f);
        }
        else
        {
          GetAudioEngine()->StopChannel(channelFound);
        }

        GetAudioEngine()->PlaySounds("button_sound_fast.wav", Vector3(), -20.0f);

        item.setFrame(0);
      }
      catch (const std::out_of_range &)
      {
      }
    }
  }

  // Deselectst eh current button, changing it's sprite to "up"
  void ButtonMenu::DeselectButton()
  {
    if (current_ >= 0 && buttons_.size() != 0)
    {
      try
      {
        GameInstance & obj = getParent().getStage()->getInstanceFromID(buttons_[current_]);

        DrawToken item = obj.RequestData<DrawToken>("Graphic");

        int channelFound = GetAudioEngine()->FindSoundChannel("button_sound.wav");
        if ( channelFound != -1 )
        {
          GetAudioEngine()->StopChannel(channelFound);
          GetAudioEngine()->PlaySounds("button_sound.wav", Vector3(), -1.0f);
        }

        glm::vec4 color = item.getShade();

        item.setFrame(1);

      }
      catch (const std::out_of_range &)
      {
      }
    }
  }

  // Constructor for the button menu handler
  ButtonMenuHandler::ButtonMenuHandler(Stage * stage) :
    ComponentHandler(stage, "ButtonMenu")
  {}

  // Updates the button menu handler and all components on it. Checks for keyboard input and sets the
  // current button apropriatly
  void ButtonMenuHandler::update()
  {
    for (auto & component : componentList_)
    {
      ButtonMenu * menu = static_cast<ButtonMenu *>(component);

      SDL_Scancode next, next_alt;
      SDL_Scancode previous, previous_alt;

      switch (menu->GetMenuType())
      {
      case ButtonMenu::HORIZONTAL:
        previous = SDL_SCANCODE_RIGHT;
        previous_alt = SDL_SCANCODE_D;
        next = SDL_SCANCODE_LEFT;
        next_alt = SDL_SCANCODE_A;
        break;
      default:
        previous = SDL_SCANCODE_UP;
        previous_alt = SDL_SCANCODE_W;
        next = SDL_SCANCODE_DOWN;
        next_alt = SDL_SCANCODE_S;
        break;
      }

      if (InputSystem::KeyPressed(next) || InputSystem::KeyPressed(next_alt))
        menu->NextButton();

      if (InputSystem::KeyPressed(previous) || InputSystem::KeyPressed(previous_alt))
        menu->PrevButton();

      if (InputSystem::KeyPressed(SDL_SCANCODE_RETURN))
        menu->Click();
    }
  }

  // Event for when a new button is added to a menu
  static void OnButtonAdd(ButtonMenu * sub, const Packet & payload)
  {
    sub->AddNewButton(payload.getData<unsigned long>());
  }

  // Event connection for button menus
  void ButtonMenuHandler::ConnectEvents(Component * base_sub)
  {
    Messenger & objMessenger = base_sub->getParent().getMessenger();

    ButtonMenu * sub = static_cast<ButtonMenu *>(base_sub);

    using namespace std::placeholders;

    SUBSCRIBER_ACTION onAdd = std::bind(OnButtonAdd, sub, std::placeholders::_1);

    objMessenger.Subscribe(objMessenger, "AddButton", onAdd);
  }
}
