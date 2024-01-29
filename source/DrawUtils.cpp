// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <algorithm>

#include "RMesh.h"
#include "DrawSystem.h"
#include "DrawUtils.h"
#include "DrawToken.h"
#include "Camera.h"

#define EPSILON 10.0e-5



namespace DrawUtils
{
  glm::mat4 IsoMatrix()
  {
    glm::mat4 isoScale = glm::scale(glm::vec3(1.0f, glm::tan(glm::radians(30.0f)), 1.0f));
    glm::mat4 isoRot = glm::rotate(glm::radians(-45.0f), glm::vec3(0, 0, 1));
    //glm::mat4 isoShear;

    return isoScale * isoRot;
  }

  bool R_DepthComp(const DrawToken & lhs, const DrawToken & rhs)
  {
    return lhs.getDepth() < rhs.getDepth();
  }

  bool R_XComp(const DrawToken & lhs, const DrawToken & rhs)
  {
    float lPos = lhs.getPosition().x;
    float rPos = rhs.getPosition().x;

    return (std::abs(lPos - rPos) > EPSILON) ? lPos > rPos : R_DepthComp(lhs, rhs);
  }

  bool R_YComp(const DrawToken & lhs, const DrawToken & rhs)
  {
    auto lMod = lhs.getModFunc();
    auto rMod = rhs.getModFunc();

    float lPos = lhs.getIsoY();
    float rPos = rhs.getIsoY();

    return (std::abs(lPos - rPos) > EPSILON) ? lPos > rPos : R_DepthComp(lhs, rhs);
  }

  void R_InitLayers(DrawSystem & sys)
  {
    sys.newDrawGroup(RL_BACKGROUND);
    DrawGroup & tile = sys.newDrawGroup(RL_TILE);
    DrawGroup & world = sys.newDrawGroup(RL_WORLD, R_YComp);
    sys.newDrawGroup(RL_HUD, R_DepthComp);
    sys.newDrawGroup(RL_MENU, R_DepthComp);

    tile.setModFunc(R_IsoTransformTiles);
    world.setModFunc(R_IsoTransformWorld);
  }

  void R_InitShaders(DrawSystem & sys)
  {
    sys.loadAndUseFragmentShader("BASIC_FRAGMENT", "res/fragment.fs");
    sys.loadAndUseVertexShader("BASIC_VERTEX", "res/vertex.fs");
  }

  void R_LoadMeshes(DrawSystem & sys)
  {
    sys.loadMesh("Square", RMesh{
      {// Verts
        Vertex{ 0.5f, 0.5f,    1, 1,  1, 1, 1, 1 },
        Vertex{ -0.5f, 0.5f,   0, 1,  1, 1, 1, 1 },
        Vertex{ -0.5f, -0.5f,  0, 0,  1, 1, 1, 1 },
        Vertex{ 0.5, -0.5,     1, 0,  1, 1, 1, 1 }
      },
      { // Tris
        Tri{ 0, 1, 2 },
        Tri{ 0, 2, 3 }
      }
    });

    sys.loadMesh("Hexagon", RMesh{
      {// Verts
        Vertex{0.f,    0.5f,    0.5, 1},   // 0
        Vertex{0.5f,   0.25f,   1, 0.75},  // 1
        Vertex{0.5f,  -0.25f,   1, 0.25},  // 2
        Vertex{0.f,   -0.5f,    0.5, 0},   // 3
        Vertex{-0.5f, -0.25f,   0, 0.25},  // 4
        Vertex{-0.5f,  0.25f,   0, 0.75},  // 5
      },
      {// Tris
        Tri{0, 1, 3},
        Tri{1, 2, 3},
        Tri{3, 4, 5},
        Tri{5, 0, 3},
      }
    });
  }

