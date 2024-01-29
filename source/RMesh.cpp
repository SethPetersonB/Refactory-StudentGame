// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "RMesh.h"
#include "glm/glm/vec2.hpp"

// Get the dot product of two vectors
static float dot(glm::vec3 first, glm::vec3 second)
{
  return (first.x * second.x) + (first.y * second.y) + (first.z * second.z);
}

// Get the half-plane from three points
static glm::vec3 HalfPlane(glm::vec3 start, glm::vec3 end, glm::vec3 inner)
{
  glm::vec3 diff = end - start;
  glm::vec3 returned;
  returned.x = diff.y;
  returned.y = -diff.x;
  returned.z = -(dot(returned, start));

  if (dot(returned, inner) > 0)
  {
    returned.x = -returned.x;
    returned.y = -returned.y;
    returned.z = -returned.z;
  }

  return returned;
}

bool RMesh::pointInMesh(const glm::vec2 & point) const
{
  glm::vec3 point3{ point.x, point.y, 1 };

  for (auto & plane : halfplanes_)
  {
    if (dot(plane, point3) > 0)
      return false;
  }

  return true;
}

RMesh::RMesh(const std::initializer_list<Vertex>& _verts, std::initializer_list<Tri> _tris):
  verts{ _verts }, tris{ _tris }
{
  getPlanes();  // Will break if verts are changed, which shouldn't happen
}

void RMesh::getPlanes()
{
  halfplanes_.clear();

  for (unsigned i = 0; i < verts.size(); i++)
  {
    glm::vec3 start = verts[i].point;
    glm::vec3 end = verts[(i + 1) % verts.size()].point;
    glm::vec3 inner = verts[(i + 2) % verts.size()].point;

    halfplanes_.push_back(HalfPlane(start, end, inner));
  }
}

Vertex::Vertex(float x, float y, 
  float u, float v):
  point {x ,y, 1},
  uv{ u, v },
  color{ 1, 1, 1, 1 }
{}

Vertex::Vertex(float x, float y, 
  float u, float v,
  float r, float g, float b, float a) :
  point{ x, y, 1 }, 
    uv{ u, v },
  color{ r, g, b, a }
{}

Vertex::Vertex(const glm::vec2 & pos, 
  const glm::vec2 & _uv,
  const glm::vec4 & c) :
  point{ pos.x, pos.y, 1 }, 
  uv{ _uv },
  color{ c }
{}

Vertex::Vertex(const glm::vec3 & pos,
  const glm::vec2 & _uv,
  const glm::vec4 & c):
  point{ pos },
    uv{ _uv },
    color{ c }
{}

Tri::Tri(unsigned _first, unsigned _second, unsigned _third) :
  first{ _first }, second{ _second }, third{ _third }
{}

unsigned Tri::operator[](size_t index) const
{
  return (&first)[index];
}

unsigned & Tri::operator[](size_t index)
{
  return (&first)[index];
}
