// Primary Author : Philip Nygard
// 
// Co-authors:
//    Kento Murawski (Initial sprite component implementation)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/sprite.h"
#include "../include/Transform.h"
#include "../include/display.h"
#include "../include/Input.h"
#include "../include/GSM.h"
#include "../include/camera.h"
#include "DrawUtils.h"

#include <algorithm>
#include <locale>     // tolower

using namespace DrawUtils;

namespace Engine
{

  // Sprite
  
  /****************************************************************************/
  /*!
    \brief
      Default constructor for Sprite components. Takes only an owner, sets
      all other members to 0, and registers the component with the 
      SpriteHandler

    \param owner
      Owner of the component
  */
  /****************************************************************************/
  Sprite::Sprite( GameInstance * owner) : 
                        Component(owner, "Sprite")
  { 
    
  }

  Sprite::Sprite(GameInstance * owner, const ParsedObject & obj) :
	  Component(owner, "Sprite")
  {

    xOffset_  = obj.getComponentProperty<float>("Sprite", "x");
    yOffset_ = obj.getComponentProperty<float>("Sprite", "y");
    GLubyte r = static_cast<unsigned char>(obj.getComponentProperty<unsigned>("Sprite", "r"));
    GLubyte g = static_cast<unsigned char>(obj.getComponentProperty<unsigned>("Sprite", "g"));
    GLubyte b = static_cast<unsigned char>(obj.getComponentProperty<unsigned>("Sprite", "b"));
    GLubyte a = static_cast<unsigned char>(obj.getComponentProperty<unsigned>("Sprite", "a"));
    float init_depth = obj.getComponentProperty<float>("Sprite", "z");

    std::string texture = obj.getComponentProperty<std::string>("Sprite", "TextureFile");

    std::string typeString = obj.getComponentProperty<std::string>("Sprite", "elementType" );
    std::transform(typeString.begin(), typeString.end(), typeString.begin(), ::tolower);

    std::string meshString = obj.getComponentProperty<std::string>("Sprite", "meshName");

    if (meshString == "")
      meshString = "Square";

    unsigned type;

    if (typeString == "background")
      type = RL_BACKGROUND;
    else if (typeString == "menu")
      type = RL_MENU;
    else if (typeString == "hud")
      type = RL_HUD;
    else if (typeString == "tile")
      type = RL_TILE;
    else
      type = RL_WORLD;

    item_ = GSM::get().getRenderer().newElement(type, meshString, texture);
    item_.setShade(glm::vec4{ r, g, b, a } / 255.f);
    item_.setDepth(init_depth);
  }

  /****************************************************************************/
  /*!
    \brief
      Constructor for Sprite components. Takes an owner, x, y, and optional
      rotation, and registers the component with the SpriteHandler

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
  //Sprite::Sprite(GameInstance * owner, 
  //         float x, float y, float width, float height) : 
  //         Component(owner, "Sprite")
  //{
  //  /*VBO_ = 0;
  //  Init(x, y, width, height);*/
  //}

  /****************************************************************************/
  /*!
    \brief
      Destructor for Sprite components. Deregisters the component from the
      handler if one exists. Will not throw an exception if no SpriteHandler
      exists
  */
  /****************************************************************************/
  Sprite::~Sprite()
  {
    //if(VBO_ != 0)
    //{
    //  glDeleteBuffers(1, &VBO_);
    //}
  }

  void Sprite::SetTransform()
  {
    /*Transform * trans = dynamic_cast<Transform *>(getParent().getComponent("Transform"));

    item_->transform_ = trans->getTransform();*/
    glm::vec2 pos = getParent().RequestData<glm::vec2>("Position"); /// TODO add event to get adjusted position (with height)
    float depth = getParent().RequestData<float>("TransformDepth");
    glm::vec2 offset{ xOffset_, yOffset_ };

    float width = getParent().RequestData<float>("Width");
    float height = getParent().RequestData<float>("Height");
    float rot = getParent().RequestData<float>("Rotation");

    item_.setPosition(pos + offset);
    item_.setScale(glm::vec2{ width, height });
    item_.setRotation(rot);
    item_.setDepth(depth);
  }

  //SpriteHandler

  /****************************************************************************/
  /*!
    \brief
      Constructor for the SpriteHandler.
  */
  /****************************************************************************/
  SpriteHandler::SpriteHandler(Stage * stage) : 
                ComponentHandler(stage, "Sprite", false)
  {
    // Add list of dependencies here
    dependencies_ = { "Transform" };
  }
 
