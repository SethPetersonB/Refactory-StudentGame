// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "glm/glm/vec4.hpp"
#include "glm/glm/vec3.hpp"
#include "glm/glm/vec2.hpp"

#include <vector> // vector

struct Vertex
{
  Vertex(float x, float y, 
    float u, float v);

  Vertex(float x, float y,
    float u, float v, 
    float r, float g, float b, float a = 1);

  Vertex(const glm::vec2 & pos, 
    const glm::vec2 & _uv,
    const glm::vec4 & c = { 1, 1, 1, 1 });

  Vertex(const glm::vec3 & pos, 
    const glm::vec2 & _uv, 
    const glm::vec4 & c = { 1, 1, 1, 1 });

  glm::vec3 point;
  glm::vec4 color;

  // UV coordinates for textures
  glm::vec2 uv;
};

struct Tri
{
  Tri(unsigned first, unsigned second, unsigned third);
  
  unsigned operator[](size_t index) const;
  unsigned & operator[](size_t index);

  unsigned first;
  unsigned second;
  unsigned third;
};

/**   
* \brief  A mesh for rendering. Should be merged with other mesh at some point
*/
class RMesh
{
public:
  bool pointInMesh(const glm::vec2 & point) const;

  RMesh(const std::initializer_list<Vertex> & _verts, std::initializer_list<Tri> _tris);

  std::vector<Vertex> verts;
  std::vector<Tri> tris;

protected:
  void getPlanes();

private:
  std::vector<glm::vec3> halfplanes_;

};
