// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Transform.h"
#include <algorithm>
#include "../include/Input.h"
#include "../include/GSM.h"
#include "../include/Physics.h"

namespace Engine
{

  // Event functions
  void TransformHandler::TransformRotated(Transform * member, const Packet & payload)
  {
    member->addRot(dynamic_cast<const Message<float> &>(payload).data);
  }

  void TransformHandler::TransformRotationSet(Transform * member, const Packet & payload)
  {
    member->setRot(dynamic_cast<const Message<float> &>(payload).data);
  }

  void TransformHandler::TransformPositionRequest(const Transform * member, Packet & data)
  {
    dynamic_cast<Message<glm::vec2> &>(data).data = member->getPos();
  }

  void TransformHandler::TransformRotationRequest(const Transform * member, Packet & data)
  {
    dynamic_cast<Message<float> &>(data).data = member->getRot();
  }

  void TransformHandler::TransformWidthRequest(const Transform * member, Packet & data)
  {
    dynamic_cast<Message<float> &>(data).data = member->getWidth();
  }

  void TransformHandler::TransformHeightRequest(const Transform * member, Packet & data)
  {
    dynamic_cast<Message<float> &>(data).data = member->getHeight();
  }

  void TransformHandler::TransformScaleSet(Transform * member, const Packet & data)
  {
    glm::vec2 scale = dynamic_cast<const Message<glm::vec2> &>(data).data;
    member->setHeight(scale.y);
    member->setWidth(scale.x);
  }

  // Transform
  
  /****************************************************************************/
  /*!
    \brief
      Default constructor for Transform components. Takes only an owner, sets
      all other members to 0, and registers the component with the 
      TransformHandler

    \param owner
      Owner of the component
  */
  /****************************************************************************/
  Transform::Transform( GameInstance * owner) : 
                        Component(owner, "Transform")
  {
    x_ = 0;
    y_ = 0;
    width_ = 1;
    height_ = 1;
    rot_ = 0;
     
  }

  Transform::Transform( GameInstance * owner, const ParsedObject & obj) : 
                        Component(owner, "Transform")
  {
    // Initialize from object here 
    x_ = obj.getComponentProperty<float>("Transform", "x");
    y_ = obj.getComponentProperty<float>("Transform", "y");
    depth_ = obj.getComponentProperty<float>("Transform", "depth");

    float prop = obj.getComponentProperty<float>("Transform", "width");
    width_ = prop > 20 ? prop : 1;

    prop = obj.getComponentProperty<float>("Transform", "height");
    height_ = prop > 0 ? prop : 1;

    rot_ = obj.getComponentProperty<float>("Transform", "rotation");

    model_ = getTransform();
  }

  /****************************************************************************/
  /*!
    \brief
      Constructor for Transform components. Takes an owner, x, y, and optional
      rotation, and registers the component with the TransformHandler

    \param owner
      Owner of the component

    \param x
      Value to initialize the x_ member to

    \param y
      Value to initialize the y_ member to
    
    \param rot
      Value to initialize the rot_ member to. Defaults to 0
  */
  /****************************************************************************/
  Transform::Transform( GameInstance * owner, 
                        float x, 
                        float y, 
                        float width,
                        float height,
                        float rot) : Component(owner, "Transform"), x_(x), y_(y), width_(width), height_(height), rot_(rot)
  {

  }

  /****************************************************************************/
  /*!
    \brief
      Destructor for Transform components. Deregisters the component from the
      handler if one exists. Will not throw an exception if no TransformHandler
      exists
  */
  /****************************************************************************/
  Transform::~Transform()
  {
  }

  glm::mat4x4 Transform::getTransform()
  {
    return getTranslate() * getRotation() * getScale();
  }

  glm::mat4x4 Transform::getScale() const
  {
    // Need to add scale component to transform class
    return glm::scale(glm::vec3(width_, height_, 1.0));
  }

  glm::mat4x4 Transform::getRotation() const  
  {
    return glm::rotate(rot_, glm::vec3(0.0, 0.0, 1.0));
  }

