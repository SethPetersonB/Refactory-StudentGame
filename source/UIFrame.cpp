// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/UIFrame.h"
#include "../include/Texture.h"
#include "../include/GSM.h"
#include "../include/Input.h"
#include "../include/Logger.h"
#include "DrawUtils.h"

using namespace Logger;
//using namespace DrawUtils;

namespace Engine
{

  // Event fire when the screen size changes
  static void OnScreenSizeChanged(unsigned long id, Stage * stage, const Packet & data)
  {
    /*try
    {
      UIFrame * sub = static_cast<UIFrame *>(stage->getInstanceFromID(id).getComponent("UIFrame"));
      if (sub->GetScaleReference() != ScaleReference::XY)
      {
        glm::vec2 size = static_cast<const Message<glm::vec2> &>(data).data;

        if (size.x >= size.y)
          sub->SetScaleReference(ScaleReference::YY);
        else
          sub->SetScaleReference(ScaleReference::XX);
      }
    }
    catch(const std::out_of_range &)
    { }*/
  }


  // Basic UIFrame constructor
  UIFrame::UIFrame(GameInstance * owner) : Component(owner, "UIFrame"), item_{ GSM::get().getRenderer().newElement(DrawUtils::RL_MENU, "Square") }
  {
    item_.setScaleReference(ScaleReference::YY);
  }

  // Initializer constructor for UIFrames component
  UIFrame::UIFrame(GameInstance * owner, const ParsedObject & obj) : Component(owner, "UIFrame")
  {
    using namespace DrawUtils;

    std::string textureID = obj.getComponentProperty<std::string>("UIFrame", "TextureFile");

    float posX = obj.getComponentProperty<float>("UIFrame", "x");
    float posY = obj.getComponentProperty<float>("UIFrame", "y");
    float width = obj.getComponentProperty<float>("UIFrame", "width");
    float height = obj.getComponentProperty<float>("UIFrame", "height");
    float z = obj.getComponentProperty<float>("UIFrame", "z");
    float frame = obj.getComponentProperty<unsigned>("UIFrame", "frame");

    unsigned r = obj.getComponentProperty<unsigned>("UIFrame", "r");
    unsigned g = obj.getComponentProperty<unsigned>("UIFrame", "g");
    unsigned b = obj.getComponentProperty<unsigned>("UIFrame", "b");
    unsigned a = obj.getComponentProperty<unsigned>("UIFrame", "a");

    std::string mesh = obj.getComponentProperty<std::string>("UIFrame", "meshName");

    if (mesh == "")
      mesh = "Square";

    //r = (r < 255) ? r : 255;
    //g = (g < 255) ? g : 255;
    //b = (b < 255) ? b : 255;
    //a = (a < 255) ? a : 255;

    std::string ref = obj.getComponentProperty<std::string>("UIFrame", "scaleReference");

    std::string typeString = obj.getComponentProperty<std::string>("UIFrame", "elementType");
    std::transform(typeString.begin(), typeString.end(), typeString.begin(), ::tolower);

    BaseLayers type;

    if (typeString == "background")
      type = RL_BACKGROUND;
    else if (typeString == "menu")
      type = RL_MENU;
    else if (typeString == "hud")
      type = RL_HUD;
    else
      type = RL_WORLD;

    item_ = GSM::get().getRenderer().newElement(type, mesh, textureID);
    item_.setShade(glm::vec4{ r, g, b, a } /255.f);
    item_.setScale(glm::vec2{ width, height });
    item_.setPosition(glm::vec2{ posX, posY });
    item_.setDepth(z);
    item_.setFrame(frame);

    if (ref == "XX")
      item_.setScaleReference(ScaleReference::XX);
    else if (ref == "YY")
      item_.setScaleReference(ScaleReference::YY);
    else 
      item_.setScaleReference(ScaleReference::XY);
  }


  // Constructor for UIFrame handler
  UIFrameHandler::UIFrameHandler(Stage * stage) : ComponentHandler(stage, "UIFrame")
  {}

