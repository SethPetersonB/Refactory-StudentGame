// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/BuildLogic.h"
#include "../include/glm/glm/vec2.hpp"
#include "../include/Logger.h"
#include "../include/Mesh.h"
#include "../include/GSM.h"

#include <map>

using namespace Logger;

namespace Logic
{

  // Mesh for top face of isometric blocks 
  const static Engine::Mesh topFace(
  {
    glm::vec3(0, 0.5, 1),
    glm::vec3(0.5, 0.25, 1),
    glm::vec3(0, 0, 1),
    glm::vec3(-0.5, 0.25, 1)
  });

  // Mesh for right face of isometric blocks
  const static Engine::Mesh rightFace(
  {
    glm::vec3(0.5, 0.25, 1),
    glm::vec3(0.5, -0.5, 1),
    glm::vec3(0, -0.5, 1),
    glm::vec3(0, 0, 1)
  });

  // Mesh for left face of isometric blocks
  const static Engine::Mesh leftFace(
  {
    glm::vec3(-0.5, 0.25, 1),
    glm::vec3(-0.5, -0.5, 1),
    glm::vec3(0, -0.5, 1),
    glm::vec3(0, 0, 1)
  });

  void OnBlockClicked(GameInstance * block, const Packet & payload)
  {
    glm::vec2 mousePos = static_cast<const Message<glm::vec2> &>(payload).data;
    glm::vec2 gridPos = block->RequestData<glm::vec2>("TilePos");
    Stage* stage = block->getStage();

    float worldRot = GSM::get().getCamera().getIsoRot();
    float pi = glm::pi<float>();

    Engine::GameInstance * tile = nullptr;

    if (topFace.PointInMesh(mousePos))
    {
      tile = &stage->getInstanceFromID(stage->GetGrid().at((int)gridPos.y).at((int)gridPos.x));

    }
    else if (rightFace.PointInMesh(mousePos) && gridPos.x < stage->GetGrid().GetGridWidth())
    {
      try
      {
        if ((worldRot >= 0 && worldRot < (pi / 4)) ||
          (worldRot >= ((7 * pi) / 4) && worldRot <= (2 * pi)))
          tile = &stage->getInstanceFromID(stage->GetGrid().at((int)gridPos.y).at((int)gridPos.x + 1));

        else if ((worldRot >= pi / 4 && worldRot < (3 * pi) / 4))
          tile = &stage->getInstanceFromID(stage->GetGrid().at((int)gridPos.y - 1).at((int)gridPos.x));

        else if (worldRot >= (3 * pi) / 4 && worldRot < (5 * pi) / 4)
          tile = &stage->getInstanceFromID(stage->GetGrid().at((int)gridPos.y).at((int)gridPos.x - 1));

        else if (worldRot >= (5 * pi) / 4 && worldRot < (7 * pi) / 4)
          tile = &stage->getInstanceFromID(stage->GetGrid().at((int)gridPos.y + 1).at((int)gridPos.x));
      }
      catch (const std::out_of_range &) {}
    }
    else if (leftFace.PointInMesh(mousePos))
    {
      try
      {
        if ((worldRot >= 0 && worldRot < (pi / 4)) ||
          (worldRot >= ((7 * pi) / 4) && worldRot <= (2 * pi)))
          tile = &stage->getInstanceFromID(stage->GetGrid().at((int)gridPos.y - 1).at((int)gridPos.x));

        else if ((worldRot >= pi / 4 && worldRot < (3 * pi) / 4))
          tile = &stage->getInstanceFromID(stage->GetGrid().at((int)gridPos.y).at((int)gridPos.x - 1));

        else if (worldRot >= (3 * pi) / 4 && worldRot < (5 * pi) / 4)
          tile = &stage->getInstanceFromID(stage->GetGrid().at((int)gridPos.y + 1).at((int)gridPos.x));

        else if (worldRot >= (5 * pi) / 4 && worldRot < (7 * pi) / 4)
          tile = &stage->getInstanceFromID(stage->GetGrid().at((int)gridPos.y).at((int)gridPos.x + 1));
      }
      catch (const std::out_of_range &) {}
    }
    else
      Log<Error>("Invalid face clicked!");


    if (tile)
      tile->PostMessage("Clicked", Engine::Message<glm::vec2>(mousePos));
  }
}