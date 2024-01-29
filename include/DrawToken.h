// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "glm/glm/vec2.hpp"
#include "glm/glm/vec4.hpp"
#include "glm/glm/mat4x4.hpp"
#include "Draw_fwd.h"

class DrawToken
{
public:
  DrawToken();

  DrawToken(const DrawToken & cpy);

  virtual ~DrawToken();

  DrawToken & operator=(const DrawToken & rhs);
  bool operator==(const DrawToken & rhs) const;

  // Getters
  ScaleReference getScaleReference() const;
  bool isVisible() const;

  glm::mat4 getMatrix(float ar) const;
  glm::mat4 getFinalMatrix(float ar) const;

  glm::vec4 getShade() const;

  glm::vec2 getPosition() const;
  glm::vec2 getFinalPosition(float ar = 16.f / 9.f) const;
  glm::vec2 getScale() const;

  size_t getDrawOrder() const;
  unsigned getFrame() const;
  DrawLayer getLayer() const;

  float getRotation() const;
  float getDepth() const;
  float getIsoY() const;

  const RMesh * getMesh() const;
  const DrawSurface * getDrawSurface() const;

  // Setters
 
  void scale(const glm::vec2 & amount);
  void scale(float amount);
  void rotate(float amount);
  void translate(const glm::vec2 & amount);

  void setScaleReference(ScaleReference ref);
  void setVisible(bool visible);
  void setShade(const glm::vec4 & shade);
  void setPosition(const glm::vec2 & pos);
  void setScale(const glm::vec2 & scale);
  void setScale(float scale);
  void setFrame(unsigned frame);
  void setRotation(float rot);
  void setDepth(float depth);
  void setMesh(const RMesh * mesh);
  void setDrawSurface(const DrawSurface * surface);

  // Only use this in MODFUNCs please. It's necessary for isometric sorting, but 
  // not useful anywhere else
  void setIsoY(float y) const;

  MODFUNC getModFunc() const;
  DrawGroup * const parent_;

  /*
  Copy Ctor _/
  Dtor      _/

  Operators:
  Assignment  _/
  Equality    _/
  
  Transformation functions (access/:
  Scale
  Rotate
  Translate

  Texture access/modification
  Mesh access/modification
  */
  
private:
  friend class DrawGroup;
  friend class DrawSystem;

  DrawToken(DrawGroup * parent, size_t id);

  void registerMe();
  void deregisterMe();


  size_t id_;

  /*
  Non-default Ctor  _/

  TokenID         _/
  Parent pointer  _/

  Register()    _/
  Deregister()  _/
  */
};