  std::vector<IMGINFO> R_GetPreloads(DrawSystem & sys)
  {
    std::vector<IMGINFO> order;

    // Load preload textures
    sys.loadTexture("P_NetResultLogo", "art_assets/preload/NetResultLogo.png");
    sys.loadTexture("P_DigipenLogo", "art_assets/preload/DigiPenLogo.png");
    sys.loadTexture("P_Loading", "art_assets/preload/Loading.png");

    // Loading image
    IMGINFO loading;
    loading.token = sys.newElement(RL_MENU, "Square", "P_Loading");
    loading.token.setVisible(true);
    loading.token.setScaleReference(ScaleReference::YY);
    //loading.texture = renderer_->getTexture("Loading");
    loading.lifetime = 1.5;
    loading.fadeInTime = 0;
    loading.frameTime = 0.5;
    loading.numFrames = 4;
    
    // Set loadng size and position
    loading.token.setScale(glm::vec2{ 0.5, 0.25 });
    loading.token.setPosition(glm::vec2{ -0.75, -0.8 });

    const_cast<Texture *>(sys.getTexture("P_Loading"))->setNumFrames(4);

    // Digipen logo
    IMGINFO digipen;
    digipen.token = sys.newElement(RL_MENU, "Square", "P_DigipenLogo");
    digipen.token.setVisible(false);
    digipen.token.setScale(glm::vec2{ 2, 1 });
    digipen.token.setScaleReference(ScaleReference::YY);
    digipen.lifetime = 6.5;

    // Net result logo
    IMGINFO netResult;
    netResult.token = sys.newElement(RL_MENU, "Square", "P_NetResultLogo");
    netResult.token.setVisible(false);
    netResult.token.setScaleReference(ScaleReference::YY);
    netResult.token.setScale(glm::vec2{ 2, 1 });
    netResult.lifetime = 6;

    // Push into vector
    order.push_back(loading);
    order.push_back(digipen);
    order.push_back(netResult);

    return order;
  }

   glm::mat4 GetIsoMat()
  {
    // These are static so they don't have to be calculated every time
    static glm::mat4 isoTrans(IsoMatrix());
    glm::vec2 pos2 = GSM::get().getCamera().getPosition();
    glm::vec3 pos3{ pos2.x, pos2.y, 0 };

    glm::mat4 worldRot = glm::rotate(GSM::get().getCamera().getIsoRot(), glm::vec3(0, 0, 1));
    glm::mat4 worldTransBack = glm::translate(-pos3);
    glm::mat4 worldTrans = glm::translate(pos3);

    return  worldTrans * isoTrans * worldRot * worldTransBack ;
  }

  std::pair<glm::mat4, bool> R_IsoTransformWorld(const DrawToken & token)
  {
    static glm::mat4 isoTrans{ IsoMatrix() };
    //Width
    float width;

    /*
    Isometric sprite transformations:

    Isometric projection on base
    Determine width of sprite:
    width = magnitued(base left - base right)

    Sprite centered on base
    Height adjusted as needed
    */

    //bottom left -> top right
    glm::vec2 tScale{ token.getScale() };
    glm::vec2 tPos{ token.getPosition() };

    glm::vec4 scale(tScale.x, tScale.y, 1, 0);

    scale = isoTrans * scale;

    width = sqrt(scale.x * scale.x + scale.y * scale.y);

    // Height-width ratio of isometric cubes
    float height = width * 2 / glm::tan(glm::radians(60.0f));

    // Scale
    glm::mat4 isoScaleMat = glm::scale(glm::vec3(width, height, 1.0f));
    glm::vec4 pos = GetIsoMat() * glm::vec4(tPos.x, tPos.y, 0, 1);

    token.setIsoY(pos.y); // Yuck, breaks a lot of decoupling I did for render system, 
                          // but necessary to get sorting working quickly and easilly for isometrics

    pos = pos + glm::vec4(0, (height / 2) * token.getDepth(), 0, 0);
    glm::mat4 isoTransMat = glm::translate(glm::vec3(pos.x, pos.y + height / 4, pos.z));

    return std::make_pair(isoTransMat * isoScaleMat, true);
  }

  std::pair<glm::mat4, bool> R_IsoTransformTiles(const DrawToken & token)
  {
    glm::vec2 tScale{ token.getScale() };
    glm::vec4 scale(tScale.x, tScale.y, 1, 0);

    float width = sqrt(scale.x * scale.x + scale.y * scale.y);

    float height = width * 2 / glm::tan(glm::radians(60.0f));

    return std::make_pair(glm::translate(glm::vec3{0, (height / 2) * token.getDepth(), 0}) * GetIsoMat(), false);
  }

  size_t GetTokenDrawOrder(const DrawToken & item)
  {
    size_t before = 0;

    DrawLayer layer = item.getLayer();

    for (DrawLayer i = 0; i < layer; i++)
    {
      try
      {
        const DrawGroup & group = GSM::get().getRenderer().getDrawGroup(i);
        before += group.size();
      }
      catch (const std::out_of_range &) {}
    }

    return before + item.getDrawOrder();
  }
}
