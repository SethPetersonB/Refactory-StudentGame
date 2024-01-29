// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Physics.h"
#include "../include/Transform.h"
#include "../include/display.h"
#include "../include/sprite.h"
#include "../include/GSM.h"
#include "../include/Logger.h"
#include "../include/Mesh.h"
#include <algorithm>

using namespace Logger;
namespace Engine
{

  /****************************************************************************/
  /*!
    \brief
    Basic constructor for the Physics component

    \param owner
      Owner of the physics component
  */
  /****************************************************************************/
  Physics::Physics(GameInstance* owner) : Component(owner, "Physics")
  {
    velocity_.x = 0;
    velocity_.y = 0;
    acceleration_.x = 0;
    acceleration_.y = 0;
    depthVelocity_ = 0;
    depthAcceleration_ = 0;
  }

  /****************************************************************************/
  /*!
    \brief
      Non-standard constructor fot the Physics component. Sets initial
      velocity and acceleration

    \param owner
      Owner of the copmonent

    \param velocity
      Initial velocity of the object

    \param acceleration
      Initial acceleration of the object
  */
  /****************************************************************************/
  Physics::Physics(GameInstance* owner, glm::vec2 velocity, glm::vec2 acceleration) : Component(owner, "Physics")
  {
    velocity_ = velocity;
    acceleration_ = acceleration;
    depthVelocity_ = 0;
    depthAcceleration_ = 0;
  }

  /****************************************************************************/
  /*!
  \brief
    Getter

  \return
    A private member variable
  */
  /****************************************************************************/
  const glm::vec2& Physics::getVelocity() const
  {
    return velocity_;
  }

  /****************************************************************************/
  /*!
  \brief
  Getter

  \return
  A private member variable
  */
  /****************************************************************************/
  const  glm::vec2 Physics::getAcceleration() const
  {
    return acceleration_;
  }

  /****************************************************************************/
  /*!
  \brief
  Getter

  \return
  A private member variable
  */
  /****************************************************************************/
  float RectangleCollider::getHeight() const
  {
    return height_;
  }

  /****************************************************************************/
  /*!
  \brief
  Getter

  \return
  A private member variable
  */
  /****************************************************************************/
  float RectangleCollider::getWidth() const
  {
    return width_;
  }


  /****************************************************************************/
  /*!
  \brief
  Adds to velocity

  \param v
  The vector to add to velocity
  */
  /****************************************************************************/
  void Physics::addVelocity(glm::vec2 v)
  {
    velocity_ += v;
  }

  /****************************************************************************/
  /*!
  \brief
  Adds to acceleration

  \param a
  The vector to add to acceleration
  */
  /****************************************************************************/
  void Physics::addAcceleration(glm::vec2 a)
  {
    acceleration_ += a;
  }

  /****************************************************************************/
  /*!
  \brief
  Adds to acceleration

  \param a
  The vector to add to acceleration
  */
  /****************************************************************************/
  float CircleCollider::getRadius() const
  {
    return radius_;
  }


  /****************************************************************************/
  /*!
  \brief
  sets velocity

  \param v
  The vector to set to velocity
  */
  /****************************************************************************/
  void Physics::setVelocity(glm::vec2 v)
  {
    velocity_ = v;
  }

  /****************************************************************************/
  /*!
  \brief
  sets acceleration

  \param a
  The vector to set to acceleration
  */
  /****************************************************************************/
  void Physics::setAcceleration(glm::vec2 a)
  {
    acceleration_ = a;
  }

  /****************************************************************************/
  /*!
  \brief
   Messaging function that sets data

  \param payload
   The data to set
  */
  /****************************************************************************/
  void Physics::onVelocityAdd(const Packet& payload)
  {
    glm::vec2 delta = dynamic_cast<const Message<glm::vec2> *>(&payload)->data;
    addVelocity(delta);
  }

  /****************************************************************************/
  /*!
  \brief
  Messaging function that sets data

  \param payload
  The data to set
  */
  /****************************************************************************/
  void Physics::onVelocitySet(const Packet& payload)
  {
    glm::vec2 delta = dynamic_cast<const Message<glm::vec2>&>(payload).data;
    setVelocity(delta);
  }