  glm::mat4x4 Transform::getTranslate() const
  {
    return glm::translate(glm::vec3(x_, y_, 0.0));
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the position of the object's transform component 
    
    \return
      vec2 of the component's position 
  */
  /****************************************************************************/
  glm::vec2 Transform::getPos() const
  {
    return glm::vec2(x_, y_);
  }

  float Transform::getDepth() const
  {
    return depth_;
  }

  /****************************************************************************/
  /*!
    \brief
      Gets the rotation of the object's transform component 
    
    \return
      Rotation of the object in radians
  */
  /****************************************************************************/
  float Transform::getRot() const
  {
    return rot_;
  }

  /****************************************************************************/
  /*!
    \brief
      Sets the component's position based on the given vector 

    \param pos 
      vec2 to set the object's position to
  */
  /****************************************************************************/
  void Transform::setPos(const glm::vec2 & pos, bool)
  {
    setX(pos.x);
    setY(pos.y);

    //if (dispatchEvent)
     // getParent().PostMessage("PositionSet", pos);
  }

  /****************************************************************************/
  /*!
    \brief
      Sets the component's position based on the given X and Y coordinates

    \param x 
      Value to set the X position to 
    
    \param y 
      Value to set the Y position to 
  */
  /****************************************************************************/
  void Transform::setPos(float x, float y)
  {
    setPos(glm::vec2(x, y));
  }

  void Transform::OnMoved(const Packet &)
  {
    //glm::vec2 delta = dynamic_cast<const Message<glm::vec2> *>(&payload)->data;
    //addPos(delta);
  }

  void Transform::OnPositionSet(const Packet & )
  {
    //glm::vec2 pos = dynamic_cast<const Message<glm::vec2> &>(payload).data;

    //setPos(pos, false);
  }

  /****************************************************************************/
  /*!
    \brief
      Sets the component's X coordinate to the given value 

    \param x 
      Position to set the X coordinate to 
  */
  /****************************************************************************/
  void Transform::setX(float x)
  {
    x_ = x;
  }

  /****************************************************************************/
  /*!
    \brief
      Sets the component's Y coordinate to the given value 

    \param x 
      Position to set the Y coordinate to 
  */
  /****************************************************************************/
  void Transform::setY(float y)
  {
    y_ = y;
  }

  /****************************************************************************/
  /*!
    \brief
      Sets the component's rotation to the given value 

    \param x 
      Rotation in radians 
  */
  /****************************************************************************/
  void Transform::setRot(float rot)
  {
    rot_ = rot;
  }

  /****************************************************************************/
  /*!
    \brief
      Adds the given vector to the component's position

    \param rhs
      vec2 to add to the component 

    \return 
      Vector of the component's new position
  */
  /****************************************************************************/
  glm::vec2 Transform::addPos(const glm::vec2 & rhs)
  {
    return addPos(rhs.x, rhs.y);
  }

  /****************************************************************************/
  /*!
    \brief
      Adds the values to the component's position

    \param x 
      Amount to add to the X coordinate

    \param y 
      Amount to add to the Y coordinate

    \return 
      Vector of the component's new position
  */
  /****************************************************************************/
  glm::vec2 Transform::addPos(float x, float y)
  {
    addX(x);
    addY(y);

    return glm::vec2(x_, y_);
  }

  /****************************************************************************/
  /*!
    \brief
      Adds the given value to the component's X coordinate

    \param x
      Amount to add the the X component

    \return 
      Vector of the component's new position
  */
  /****************************************************************************/
  glm::vec2 Transform::addX(float x)
  {
    setX(x_ + x);

    return glm::vec2(x_, y_);
  }

  /****************************************************************************/
  /*!
    \brief
      Adds the given value to the component's Y coordinate

    \param x
      Amount to add the the Y component

    \return 
      Vector of the component's new position
  */
  /****************************************************************************/
  glm::vec2 Transform::addY(float y)
  {
    setY(y_ + y);

    return glm::vec2(x_, y_);
  }

  //TransformHandler

  /****************************************************************************/
  /*!
    \brief
      Constructor for the TransformHandler.
  */
  /****************************************************************************/
  TransformHandler::TransformHandler( Stage * stage) : 
                                      ComponentHandler(stage, "Transform")
  {

    // Add list of dependencies here
    dependencies_ = {};

  }

  /****************************************************************************/
  /*!
    \brief
      Destructor for the TransformHandler. Removes its place on the handler list
  */
  /****************************************************************************/
  TransformHandler::~TransformHandler()
  {
    //handlers_.erase("Transform"); // Remove Transform from the list of handlers

  }

  void Transform::OnZSet(const Packet& payload)
  {
    float z = dynamic_cast<const Message<float> *>(&payload)->data;
    setDepth(z);
  }

  /****************************************************************************/
  /*!
    \brief
      Event connection example for Transform component. Sets up four event
      listeners (Position moved, position set, rotated, rotation set) and two 
      request functions (request for rotation and position)

    \param sub
      Subscriber that will have events connected for it
  */
  /****************************************************************************/
  void TransformHandler::ConnectEvents(Component * base_sub)
  {
    Transform * sub = dynamic_cast<Transform *>(base_sub);

    using namespace std::placeholders;

    Messenger & objMessenger = sub->getParent().getMessenger();

    //SUBSCRIBER_ACTION onMoved = std::bind(&Transform::OnMoved, sub, std::placeholders::_1);
    //SUBSCRIBER_ACTION onPosSet = std::bind(&Transform::OnPositionSet, sub, std::placeholders::_1);
    //SUBSCRIBER_ACTION onRotated = std::bind(&TransformHandler::TransformRotated, sub, std::placeholders::_1);
    //SUBSCRIBER_ACTION onRotSet = std::bind(&TransformHandler::TransformRotationSet, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onScaleSet = std::bind(&TransformHandler::TransformScaleSet, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION onZSet = [sub](const Packet & payload) {sub->setDepth(payload.getData<float>()); };

    REQUEST_ACTION posRequest = std::bind(&TransformHandler::TransformPositionRequest, sub, std::placeholders::_1);
    REQUEST_ACTION rotRequest = std::bind(&TransformHandler::TransformRotationRequest, sub, std::placeholders::_1);
    REQUEST_ACTION widthRequest = std::bind(&TransformHandler::TransformWidthRequest, sub, std::placeholders::_1);
    REQUEST_ACTION heightRequest = std::bind(&TransformHandler::TransformHeightRequest, sub, std::placeholders::_1);
    REQUEST_ACTION depthRequest = [sub](Packet & data){ data.setData<float>(sub->getDepth()); };

    //objMessenger.Subscribe(objMessenger, "PositionMoved", onMoved);
    //objMessenger.Subscribe(objMessenger, "PositionSet", onPosSet);
    //objMessenger.Subscribe(objMessenger, "Rotated", onRotated);
    //objMessenger.Subscribe(objMessenger, "RotationSet", onRotSet);
    objMessenger.Subscribe(objMessenger, "ScaleSet", onScaleSet);
    //objMessenger.Subscribe(objMessenger, "TransfomrDepthSet", onZSet);

    objMessenger.SetupRequest("Position", posRequest);
    objMessenger.SetupRequest("Rotation", rotRequest);
    objMessenger.SetupRequest("TransformDepth", depthRequest);
    objMessenger.SetupRequest("Width", widthRequest);
    objMessenger.SetupRequest("Height", heightRequest);

    ScriptRouter & router = getStage()->getScriptEventRouter();

    std::vector<std::shared_ptr<ScriptEvent>> events;

    events.push_back(router.newEvent<glm::vec2>(objMessenger, "PositionSet"));
    events.push_back(router.newEvent<glm::vec2>(objMessenger, "PositionMoved"));

    events.push_back(router.newEvent<float>(objMessenger, "RotationSet"));
    events.push_back(router.newEvent<float>(objMessenger, "Rotated"));
    events.push_back(router.newEvent<float>(objMessenger, "TransformDepthSet"));
    sub->getParent().registerScriptEvent(events);
  }

  void TransformHandler::update()
  {
  }

  static void lua_set_scale(Transform& comp, const glm::vec2& scale)
  {
    comp.setWidth(scale.x);
    comp.setHeight(scale.y);
  }

  static glm::vec2 lua_get_scale(const Transform& comp)
  {
    return glm::vec2(comp.getWidth(), comp.getHeight());
  }
  // Necessary as setPos has a default parameter, which luabind cannot handle
  void setPos_wrap(Transform * comp, const glm::vec2 & pos)
  {
    comp->setPos(pos);
  }

  void TransformHandler::getLuaRegisters()
  {
    using namespace luabind;

    getStage()->registerLuaModule(
      class_<Transform, Component>("Transform")
      .property("position", &Transform::getPos, &setPos_wrap)
      .property("rotation", &Transform::getRot, &Transform::setRot)
      .property("depth", &Transform::getDepth, &Transform::setDepth)
      .property("scale", &lua_get_scale, &lua_set_scale)
    );
  }
}
 
