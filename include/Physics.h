// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include "glm/glm/vec2.hpp"
#include <unordered_set>

namespace Engine
{
  class Physics;
  class PhysicsHandler;
  class ColliderHandler;
  class Collider;
  class Collision;
  class GameInstance;
  class CircleCollider;
  class RectangleCollider;

  class ColliderHandler : public ComponentHandler
  {
  public:

    ColliderHandler(Stage * stage);
    virtual ~ColliderHandler() {};
    void update();

    // collision functions
    bool CheckCollision(GameInstance & first, GameInstance & second);

  protected:
    void ConnectEvents(Component * sub);


  private:

  };

  class Collider : public Component
  {
  public:
    typedef std::unordered_set<unsigned long> COLLISION_LIST;
    Collider(GameInstance* owner);
    virtual ~Collider() {};

    bool IsColliding(unsigned long other) { return collisions_.find(other) != collisions_.end(); }
    COLLISION_LIST & getCollisions() { return collisions_; }

    void AddCollision(unsigned long other) { collisions_.insert(other); }
    void RemoveCollision(unsigned long other) { collisions_.erase(other); }
    void UpdateCollisions(bool status) { needsUpdate_ = status; }

    bool NeedsUpdate() const { return needsUpdate_; }
    void SetUpdate(bool b) { needsUpdate_ = b; }
  private:
    COLLISION_LIST collisions_;
    bool needsUpdate_;
  };

  class CircleCollider : public Collider
  {
  public:
    CircleCollider(GameInstance* owner, float radius = 2);
    virtual ~CircleCollider() {};
    float getRadius() const;

  private:
    float radius_;
  };

  class RectangleCollider : public Collider
  {
  public:
    RectangleCollider(GameInstance* owner);
    virtual ~RectangleCollider() {};
    float getHeight() const;
    float getWidth() const;

  private:
    float height_;
    float width_;
  };

  class PhysicsHandler : public ComponentHandler
  {
  public:

    PhysicsHandler(Stage * stage);
    virtual ~PhysicsHandler() {};

    void update();
    void getLuaRegisters() override;

  protected:
    void ConnectEvents(Component * sub);

  private:

  };

  class Physics : public Component
  {
  public:
    Physics(GameInstance* owner);
    Physics(GameInstance* owner, glm::vec2 velocity, glm::vec2 acceleration);

    virtual ~Physics() {};

    const glm::vec2& getVelocity() const;
    const glm::vec2 getAcceleration() const;
    void setVelocity(glm::vec2 v);
    void setAcceleration(glm::vec2 a);
    void addVelocity(glm::vec2 v);
    void addAcceleration(glm::vec2 a);
    void setDepthVelocity(float v);
    void setDepthAcceleration(float v);

    void onVelocityAdd(const Packet& payload);
    void onVelocitySet(const Packet& payload);
    void onAccelerationAdd(const Packet& payload);
    void onAccelerationSet(const Packet& payload);

    void PhysicsVelocityRequest(Packet& data);
    void PhysicsAccelerationRequest(Packet& data);

    float getDepthVelocity() const;
    float getDepthAcceleration() const;

  private:
    glm::vec2 velocity_;
    glm::vec2 acceleration_;
    float depthVelocity_;
    float depthAcceleration_;
  };
}
