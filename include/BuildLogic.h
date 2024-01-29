// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "glm\glm\vec3.hpp"
#include "GameInstance.h"
#include "Messages.h"

using namespace Engine;

namespace Logic
{

  class ClickedFace
  {
  public:
    enum Faces { TOP, BOTTOM, LEFT, RIGHT, FRONT, BACK, NONE };

    static const std::vector<glm::vec3> & GetFace(Faces face);

  private:
    static std::unordered_map<ClickedFace::Faces, std::vector<glm::vec3>> IsoFaces;
  };

  ClickedFace::Faces GetClickedFace(glm::vec2 mousePos);

  void OnBlockClicked(GameInstance * block, const Packet & payload);

  typedef std::unordered_map<ClickedFace::Faces, std::vector<glm::vec3>> PLANE_LIST;
  typedef std::unordered_map<ClickedFace::Faces, std::vector<unsigned>> FACE_LIST;


  struct IsoMesh
  {
    IsoMesh();

    ClickedFace::Faces GetIntersectFace(const glm::vec3 & point) const;
  
  private:

    bool CheckIntersect(std::vector<glm::vec3> const & face, glm::vec3 const & point) const;
    std::vector<glm::vec3> verts;
    FACE_LIST faces;
    PLANE_LIST planes;
  };
}