  /****************************************************************************/
  /*!
  \brief
  Messaging function that sets data

  \param payload
  The data to set
  */
  /****************************************************************************/
  void Physics::onAccelerationSet(const Packet& payload)
  {
    glm::vec2 delta = dynamic_cast<const Message<glm::vec2> *>(&payload)->data;
    setAcceleration(delta);
  }

  /****************************************************************************/
  /*!
  \brief
  Messaging function that sets data

  \param payload
  The data to set
  */
  /****************************************************************************/
  void Physics::onAccelerationAdd(const Packet& payload)
  {
    glm::vec2 delta = dynamic_cast<const Message<glm::vec2> *>(&payload)->data;
    addAcceleration(delta);
  }

  /****************************************************************************/
  /*!
  \brief
  Messaging function that requests data

  \param payload
  The data to request
  */
  /****************************************************************************/
  void Physics::PhysicsVelocityRequest(Packet& data)
  {
    dynamic_cast<Message<glm::vec2> &>(data).data = getVelocity();
  }

  /****************************************************************************/
  /*!
  \brief
  Messaging function that requests data

  \param payload
  The data to request
  */
  /****************************************************************************/
  void Physics::PhysicsAccelerationRequest(Packet& data)
  {
    dynamic_cast<Message<glm::vec2> &>(data).data = getAcceleration();
  }

  /****************************************************************************/
  /*!
  \brief
  PhysicsHandler constructor

  \param stage
  The stage it is on
  */
  /****************************************************************************/
  PhysicsHandler::PhysicsHandler(Stage * stage) : ComponentHandler(stage, "Physics")
  {
    dependencies_ = { "Transform" };
  }


