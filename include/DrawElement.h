// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "glm/glm/mat4x4.hpp"
#include "glm/glm/vec2.hpp"
#include "glm/glm/vec4.hpp"

#include "Draw_fwd.h"

struct DrawElement
{
public:
  DrawElement & operator=(const DrawElement &) = delete;

  void update() const;

  bool doesNeedUpdate() const;

  const glm::mat4 getMatrix(float ar) const;
protected:
  friend class DrawToken;
  friend class DrawSystem;

  ScaleReference ref;

  bool visible;

  glm::vec4 shade;

  glm::vec2 position;
  glm::vec2 scale;

  unsigned frame;
  DrawLayer layer;

  float rotation;
  float depth;
  mutable float isoY; // Don't like including this here, but saves a lot of trouble

  const DrawSurface * surface;
  const RMesh * mesh;

  /*
  Drawing Data
  */

private:
  friend class DrawGroup;

  DrawElement(DrawGroup * parent,
    const glm::vec2 & _pos, const glm::vec2 & _scale, float _rot,
    const RMesh * _mesh, const DrawSurface * _surface = nullptr, 
    const glm::vec4 & _shade = { 1, 1, 1, 1 });

  DrawGroup * parent_;

  mutable glm::mat4 scale_;
  mutable glm::mat4 rot_;
  mutable glm::mat4 trans_;

  mutable bool needsUpdate_;
  /*
  Ctor

  non-drawing data
  */
};