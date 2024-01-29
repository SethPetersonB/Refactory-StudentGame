// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include "UIFrame.h"
#include <vector>

namespace Engine
{
  class ButtonMenu : public Component
  {
  public:
    enum MenuType { VERTICAL, HORIZONTAL };

    ButtonMenu(GameInstance * owner);
    ButtonMenu(GameInstance * owner, const ParsedObject & obj);

    void AddNewButton(unsigned long id);
    void SetButton(unsigned button);
    void Click();
    void NextButton();
    void PrevButton();
    void ClearButton();
    
    MenuType GetMenuType() { return type_; }

    void OnMouseEnterButton(GameInstance * obj, const Packet & paylaod);
    void OnMouseExitButton(GameInstance * obj, const Packet & payload);

  protected:
    void SelectButton();
    void DeselectButton();

  private:
    std::vector<unsigned long> buttons_;
    int current_;
    MenuType type_;

  };

  class ButtonMenuHandler : public ComponentHandler
  {
  public:
    ButtonMenuHandler(Stage * owner);

    void update();

    void ConnectEvents(Component * base_sub);

  private:

  };
}