// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include "glm/glm/vec2.hpp"
#include "DrawToken.h"

namespace Engine
{
  class UIFrame : public Component
  {
  public:
    UIFrame(GameInstance * owner);
    UIFrame(GameInstance * owner, const ParsedObject & obj);

    void SetVisible(bool vis) { item_.setVisible(vis); }
    bool isVisible() const;

    void setTextureFrame(unsigned frame) { item_.setFrame(frame); }

    //glm::mat4 GetMatrix() const { return item_->GetMatrix(); }
    //unsigned GetOrder() const { return item_->GetOrder(); }

    ScaleReference GetScaleReference() const 
    {
      return item_.getScaleReference();
    }

    void SetScaleReference(ScaleReference ref) 
    { item_.setScaleReference(ref); }

    ///GraphicalElementType getType() const { return item_->GetType(); }
	DrawToken getItem() const { return item_; }

  unsigned getTextureFrame() const { return item_.getFrame(); }

  private:
    glm::vec2 position_;
    glm::vec2 scale_;

    DrawToken item_;
  };


  class UIFrameHandler : public ComponentHandler
  {
  public:
    UIFrameHandler(Stage * stage);

    static void StagePaused(unsigned long id, Stage * stage, const Packet & data);
  
    void update();

    void getLuaRegisters() override;

  protected:
    //static void UIFrameMatrixRequest(const UIFrame * sub, Packet & data);

    void ConnectEvents(Component * sub);
  private:

  };
}
