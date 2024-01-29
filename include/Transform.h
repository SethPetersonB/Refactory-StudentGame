// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include "GameInstance.h"
#include "glm/glm/glm.hpp"
#include "glm/glm/vec2.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "ParsedObjects.h"

namespace Engine
{
  class Transform;  // Forward declaration of Transform component

  

  class TransformHandler : public ComponentHandler
  {
  public:
    TransformHandler(Stage * stage);
    virtual ~TransformHandler();

    void update();
    void getLuaRegisters() override;

    //void ConnectEvents(Transform * sub) const;

    static void TransformRotated(Transform * member, const Packet & data);
    static void TransformRotationSet(Transform * member, const Packet & data);
    static void TransformScaleSet(Transform * member, const Packet & data);

    static void TransformPositionRequest(const Transform * member, Packet & data);
    static void TransformRotationRequest(const Transform * member, Packet & data);
    static void TransformWidthRequest(const Transform * member, Packet & data);
    static void TransformHeightRequest(const Transform * member, Packet & data);

  protected:
    void ConnectEvents(Component * sub);

  private:
    

  };

  class Transform : public Component
  {
  public:
    Transform(GameInstance * owner);
    Transform(GameInstance * owner, const ParsedObject & obj);
    Transform(GameInstance * owner, float x, float y, float width, float height, float rot = 0);

    virtual ~Transform();

    glm::mat4 getTransform();
    glm::mat4 getScale() const;
    glm::mat4 getRotation() const;
    glm::mat4 getTranslate() const;

    glm::vec2 getPos() const;
    float getDepth() const;
    float getRot() const;
    float getWidth() const { return width_; }
    float getHeight() const { return height_; }

    void setX(float x);
    void setY(float y);
    void setDepth(float z) { depth_ = z; }
    void addRot(float rot){ rot_ += rot; }
    void setRot(float rot);
    void setPos(const glm::vec2 & pos, bool dispatchEvent = true);
    void setPos(float x, float y);
	inline void setWidth(float w) { width_ = w; }
	inline void setHeight(float h) { height_ = h; }
	inline void setStart(glm::vec2 pos) { startPos_ = pos; }
	inline const glm::vec2 getStart() const { return startPos_; }
    void OnMoved(const Packet & payload);
    void OnPositionSet(const Packet & payload);
    void OnZSet(const Packet& payload);

    glm::vec2 addPos(const glm::vec2 & rhs);
    glm::vec2 addPos(float x, float y);

    glm::vec2 addX(float x);
    glm::vec2 addY(float y);
    
  private:
    glm::mat4 model_;

    float x_;   // X coordinate of the component
    float y_;   // Y coordinate of the component
    float depth_;
    float width_;
    float height_;
    float rot_; // Rotation (in radians) of the component
	glm::vec2 startPos_;
  };
}

