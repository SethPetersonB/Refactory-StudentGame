// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <vector>

#include "glm/glm/vec3.hpp"
#include "glm/glm/mat4x4.hpp"


namespace Engine
{
  float dot(glm::vec3 first, glm::vec3 second);

  glm::vec3 HalfPlane(glm::vec3 start, glm::vec3 end, glm::vec3 inner);


  class Mesh
  {
  public:
    Mesh(const std::vector<glm::vec3> & verts);
    virtual ~Mesh() {};

    bool PointInMesh(const glm::vec2 &) const;
    bool PointInMesh(const glm::vec3 &) const;

    float Width() const { return width_; }
    float Height() const { return height_; }
    const glm::vec3 & Center() const { return center_; }

  protected:
    void GetPlanes();

  private:

    std::vector<glm::vec3> vertList_;
    float width_;
    float height_;
    glm::vec3 center_;

    std::vector<glm::vec3> halfPlanes_;
  };

}
