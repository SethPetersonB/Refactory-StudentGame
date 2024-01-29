// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

#include "glm/glm/vec2.hpp"
#include "glm/glm/vec4.hpp"

#include "DrawToken.h"
#include "DrawElement.h"
#include "Draw_fwd.h"

class DrawGroup
{
public:
  DrawGroup( const COMPFUNC & sorter = nullptr);
  //~DrawGroup();

  DrawGroup(const DrawGroup &) = delete;
  DrawGroup & operator=(const DrawGroup &) = delete;


  glm::mat4 getTransformation() const;

  void setSortFunc(const COMPFUNC & func);
  void setModFunc(const MODFUNC & func);

  void setTransformation(const glm::mat4 & transfrom);

  DrawToken newElement(
    const glm::vec2 & pos, const glm::vec2 & scale, float rot,
    const RMesh * mesh, const DrawSurface * surface = nullptr,
    const glm::vec4 & _shade = { 1, 1, 1, 1 });

  DrawToken newElement(const RMesh * mesh, const DrawSurface * surface = nullptr);

  void draw(Renderer & render);

  size_t getDrawOrder(size_t id) const;
  size_t size() const;

  /*
    Ctor
    Dtor

    NewElement 
    SetSort
    Get/Set gloabl transformation
    Draw
  */
  

protected:
  friend class DrawToken;

  bool registerToken(size_t id);
  void deregisterToken(size_t id);

  const DrawElement & getElement(size_t token) const;
  DrawToken getToken(size_t id);

  DrawElement & getElement(size_t token);

  /*
    Register/Deregister token
    Get element
    Move elements
  */
private:
  friend class DrawSystem;

  void sort();
  void scrub();

  struct DrawStruct
  {
    DrawStruct(DrawGroup * parent, const glm::vec2 & pos, const glm::vec2 & scale, float rot, const RMesh * mesh, const DrawSurface * surface, const glm::vec4 & shade);
    int count;
    DrawElement element;
  };

  std::unordered_map<size_t, DrawStruct> objects_;
  std::vector<size_t> drawOrder_;
  /*
  Sort

  List of objects _/
  Sorting function _/
  Global transformation

  */
  glm::mat4 global_;
  size_t total_;
  COMPFUNC sorter_;
  MODFUNC modifier_;
};

