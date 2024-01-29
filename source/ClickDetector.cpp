// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#include "../include/ClickDetector.h"
#include "../include/Input.h"
#include "../include/camera.h"
#include "../include/GSM.h"
#include "../include/Logger.h"
#include "../include/MenuButtons.h"
#include "DrawSystem.h"
#include "DrawUtils.h"
#include "Draw_fwd.h"
#include "RMesh.h"

using namespace Logger;
using namespace DrawUtils;

namespace Engine
{

  ClickDetector::ClickDetector(GameInstance * owner) :
    Component(owner, "ClickDetector"), meshId("Square")
  {}

  ClickDetector::ClickDetector(GameInstance * owner, const ParsedObject & obj) :
    Component(owner, "ClickDetector")
  {
    meshId = obj.getComponentProperty<std::string>("ClickDetector", "MeshType");
  }

  ClickDetectorHandler::ClickDetectorHandler(Stage * stage) :
    ComponentHandler(stage, "ClickDetector"), lastMousedOver_(0)
  {
    dependencies_ = { "Sprite" };
  }

  /*
  Mouse to screen (Camera->camera to world?)
  World to object (inverse transform matrix)


  Somewhat hacky way of doing this. Assumes all objects use the same square mesh and probably inefficient
  */
  void ClickDetectorHandler::update()
  {

    // Check for mouse exiting last object
    unsigned long frontClicked = 0; // Current front object
    unsigned frontOrder = 0;                    // Draw order of the front object
    glm::vec2 clickPos;

    Camera & cam = GSM::get().getCamera();
    DrawSystem & disp = GSM::get().getRenderer();

    int screenWidth = disp.getViewWidth();
    int screenHeight = disp.getViewHeight();

    glm::mat4 camWorld = cam.CameraToWorld();
    glm::vec2 mousePos = InputSystem::MousePosition() -glm::vec2(disp.getViewOffsetX(), disp.getViewOffsetY());
    mousePos.x = (2 * (mousePos.x / screenWidth)) - 1;
    mousePos.y = (-2 * mousePos.y / screenHeight) + 1;

    glm::vec4 mouse4(mousePos.x, mousePos.y, 0, 1);

    for (auto & component : componentList_)
    {
      glm::vec4 mouse4temp{ mouse4 };

      ClickDetector * detector = static_cast<ClickDetector *>(component);
      GameInstance & parent = component->getParent();

      DrawToken obj = parent.RequestData<DrawToken>("Graphic");
      glm::mat4 matrixFinal = obj.getFinalMatrix(1);
      glm::mat4 invFinal = glm::inverse(matrixFinal);

      DrawLayer layer = parent.RequestData<DrawLayer>("DrawLayer");
      //glm::mat4 layerTransInv = glm::inverse(disp.getDrawGroup(layer).getTransformation());

      glm::vec4 mouseObj = invFinal * mouse4temp;
      glm::vec2 mousePos(mouseObj.x, mouseObj.y);

      const RMesh * detectorMesh = disp.getMesh(detector->getMeshID());


      // Transformation matrix for mouse
      if (detectorMesh != nullptr && detectorMesh->pointInMesh(mousePos))
      {
        size_t currOrder = detector->getParent().RequestData<size_t>("DrawOrder");

        //  /*
        //    An object will be drawn first if it's draw order is higher than another.
        //    That means that if an object has a higher draw order than another, it is in front of
        //    it, and is likely what was clicked on
        //  */

        // Detect inside mesh

        if (frontClicked == 0 || currOrder >= frontOrder)
        {
          frontOrder = currOrder;
          frontClicked = detector->getParent().getId();
          clickPos = glm::vec2(mouseObj.x, mouseObj.y);
        }

      }

    }

    if (frontClicked && InputSystem::Mouse1Clicked())
      try
    {
      getStage()->getInstanceFromID(frontClicked).PostMessage("Clicked", Message<glm::vec2>(clickPos));
    }
    catch(const std::out_of_range &){}

    if (frontClicked && InputSystem::Mouse2Clicked())
      try
    {
      getStage()->getInstanceFromID(frontClicked).PostMessage("RightClicked", Message<glm::vec2>(clickPos));
    }
    catch (const std::out_of_range &) {}

    if (frontClicked != lastMousedOver_ && lastMousedOver_ != 0)
    {
      try
      {
        getStage()->getInstanceFromID(lastMousedOver_).PostMessage("MouseExit", Message<glm::vec2>(clickPos));
      }
      catch(const std::out_of_range &){}

      lastMousedOver_ = 0;

    }

    if (frontClicked && frontClicked != lastMousedOver_)
    {
      lastMousedOver_ = frontClicked;
      try
      {
        getStage()->getInstanceFromID(frontClicked).PostMessage("MouseEntered", Message<glm::vec2>(clickPos));
      }
      catch(const std::out_of_range &){}
    }
  }

  void ClickDetectorHandler::ConnectEvents(Component * base_sub)
  {
    Messenger & objMessenger = base_sub->getParent().getMessenger();

    ScriptRouter & router = getStage()->getScriptEventRouter();

    std::vector<std::shared_ptr<ScriptEvent>> events;

    events.push_back(router.newEvent<glm::vec2>(objMessenger, "Clicked"));

    events.push_back(router.newEvent<glm::vec2>(objMessenger, "MouseEntered"));
    events.push_back(router.newEvent<glm::vec2>(objMessenger, "MouseExit"));

    base_sub->getParent().registerScriptEvent(events);
  }
}
