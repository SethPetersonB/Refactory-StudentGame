// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/MenuButtons.h"
#include "../include/UIFrame.h"
#include "../include/Input.h"

namespace Engine
{

  /****************************************************************************/
  /*!
  \brief
  Event for when a button is selected

  \param button
  The button

  \param previous
  Current stage

  \param next
  The stage the button will go to

  \param payload
  The data of the message (mouse position)

  */
  /****************************************************************************/
  void MenuButtons::ButtonClickedEvent(GameInstance * button, Stage * previous, Stage * next, const Packet& payload)
  {
    // get variables
    glm::vec2 mousepos = static_cast<const Message<glm::vec2>&>(payload).data;
    DrawToken item = button->RequestData<DrawToken>("Graphic");
    glm::vec2 position = item.getPosition();
    glm::vec2 scale = item.getScale();
    float width = scale.x;
    float height = scale.y;
    GSM & gsm = GSM::get();

    // change stages
    previous->setStageRunning(false);
    next->setStageRunning(true);
    //why
    //if (button->getObjectType() == "Yesbutton") // quit game
    //{
    //  gsm.getMessenger().Post("GSM_END", Message<std::string>("Clicked exit button"));
    //}

  }

  /****************************************************************************/
  /*!
  \brief
  Event to make an object invisible if a certain stage is running

  \param g
  The object

  \param payload
  Data of message

  */
  /****************************************************************************/
  void MenuButtons::InvisibleEvent(GameInstance* g, const Packet& payload)
  {
    const bool b = dynamic_cast<const Message<bool>&>(payload).data;
    if (b)
    {
      dynamic_cast<UIFrame*>(g->getComponent("UIFrame"))->SetVisible(false);
    }
  }

}
