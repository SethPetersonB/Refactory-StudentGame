// Primary Author : Philip Nygard
// 
// Co-authors:
//    Kento Murawski (Initial sprite component implementation)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#pragma once

#include "GameInstance.h"
#include "glm/glm/ext.hpp"
#include "shader.h"
//#include "GraphicsPipeline.h"
#include "DrawToken.h"

namespace Engine
{
  class SpriteHandler;
  class Sprite;

  

  class SpriteHandler : public ComponentHandler
  {
  public:
    SpriteHandler(Stage * stage);
    virtual ~SpriteHandler();


    void update();

    void getLuaRegisters() override;
  protected:
    void ConnectEvents(Component * sub);

    //static void SpriteDrawOrderRequest(const Sprite * obj, Packet & data);
    static void SpriteDepthChanged(Sprite * member, const Packet & data);
    static void SpriteDepthSet(Sprite * member, const Packet & data);
    static void SpriteDepthRequest(const Sprite * member, Packet & data);
    //static void SpriteMatrixRequest(const Sprite * item, Packet & data);

  private:

  };

  class Sprite : public Component
  {
  public:
    Sprite(GameInstance * owner);
    Sprite(GameInstance * owner, const ParsedObject & obj);
    /* Sprite(GameInstance * owner,
            float x, float y, float width, float height);*/

    void SetTransform();
    //glm::mat4 GetTransform() { return transform_; }


    DrawToken getItem() const { return item_; }
    virtual ~Sprite();

    //glm::mat4 GetMatrix() const { return item_->GetMatrix(); }
    //GraphicalElementType getType() const { return item_->GetType(); }

    void setTextureFrame(unsigned frame) { item_.setFrame(frame); }
    unsigned getTextureFrame() const { return item_.getFrame(); }

  private:
    float depth;
    float xOffset_;
    float yOffset_;

    DrawToken item_;
  };
}
