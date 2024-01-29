// Primary Author : Philip Nygard
//
// � Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright � Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "glm/glm/gtx/transform.hpp"
#include "DrawElement.h"

/**   
* \brief  Updates this object's matrix
*/
void DrawElement::update() const
{

  trans_ = glm::translate(glm::vec3{ position.x, position.y, 0 });
  rot_ = glm::rotate(rotation, glm::vec3{ 0, 0, 1 });
  scale_ = glm::scale(glm::vec3{ scale.x, scale.y, 1 });

  needsUpdate_ = false;
}

/**
* \brief  Determines if the object's matrix needs to be updated
*
* \return True if the object needs to be updated
*/
bool DrawElement::doesNeedUpdate() const
{
  return needsUpdate_;
}

/**
* \brief  Gets the matrix.
*
* \return The matrix.
*/
const glm::mat4 DrawElement::getMatrix(float ar) const
{
  glm::mat4 & scaling = scale_;

  switch (ref)
  {
  case ScaleReference::XX:
    scaling *= glm::scale(glm::vec3{ 1, ar, 1 });
    break;

  case ScaleReference::YY:
    scaling *= glm::scale(glm::vec3{ 1 / ar, 1, 1 });
    break;

  default:
    break;
  }
  return trans_ * rot_ * scaling;
}

DrawElement::DrawElement(DrawGroup * parent,
  const glm::vec2 & _pos, const glm::vec2 & _scale, float _rot, 
  const RMesh * _mesh, const DrawSurface * _surface, 
  const glm::vec4 & _shade) :
  parent_(parent), shade(_shade), position(_pos), scale(_scale), rotation(_rot), 
  frame{ 0 }, depth{ 0 },
  surface(_surface), mesh(_mesh),
  visible{true},
  needsUpdate_{ true }
{}