  // Function fired when a stage pauses
  void UIFrameHandler::StagePaused(unsigned long id, Stage * stage, const Packet & data)
  {
    try
    {
      bool visible = dynamic_cast<const Message<bool> &>(data).data;

      UIFrame * member = dynamic_cast<UIFrame *>(stage->getInstanceFromID(id).getComponent("UIFrame"));
      member->SetVisible(visible);
    }
    catch (...) {}
  }


  //// Request for UIFrame transformationm matrix
  //void UIFrameHandler::UIFrameMatrixRequest(const UIFrame * item, Packet & data)
  //{
  //  static_cast<Message<glm::mat4> &>(data).data = item->GetMatrix();
  //}

  //// Not used
  //void DrawOrderRequest(const UIFrame * item, Packet & data)
  //{
  //  static_cast<Message<unsigned> &>(data).data = item.getOrder();
  //}

  // Not used
  static void OnPosSet(UIFrame * frame, const Packet & payload)
  {
    glm::vec3 pos = payload.getData<glm::vec3>();

    frame->getItem().setPosition(glm::vec2{ pos.x, pos.y });
  }


  // UIFrame event connector
  void UIFrameHandler::ConnectEvents(Component * base_sub)
  {
    UIFrame * sub = dynamic_cast<UIFrame *>(base_sub);

    using namespace std::placeholders;
    
    Messenger & objMessenger = sub->getParent().getMessenger();

    SUBSCRIBER_ACTION onPaused = std::bind(&UIFrameHandler::StagePaused, sub->getParent().getId(), getStage(), std::placeholders::_1);
    SUBSCRIBER_ACTION onScreenChange = std::bind(OnScreenSizeChanged, sub->getParent().getId(), sub->getParent().getStage(), std::placeholders::_1);
    SUBSCRIBER_ACTION onPosSet = std::bind(OnPosSet, sub, std::placeholders::_1);

    REQUEST_ACTION getMatrix = std::bind(&DrawUtils::ElementMatrixRequest<UIFrame>, sub, std::placeholders::_1);
    REQUEST_ACTION getDrawOrder = std::bind(&DrawUtils::ElementOrderRequest<UIFrame>, sub, std::placeholders::_1);
    //REQUEST_ACTION getType = std::bind(&ItemTypeRequest<UIFrame>, sub, std::placeholders::_1);
    REQUEST_ACTION graphicRequest = std::bind(&DrawUtils::GraphicRequest<UIFrame>, sub, std::placeholders::_1);
    REQUEST_ACTION getFrame = std::bind(&DrawUtils::TextureFrameRequest<UIFrame>, sub, std::placeholders::_1);
    REQUEST_ACTION getLayer = std::bind(&DrawUtils::ElementLayerRequest<UIFrame>, sub, std::placeholders::_1);

    objMessenger.Subscribe(sub->getParent().getStage()->getMessenger(), "STAGE_PAUSED", onPaused);
    objMessenger.Subscribe(GSM::get().getMessenger(), "SCREEN_SIZE_CHANGED", onScreenChange);
    objMessenger.Subscribe(objMessenger, "PositionSet", onPosSet);

    objMessenger.SetupRequest("Graphic", graphicRequest);
    objMessenger.SetupRequest("Matrix", getMatrix);
    objMessenger.SetupRequest("DrawOrder", getDrawOrder);
    objMessenger.SetupRequest("TextureFrame", getFrame);
    objMessenger.SetupRequest("DrawLayer", getLayer);

    ScriptRouter & router = getStage()->getScriptEventRouter();

    std::vector<std::shared_ptr<ScriptEvent>> events;

    events.push_back(router.newEvent<unsigned>(objMessenger, "TextureFrameSet"));
    events.push_back(router.newEvent<glm::vec4>(objMessenger, "ColorSet"));
    events.push_back(router.newEvent<float>(objMessenger, "RotationSet"));
    events.push_back(router.newEvent<float>(objMessenger, "SpriteDepthSet"));
    events.push_back(router.newEvent<float>(objMessenger, "SpriteDepthChanged"));
    sub->getParent().registerScriptEvent(events);
  }

  void UIFrameHandler::update()
  {
  }

  void UIFrameHandler::getLuaRegisters()
  {
    DrawUtils::GetDrawTokenLuaRegisters<UIFrame>(getStage(), "UIFrame");
  }
}
