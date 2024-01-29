// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "DrawToken.h"
#include "DrawGroup.h"

DrawToken::DrawToken() : 
  parent_(nullptr), id_(0)
{}

/**
* \brief  Copy constructor for DrawTokens
*
* \param  cpy The copy
*/
DrawToken::DrawToken(const DrawToken & cpy) : 
  id_{ cpy.id_ }, parent_{ cpy.parent_ }
{
  registerMe();
}


/**   
* \brief  Destructor for DrawTokens. Unregisters the token from the system
*/
DrawToken::~DrawToken()
{
  deregisterMe();
}

/**
* \brief  Assignment operator for DrawTokens
*
* \param  rhs The right hand side
*
* \return A reference to this object.
*/
DrawToken & DrawToken::operator=(const DrawToken & rhs)
{
  // Self-assignment check
  if (operator==(rhs))
    return *this;

  // Deregister old token
  deregisterMe();

  const_cast<DrawGroup *>(parent_) = rhs.parent_;
  id_ = rhs.id_;

  // Register new token
  registerMe();

  return *this;
}

/**
* \brief  Equality operator.
*
* \param  rhs The right hand side
*
* \return True if the parameters are considered equivalent.
*/
bool DrawToken::operator==(const DrawToken & rhs) const
{
  return (id_ == rhs.id_) && (parent_ == rhs.parent_);
}

ScaleReference DrawToken::getScaleReference() const
{
  return  parent_->getElement(id_).ref;
}

bool DrawToken::isVisible() const
{
  return  parent_->getElement(id_).visible;
}

glm::mat4 DrawToken::getMatrix(float ar) const
{
  return  parent_->getElement(id_).getMatrix(ar);
}

glm::mat4 DrawToken::getFinalMatrix(float ar) const
{
  MODFUNC modFunc = parent_->modifier_;

  glm::mat4 mod;
  glm::mat4 objectTrans;
  bool ovride = false;

  if (modFunc)
  {
    auto res = modFunc(*this);
    mod = res.first;
    ovride = res.second;
  }

  if (ovride)
    objectTrans = mod;
  else
    objectTrans = mod * getMatrix(ar);

  return parent_->global_ * objectTrans;
}

/**
* \brief  Gets the shade.
*
* \return The shade.
*/
glm::vec4 DrawToken::getShade() const
{
  return parent_->getElement(id_).shade;
}

/**
* \brief  Gets the position.
*
* \return The position.
*/
glm::vec2 DrawToken::getPosition() const
{
  return parent_->getElement(id_).position;
}

glm::vec2 DrawToken::getFinalPosition(float ar) const
{
  glm::vec4 pos{ getPosition().x, getPosition().y, 0, 1 };
  glm::mat4 mat = getFinalMatrix(ar);

  pos = mat * pos;

  return glm::vec2{ pos.x, pos.y };
}

/**
* \brief  Gets the scale.
*
* \return The scale.
*/
glm::vec2 DrawToken::getScale() const
{
  return parent_->getElement(id_).scale;
}

size_t DrawToken::getDrawOrder() const
{
  return parent_->getDrawOrder(id_);
}

unsigned DrawToken::getFrame() const
{
  return parent_->getElement(id_).frame;
}

DrawLayer DrawToken::getLayer() const
{
  return parent_->getElement(id_).layer;
}

/**
* \brief  Gets the rotation.
*
* \return The rotation.
*/
float DrawToken::getRotation() const
{
  return parent_->getElement(id_).rotation;
}

float DrawToken::getDepth() const
{
  return parent_->getElement(id_).depth;
}

float DrawToken::getIsoY() const
{
  return  parent_->getElement(id_).isoY;
}

/**
* \brief  Gets the mesh.
*
* \return The mesh used by the object
*/
const RMesh * DrawToken::getMesh() const
{
  return parent_->getElement(id_).mesh;
}

/**
* \brief  Gets draw surface.
*
* \return The drawsurface used by the object
*/
const DrawSurface * DrawToken::getDrawSurface() const
{
  return parent_->getElement(id_).surface;
}

/**
* \brief  Scales the given amount.
*
* \param  amount  The amount to scale
*/
void DrawToken::scale(const glm::vec2 & amount)
{
  setScale(getScale() * amount);
}

/**
* \brief  Scales the given amount
*
* \param  amount  The amount to scale
*/
void DrawToken::scale(float amount)
{
  setScale(getScale() * glm::vec2{ amount, amount });
}

/**
* \brief  Rotates the given amount in radians
*
* \param  amount  The amount to rotate
*/
void DrawToken::rotate(float amount)
{
  setRotation(getRotation() + amount);
}

/**
* \brief  Translates the given amount.
*
* \param  amount  The amount to translate
*/
void DrawToken::translate(const glm::vec2 & amount)
{
  setPosition(getPosition() + amount);
}

void DrawToken::setScaleReference(ScaleReference ref)
{
  parent_->getElement(id_).ref = ref;
}

void DrawToken::setVisible(bool visible)
{
  parent_->getElement(id_).visible = visible;
}

/**
* \brief  Sets a shade.
*
* \param  shade The shade to set
*/
void DrawToken::setShade(const glm::vec4 & shade)
{
  parent_->getElement(id_).shade = shade;
}

/**
* \brief  Sets a position.
*
* \param  pos The position
*/
void DrawToken::setPosition(const glm::vec2 & pos)
{
  parent_->getElement(id_).position = pos;
}

/**
* \brief  Sets a scale.
*
* \param  scale The scale
*/
void DrawToken::setScale(const glm::vec2 & scale)
{
  parent_->getElement(id_).scale = scale;
}

/**
* \brief  Sets a scale.
*
* \param  scale The scale
*/
void DrawToken::setScale(float scale)
{
  parent_->getElement(id_).scale = glm::vec2{ scale, scale };
}

void DrawToken::setFrame(unsigned frame)
{
  parent_->getElement(id_).frame = frame;
}

/**
* \brief  Sets a rotation.
*
* \param  rot The rot
*/
void DrawToken::setRotation(float rot)
{
  parent_->getElement(id_).rotation = rot;
}

void DrawToken::setDepth(float depth)
{
  parent_->getElement(id_).depth = depth;
}

/**
* \brief  Sets a mesh.
*
* \param  mesh  The mesh
*/
void DrawToken::setMesh(const RMesh * mesh)
{
  parent_->getElement(id_).mesh = mesh;
}

/**
* \brief  Sets draw surface.
*
* \param  surface The surface
*/
void DrawToken::setDrawSurface(const DrawSurface * surface)
{
  parent_->getElement(id_).surface = surface;
}

void DrawToken::setIsoY(float y) const
{
  parent_->getElement(id_).isoY = y;
}

MODFUNC DrawToken::getModFunc() const
{
  return parent_->modifier_;
}

/**
* \brief  Constructor.
*
* \param  parent  The parent for the token
* \param  id      The id for the token
*/
DrawToken::DrawToken(DrawGroup * parent, size_t id) :
  id_(id), parent_(parent)
{
  registerMe();
}

/**   
* \brief  Registers the token with tne system.  
*/
void DrawToken::registerMe()
{
  if (parent_ != nullptr)
    parent_->registerToken(id_);
}


/**   
* \brief  Unregisters the token from the system
*/
void DrawToken::deregisterMe()
{
  if (parent_ != nullptr)
    parent_->deregisterToken(id_);
}
