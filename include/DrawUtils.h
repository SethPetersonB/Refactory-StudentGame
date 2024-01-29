// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include "Draw_fwd.h"
#include "Messages.h"
#include "DrawToken.h"
#include "glm/glm/vec2.hpp"
#include "glm/glm/vec4.hpp"
#include "glm/glm/mat4x4.hpp"
#include "GSM.h"
#include "Stage.h"

#include "Timer.h"

#define COLORSHIFT 0.5f

using namespace Engine;

namespace DrawUtils
{
  // Extremely primitive, non-component based sprite data for loading and splash screens
  struct IMGINFO
  {
    float lifetime;

    float fadeInTime = 1.5;

    float fadeOutTime = 1.5;

    unsigned numFrames = 0;
    unsigned currFrame = 0;
    float frameTime = 0.25;
    Timer frameTimer;

    DrawToken token;
  };


  template<typename T>
  void OnMouseEntered(T * sub, const Packet &);

  template<typename T>
  void OnMouseExit(T * sub, const Packet &);

  template<typename T>
  void ElementLayerRequest(const T * sub, Packet & data);

  template<typename T>
  void ElementMatrixRequest(const T * sub, Packet & data);

  template<typename T>
  void ElementOrderRequest(const T * sub, Packet & data);

  template<typename T>
  void GraphicRequest(const T * member, Packet & data);

  template<typename T>
  void TextureFrameRequest(const T * member, Packet & data);

  template<typename T>
  void OnTextureFrameSet(T * member, const Packet & data);

  template<typename T>
  void GetDrawTokenLuaRegisters(Stage * stage, const std::string & compName);

  enum BaseLayers : DrawLayer {
    RL_BACKGROUND = 0,
    RL_TILE = 1,
    RL_WORLD = 2,
    RL_HUD = 3,
    RL_MENU = 4,
  };

  bool R_DepthComp(const DrawToken & lhs, const DrawToken & rhs);
  bool R_XComp(const DrawToken & lhs, const DrawToken & rhs);
  bool R_YComp(const DrawToken & lhs, const DrawToken & rhs);

  void R_InitLayers(DrawSystem & sys);
  void R_InitShaders(DrawSystem & sys);
  void R_LoadMeshes(DrawSystem & sys);

  std::vector<IMGINFO> R_GetPreloads(DrawSystem & sys);

  void R_LoadTextures(DrawSystem * sys, const std::string & defPath);

  glm::mat4 GetIsoMat();
  glm::mat4 IsoMatrix();

  size_t GetTokenDrawOrder(const DrawToken & item);

  std::pair<glm::mat4, bool> R_IsoTransformWorld(const DrawToken & token);
  std::pair<glm::mat4, bool> R_IsoTransformTiles( const DrawToken & token );

}
#include "../source/DrawUtils_templates.cpp"
