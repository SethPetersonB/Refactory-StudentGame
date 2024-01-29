// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "DrawGroup.h"
#include "Renderer.h"
#include "RMesh.h"
#include "DrawSurface.h"

using namespace Logger;

/**
* \brief  Constructor for DrawGroups. Sets the sorting function and initializes ID values
*
* \param  sorter  The sorter
*/
DrawGroup::DrawGroup(const COMPFUNC & sorter) :
  total_{ 0 }, sorter_{ sorter }
{}

/**
* \brief  Gets the global transformation matrix applied to all objects in the group
*
* \return The transformation matrix
*/
glm::mat4 DrawGroup::getTransformation() const
{
  return global_;
}

/**
* \brief  Sets sort function.
*
* \param  func  The function
*/
void DrawGroup::setSortFunc(const COMPFUNC & func)
{
  sorter_ = func;
}

void DrawGroup::setModFunc(const MODFUNC & func)
{
  modifier_ = func;
}

/**
* \brief  Sets the global transformation.
*
* \param  transfrom The transfromation
*/
void DrawGroup::setTransformation(const glm::mat4 & transfrom)
{
  global_ = transfrom;
}

/**
* \brief  Creates a new element.
*
* \param  pos     The position
* \param  scale   The scale
* \param  rot     The rot
* \param  mesh    The mesh
* \param  surface The surface
* \param  shade   The shade
*
* \return A DrawToken.
*/
DrawToken DrawGroup::newElement(const glm::vec2 & pos, const glm::vec2 & scale, float rot, const RMesh * mesh, const DrawSurface * surface, const glm::vec4 & shade)
{
  size_t id = ++total_;

  drawOrder_.push_back(id);
  objects_.insert(std::make_pair(id, DrawStruct{ this, pos, scale, rot, mesh, surface, shade }));

  return DrawToken(this, id);
}

/**
* \brief  Creates a new element.
*
* \param  mesh    The mesh
* \param  surface The surface
*
* \return A DrawToken.
*/
DrawToken DrawGroup::newElement(const RMesh * mesh, const DrawSurface * surface)
{
  return newElement(glm::vec2{ 0, 0 }, glm::vec2{ 1, 1 }, 0, mesh, surface);
}

/**
* \brief  Draws all elements to the given renderer
*
* \param [in,out] render  The render to draw to
*/
void DrawGroup::draw(Renderer & render)
{
  scrub();
  sort();

  float ar = static_cast<float>(render.getWidth()) / render.getHeight();
  for (auto & elId : drawOrder_)
  {
    const DrawElement & element = getElement(elId);


    if (element.doesNeedUpdate())
      element.update();
    
    if (element.visible)
    {
      const DrawToken token = getToken(elId);
      glm::mat4 objectTrans{ token.getFinalMatrix(ar) };

      render.draw(*element.mesh, objectTrans, element.shade, element.surface, element.frame);
    }
  }
}

size_t DrawGroup::getDrawOrder(size_t id) const
{
  for (size_t i = 0; i < drawOrder_.size(); i++)
  {
    if (drawOrder_[i] == id)
      return i;
  }

  return drawOrder_.size();
}

size_t DrawGroup::size() const
{
  return objects_.size();
}

/**
* \brief  Registers the token described by ID.
*
* \param  id  The identifier
*
* \return True if it succeeds, false if it fails.
*/
bool DrawGroup::registerToken(size_t id)
{
  auto it = objects_.find(id);

  bool found = (it != objects_.end());

  if (found)
    ++(it->second.count);

  return found;
}

/**
* \brief  Deregisters the token described by ID.
*
* \param  id  The identifier
*/
void DrawGroup::deregisterToken(size_t id)
{
  auto it = objects_.find(id);

  bool found = (it != objects_.end());

  if (found)
  {
    --(it->second.count);

    // Remove an object if all references are deregistered
    if (it->second.count <= 0)
      objects_.erase(it);
  }
}

/**
* \brief  Gets an element.
*
* \exception  std::out_of_range Thrown when an invalid token is passed
*                               
* \param  token The token
*
* \return The element.
*/
const DrawElement & DrawGroup::getElement(size_t token) const
{
  auto it = objects_.find(token);

  if (it == objects_.end())
    throw std::out_of_range("Attempting to retrieve unknown draw element");

  return it->second.element;
}

/**
* \brief  Gets a token for the given id.
*
* \param  id  The identifier
*
* \return The token.
*/
DrawToken DrawGroup::getToken(size_t id)
{
  return DrawToken(this, id);
}

/**
* \brief  Gets an element.
*
* \exception  std::out_of_range Thrown when an invalid token is passed
*
* \param  token The token
*
* \return The element.
*/
DrawElement & DrawGroup::getElement(size_t token)
{
  auto it = objects_.find(token);

  if (it == objects_.end())
    throw std::out_of_range("Attempting to retrieve unknown draw element");

  DrawElement & element = it->second.element;

  element.needsUpdate_ = true;

  return element;
}

/**   
* \brief  Sorts the draw order for the group
*/
void DrawGroup::sort()
{
  if (!sorter_)
    return;

  //insertion sort
  size_t currIndex = 1;

  while (currIndex < drawOrder_.size())
  {
    size_t swp = currIndex;

    while (swp > 0 && sorter_(getToken(drawOrder_[swp]), getToken(drawOrder_[swp - 1])))
    {
      std::swap(drawOrder_[swp], drawOrder_[swp - 1]);
      --swp;
    }

    ++currIndex;
  }
}


/**   
* \brief  Removes all expired objects from the group
*/
void DrawGroup::scrub()
{
  auto it = drawOrder_.begin();

  while (it != drawOrder_.end())
  {
    if (objects_.find(*it) == objects_.end())
      it = drawOrder_.erase(it);
    else
      ++it;
  }
}

/**
* \brief  Constructor for DrawStruct
*
* \param [in,out] parent  The parent
* \param          pos     The position
* \param          scale   The scale
* \param          rot     The rot
* \param          mesh    The mesh
* \param          surface The surface
* \param          shade   The shade
*/
DrawGroup::DrawStruct::DrawStruct(DrawGroup * parent, const glm::vec2 & pos, const glm::vec2 & scale, float rot, const RMesh * mesh, const DrawSurface * surface, const glm::vec4 & shade) :
  count{ 0 }, element{ parent, pos, scale, rot, mesh, surface, shade }
{}