  /****************************************************************************/
  /*!
  \brief
  Connects messaging events for objects with the physics component.

  \param base_sub
  The base component pointer that will subscribe to events
  */
  /****************************************************************************/
  void PhysicsHandler::ConnectEvents(Component * base_sub)
  {
    Physics * sub = dynamic_cast<Physics *>(base_sub);

    using namespace std::placeholders;

    Messenger& objMessenger = sub->getParent().getMessenger();

    SUBSCRIBER_ACTION onVelAdd = std::bind(&Physics::onVelocityAdd, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onAccAdd = std::bind(&Physics::onAccelerationAdd, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onVelSet = std::bind(&Physics::onVelocitySet, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onAccSet = std::bind(&Physics::onAccelerationSet, sub, std::placeholders::_1);

    REQUEST_ACTION velRequest = std::bind(&Physics::PhysicsVelocityRequest, sub, std::placeholders::_1);
    REQUEST_ACTION accRequest = std::bind(&Physics::PhysicsAccelerationRequest, sub, std::placeholders::_1);

    objMessenger.Subscribe(objMessenger, "AddVelocity", onVelAdd);
    objMessenger.Subscribe(objMessenger, "AddAcceleration", onAccAdd);
    objMessenger.Subscribe(objMessenger, "SetVelocity", onVelSet);
    objMessenger.Subscribe(objMessenger, "SetAcceleration", onAccSet);

    objMessenger.SetupRequest("Velocity", velRequest);
    objMessenger.SetupRequest("Acceleration", accRequest);
  }

  /****************************************************************************/
  /*!
  \brief
  Update function for physics components

  */
  /****************************************************************************/
  void PhysicsHandler::update()
  {
    for (auto component : componentList_)
    {
      Physics * comp = static_cast<Physics *>(component);

      // update velocity based on acceleration
      float dt = GSM::get().getDisplay().GetFrameTime();
      glm::vec2 acceleration = comp->getAcceleration();
      comp->addVelocity(acceleration * dt); // v += a * dt
      glm::vec2 velocity = comp->getVelocity();

      float dAccel = comp->getDepthAcceleration();
      float dVel = comp->getDepthVelocity();

      comp->setDepthVelocity(dVel + dAccel * dt);
      dVel = comp->getDepthVelocity();

      if (velocity != glm::vec2()) // if velocity is not zero
      {
        component->getParent().getMessenger().Post("PositionMoved", Message<glm::vec2>(velocity * dt)); // add to position
      }

      if (abs(dVel) > 0)
      {
        float depth = comp->getParent().RequestData<float>("TransformDepth");
        comp->getParent().PostMessage("TransformDepthSet", depth + dVel * dt);
      }
    }

  }

  void PhysicsHandler::getLuaRegisters()
  {
    using namespace luabind;

    getStage()->registerLuaModule(
      class_<Physics, Component>("Physics")
      .property("velocity", &Physics::getVelocity, &Physics::setVelocity)
      .property("acceleration", &Physics::getAcceleration, &Physics::setAcceleration)
      .property("depthVelocity", &Physics::getDepthVelocity, &Physics::setDepthVelocity)
      .property("depthAcceleration", &Physics::getDepthAcceleration, &Physics::setDepthAcceleration)
    );
  }

  /****************************************************************************/
  /*!
  \brief
  Collider constructor

  \param owner
  The Gameinstance that owns this component

  */
  /****************************************************************************/
  Collider::Collider(GameInstance* owner) : Component(owner, "Collider"), needsUpdate_(true)
  {

  }


  /****************************************************************************/
  /*!
  \brief
  Collider constructor

  \param owner
  The Gameinstance that owns this component

  \param radius
  Radius of circle

  */
  /****************************************************************************/
  CircleCollider::CircleCollider(GameInstance* owner, float radius) : Collider(owner)
  {
    radius_ = radius;
  }

  /****************************************************************************/
  /*!
  \brief
  Collider constructor

  \param owner
  The Gameinstance that owns this component

  */
  /****************************************************************************/
  RectangleCollider::RectangleCollider(GameInstance* owner) : Collider(owner)
  {
    height_ = owner->RequestData<float>("Height");
    width_ = owner->RequestData<float>("Width");
  }

  /****************************************************************************/
  /*!
  \brief
  ColliderHandler constructor

  \param stage
  The stage it is on

  */
  /****************************************************************************/
  ColliderHandler::ColliderHandler(Stage * stage) : ComponentHandler(stage, "Collider")
  {
    dependencies_ = { "Physics", "Transform" };
  }

  /****************************************************************************/
  /*!
  \brief
  ColliderHandler update function

  */
  /****************************************************************************/
  void ColliderHandler::update()
  {
    std::vector < std::pair<unsigned long, unsigned long>> newCollisions;
    std::vector < std::pair<unsigned long, unsigned long>> endedCollisions;

    for (unsigned i = 0; i < componentList_.size(); ++i)
    {
      Collider * collider = static_cast<Collider *>(componentList_[i]);

      // check if object is moving
      // this check was literally all I had to do to fix collision...
      glm::vec2 velocity = componentList_[i]->getParent().RequestData<glm::vec2>("Velocity");
      if (velocity.x != 0 || velocity.y != 0)  // moving
        collider->SetUpdate(true);

      else
        collider->SetUpdate(false);

     if (collider->NeedsUpdate())
      {
        Collider::COLLISION_LIST & collisions = collider->getCollisions();
        for (auto & other : collisions)
        {
          try
          {
            GameInstance & obj = getStage()->getInstanceFromID(other);
            if (!CheckCollision(collider->getParent(), obj))
            {
              unsigned long colId = collider->getParent().getId();
              unsigned long mx = std::max(colId, other);
              unsigned long mn = std::min(colId, other);

              auto pair = std::make_pair(mx, mn);

              if (std::find(endedCollisions.begin(), endedCollisions.end(), pair) == endedCollisions.end())
                endedCollisions.push_back(pair);
            }
          }
          catch (const std::out_of_range &) {}
        }

        // Check backwards through list for new collisions
       for (unsigned j = 0; j < componentList_.size(); j++)
        {
          GameInstance& other = static_cast<Collider *>(componentList_[j])->getParent();
          if (CheckCollision(collider->getParent(), other) && !collider->IsColliding(other.getId()))
            newCollisions.push_back(std::make_pair(collider->getParent().getId(), other.getId()));
        }
      }
    }
      // Loop throug new collisios and send events
      for (auto & newCol : newCollisions)
      {
        try
        {
          GameInstance & obj = getStage()->getInstanceFromID(newCol.first);
          obj.PostMessage("CollisionStarted", newCol.second);
        }
        catch (const std::out_of_range &) {}

        try
        {
          GameInstance & obj = getStage()->getInstanceFromID(newCol.second);
          obj.PostMessage("CollisionStarted", newCol.first);
        }
        catch (const std::out_of_range &) {}
      }

      for (auto & endCol : endedCollisions)
      {
        try
        {
          GameInstance & obj = getStage()->getInstanceFromID(endCol.first);
          obj.PostMessage("CollisionEnded", endCol.second);
        }
        catch (const std::out_of_range &) {}

        try
        {
          GameInstance & obj = getStage()->getInstanceFromID(endCol.second);
          obj.PostMessage("CollisionEnded", endCol.first);
        }
        catch (const std::out_of_range &) {}
      }
      
  }

  /****************************************************************************/
  /*!
  \brief
  Checks if a collision occured between two objects

  \param first
  First object

  \param second
  Second object

  \return
  If there was a collision

  */
  /****************************************************************************/
  bool ColliderHandler::CheckCollision(GameInstance & first, GameInstance & second)
  {
    //float rot1 = rectangle1.getParent().RequestData<float>("Rotation");
    glm::vec2 rec1 = first.RequestData<glm::vec2>("Position");
    float rx1 = rec1.x;
    float ry1 = rec1.y;
    float w1 = first.RequestData<float>("Width") / 2.5f;
    float h1 = first.RequestData<float>("Height") / 2.5f;

    //float rot2 = rectangle2.getParent().RequestData<float>("Rotation");
    glm::vec2 rec2 = second.RequestData<glm::vec2>("Position");
    float rx2 = rec2.x;
    float ry2 = rec2.y;
    float w2 = second.RequestData<float>("Width") / 2.5f;
    float h2 = first.RequestData<float>("Height") / 2.5f;
         

    // variables for the sides of each rectangle
    // calculating collision between rotated rectangles is impossible with this basic method
    float L1 = rx1 - w1;
    float R1 = rx1 + w1;
    float T1 = ry1 + h1;
    float B1 = ry1 - h1;
    float L2 = rx2 - w2;
    float R2 = rx2 + w2;
    float T2 = ry2 + h2;
    float B2 = ry2 - h2;
             

    if (R2 < L1 || R1 < L2 || T1 < B2 || T2 < B1) // compare points' location in reference to each other
      return false; // no collision
    else
      return true; // collision
  }
          

  /****************************************************************************/
  /*!
  \brief
  Function called when a collision starts

  \param sub
  The subscriber collider
     
  \param payload
  data of message
       

  */
  /****************************************************************************/
  static void OnCollisionStart(Collider * sub, const Packet & payload)
  {
    sub->AddCollision(payload.getData<unsigned long>());
  }
      

  /****************************************************************************/
  /*!
  \brief
  Function called when a collision ends

  \param sub
  The subscriber collider

  \param payload
  data of message

  */
  /****************************************************************************/
  static void OnCollisionEnd(Collider * sub, const Packet & payload)
  {

    sub->RemoveCollision(payload.getData<unsigned long>());
  }

  /****************************************************************************/
  /*!
  \brief
  Function called when a position is changed

  \param sub
  The subscriber collider

  \param payload
  data of message

  */
  /****************************************************************************/
  static void OnPositionChanged(Collider * sub, const Packet & payload)
  {
    sub->UpdateCollisions(true);
  }

  /****************************************************************************/
  /*!
  \brief
  Connects messaging events for colliders.

  \param base_sub
  The collider component that subscribes to messages
   

  */
  /****************************************************************************/
  void ColliderHandler::ConnectEvents(Component * base_sub)
  {
    Collider * sub = static_cast<Collider *>(base_sub);
    Messenger & objMessenger = base_sub->getParent().getMessenger();
    using namespace std::placeholders;
    

    SUBSCRIBER_ACTION hit = std::bind(OnCollisionStart, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION end = std::bind(OnCollisionEnd, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION moved = std::bind(OnPositionChanged, sub, std::placeholders::_1);

    objMessenger.Subscribe(objMessenger, "CollisionStarted", hit);
    objMessenger.Subscribe(objMessenger, "CollisionEnded", end);
    objMessenger.Subscribe(objMessenger, "PositionSet", moved);
    objMessenger.Subscribe(objMessenger, "PositionMoved", moved);

  }

  float Physics::getDepthVelocity() const { return depthVelocity_; }
  float Physics::getDepthAcceleration() const { return depthAcceleration_; }

  void Physics::setDepthAcceleration(float a) { depthAcceleration_ = a; }
  void Physics::setDepthVelocity(float v) { depthVelocity_ = v; }
}
