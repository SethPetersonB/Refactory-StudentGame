// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "Disappear.h"
#include "DrawToken.h"
#include "Stage.h"

namespace Engine
{
  Disappear::Disappear(GameInstance* owner) : Component(owner, "Disappear")
  {
    key_ = SDL_SCANCODE_0;
    visible_ = true;
    two_way_ = false;
    placement_trigger_ = false;
  }

  Disappear::Disappear(GameInstance* owner, const ParsedObject& obj) : Component(owner, "Disappear")
  {
    key_ = SDL_GetScancodeFromName(obj.getComponentProperty<std::string>("Disappear", "key").c_str());
    visible_ = obj.getComponentProperty<bool>("Disappear", "visible");
    two_way_ = obj.getComponentProperty<bool>("Disappear", "two_way");
    placement_trigger_ = obj.getComponentProperty<bool>("Disappear", "placement_trigger");
  }

  bool Disappear::IsVisible() const
  {
    return visible_;
  }

  void Disappear::SetVisible(bool visible)
  {
    visible_ = visible;
  }

  SDL_Scancode Disappear::Key() const
  {
    return key_;
  }

  bool Disappear::TwoWay() const
  {
    return two_way_;
  }

  bool Disappear::PlacementTrigger() const
  {
    return placement_trigger_;
  }

  DisappearHandler::DisappearHandler(Stage* stage) : ComponentHandler(stage, "Disappear")
  {
    dependencies_ = { "Sprite" };
  }

  void DisappearHandler::update()
  {
    // set each object invisible/visible if key is pressed
    for (auto i : componentList_)
    {
      Disappear* component = dynamic_cast<Disappear*>(i);
      if (InputSystem::KeyPressed(component->Key()))
      {
        bool visible = component->IsVisible();
        if (component->TwoWay())
        {
          component->SetVisible(!visible);
        }
        else
        {
          component->SetVisible(false);
        }
      }

      if (component->PlacementTrigger())
      {
        Grid& grid = component->getParent().getStage()->GetGrid();
        if (grid.GetNumBlocks() > 0)
        {
          bool visible = component->IsVisible();
          if (component->TwoWay())
          {
            component->SetVisible(!visible);
          }
          else
          {
            component->SetVisible(false);
          }
        }
      }
      component->getParent().RequestData<DrawToken>("Graphic").setVisible(component->IsVisible());

    }
  }

  void DisappearHandler::getLuaRegisters()
  {

  }

  void DisappearHandler::ConnectEvents(Component * base_sub)
  {

  }
}
