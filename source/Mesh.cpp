// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Mesh.h"
#include "../include/Logger.h"

using namespace Logger;

namespace Engine
{

  // Get the dot product of two vectors
  float dot(glm::vec3 first, glm::vec3 second)
  {
    return (first.x * second.x) + (first.y * second.y) + (first.z * second.z);
  }

  // Get the half-plane from three points
  glm::vec3 HalfPlane(glm::vec3 start, glm::vec3 end, glm::vec3 inner)
  {
    glm::vec3 diff = end - start;
    glm::vec3 returned;
    returned.x = diff.y;
    returned.y = -diff.x;
    returned.z = -((returned.x * start.x) + (returned.y * start.y));

    if (dot(returned, inner) > 0)
    {
      returned.x = -returned.x;
      returned.y = -returned.y;
      returned.z = -returned.z;
    }

    return returned;
  }

  // Constructor for meshes
  Mesh::Mesh(std::vector<glm::vec3> const & verts) :
    vertList_(verts), width_(0), height_(0)
  {
    if (!verts.empty())
    {
      // Max and min bounds of the X axis
      float xMin = verts[0].x;
      float xMax = xMin;

      // Max and min bounds of the Y axis
      float yMin = verts[0].y;
      float yMax = yMin;

      for (auto & point : verts)
      {
        // Check if new X max or min
        if (point.x > xMax)
          xMax = point.x;
        else if (point.x < xMin)
          xMin = point.x;

        // Check if new Y max or min
        if (point.y > yMax)
          yMax = point.y;
        else if (point.y < yMin)
          yMin = point.y;
      }

      // Get bounds of the mesh
      width_ = xMax - xMin;
      height_ = yMax - yMin;

      glm::vec3 min(xMin, yMin, 1);
      glm::vec3 max(xMax, yMax, 1);

      center_ = glm::vec3((max.x + min.x) / 2, (max.y + min.y) / 2, 1);


      // Get the object->NDC transformations

      GetPlanes();  // get half planes of the mesh
    }
  }

  void Mesh::GetPlanes()
  {
    halfPlanes_.clear();

    for (unsigned i = 0; i < vertList_.size(); i++)
    {
      glm::vec3 start = vertList_[i];
      glm::vec3 end = vertList_[(i + 1) % vertList_.size()];
      glm::vec3 inner = vertList_[(i + 2) % vertList_.size()];

      halfPlanes_.push_back(HalfPlane(start, end, inner));
    }
  }

  bool Mesh::PointInMesh(const glm::vec2 & point) const
  {
    return PointInMesh(glm::vec3(point.x, point.y, 1));
  }

  // Test if a point is inside of the mesh
  bool Mesh::PointInMesh(const glm::vec3 & point) const
  {
    for (auto & plane : halfPlanes_)
    {
      if (dot(plane, point) > 0)
        return false;
    }

    return true;
  }
}