  /****************************************************************************/
  /*!
    \brief
      Destructor for the SpriteHandler. Removes its place on the handler list
  */
  /****************************************************************************/
  SpriteHandler::~SpriteHandler()
  {
    
  }

  void SpriteHandler::SpriteDepthRequest(const Sprite * member, Packet & data)
  {
    dynamic_cast<Message<float> &>(data).data = member->getItem().getDepth();
  }

  void SpriteHandler::SpriteDepthChanged(Sprite * member, const Packet & data)
  {
    float depth = dynamic_cast<const Message<float> &>(data).data;
    
    member->getItem().setDepth(depth);
  }

  void SpriteHandler::SpriteDepthSet(Sprite * member, const Packet & data)
  {
    float depth = dynamic_cast<const Message<float> &>(data).data;

    member->getItem().setDepth(depth);
  }

  void SpriteHandler::ConnectEvents(Component * base_sub)
  {
    Sprite * sub = dynamic_cast<Sprite *>(base_sub);

    using namespace std::placeholders;

    Messenger & objMessenger = sub->getParent().getMessenger();

    //SUBSCRIBER_ACTION onDepthChanged = std::bind(&SpriteHandler::SpriteDepthChanged, sub, std::placeholders::_1);
    //SUBSCRIBER_ACTION onDepthSet = std::bind(&SpriteHandler::SpriteDepthSet, sub, std::placeholders::_1);

    REQUEST_ACTION depthRequest = std::bind(&SpriteHandler::SpriteDepthRequest, sub, std::placeholders::_1);

    ///TODO: Check that this works
    REQUEST_ACTION graphicRequest = std::bind(&GraphicRequest<Sprite>, sub, std::placeholders::_1);
    REQUEST_ACTION getMatrix = std::bind(&DrawUtils::ElementMatrixRequest<Sprite>, sub, std::placeholders::_1);
    REQUEST_ACTION orderRequest = std::bind(&DrawUtils::ElementOrderRequest<Sprite>, sub, std::placeholders::_1);

    ///// TODO add this back
    //REQUEST_ACTION getType = std::bind(&ItemTypeRequest<Sprite>, sub, std::placeholders::_1);
    REQUEST_ACTION getLayer = std::bind(&DrawUtils::ElementLayerRequest<Sprite>, sub, std::placeholders::_1);

    //objMessenger.Subscribe(objMessenger, "SpriteDepthChanged", onDepthChanged);
    //objMessenger.Subscribe(objMessenger, "SpriteDepthSet", onDepthSet);


    objMessenger.SetupRequest("Depth", depthRequest);
    objMessenger.SetupRequest("Graphic", graphicRequest);
    objMessenger.SetupRequest("DrawLayer", getLayer);

    objMessenger.SetupRequest("DrawOrder", orderRequest);
    objMessenger.SetupRequest("Matrix", getMatrix);


    ScriptRouter & router = getStage()->getScriptEventRouter();

    std::vector<std::shared_ptr<ScriptEvent>> events;

    events.push_back(router.newEvent<unsigned>(objMessenger, "TextureFrameSet"));
    events.push_back(router.newEvent<glm::vec4>(objMessenger, "ColorSet"));
    events.push_back(router.newEvent<float>(objMessenger, "RotationSet"));
    events.push_back(router.newEvent<float>(objMessenger, "SpriteDepthSet"));
    events.push_back(router.newEvent<float>(objMessenger, "SpriteDepthChanged"));

    sub->getParent().registerScriptEvent(events);

  }

  //void SpriteHandler::SpriteDrawOrderRequest(const Sprite * obj, Packet & data)
  //{
  //  static_cast<Message<unsigned> &>(data).data = obj->getItem()->GetOrder();
  //}

  //void SpriteHandler::SpriteMatrixRequest(const Sprite * item, Packet & data)
  //{
  //  static_cast<Message<glm::mat4> &>(data).data = item->GetMatrix();
  //}

  void SpriteHandler::update()
  {
    {
      for (unsigned int i = 0; i < componentList_.size(); ++i)
      {
        if (componentList_[i]->getComponentType() == "Sprite")
        {
          static_cast<Sprite*>(componentList_[i])->SetTransform();
        }
      }
    }
  }

  void SpriteHandler::getLuaRegisters()
  {
    DrawUtils::GetDrawTokenLuaRegisters<Sprite>(getStage(), "Sprite");
  }
}
