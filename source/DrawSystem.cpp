// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <GL/GL.h>

#include "DrawSystem.h"
#include "Texture.h"
#include "RMesh.h"
#include "Shader.h"

using namespace Logger;

template<typename KEYTYPE, typename RESTYPE, typename ...INIT_TYPES>
void loadResource(DrawSystem::RES_MAP<KEYTYPE, RESTYPE> & resMap, const KEYTYPE & key,  const INIT_TYPES & ... init)
{
  resMap.erase(key);
  resMap.insert(std::make_pair(key, std::make_unique<RESTYPE>(init...)));
}

DrawSystem::DrawSystem(SDL_Window * disp, size_t x, size_t y, size_t width, size_t height) :
  render_(disp, x, y, width, height)
{}

const Texture * DrawSystem::getTexture(const std::string & name) const
{
  auto it = textures_.find(name);

  if (it == textures_.end())
    return nullptr;

  return it->second.get();
}

const RMesh * DrawSystem::getMesh(const std::string & name) const
{
  auto it = meshes_.find(name);

  if (it == meshes_.end())
    return nullptr;

  return it->second.get();
}

void DrawSystem::loadMesh(const std::string & name, const RMesh & mesh)
{
  loadResource(meshes_, name, mesh);
}

void DrawSystem::loadTexture(const std::string & name, const std::string & path, size_t frames)
{
  if (name == "")
    throw std::runtime_error("Invalid texture name given");

  loadResource(textures_, name, path, frames);
}

void DrawSystem::loadVertexShader(const std::string & name, const std::string & path)
{
  loadResource(vertexShaders_, name, path, GL_VERTEX_SHADER);
}

void DrawSystem::loadFragmentShader(const std::string & name, const std::string & path)
{
  loadResource(fragmentShaders_, name, path, GL_FRAGMENT_SHADER);
}

void DrawSystem::loadAndUseVertexShader(const std::string & name, const std::string & path)
{
  loadVertexShader(name, path);
  useVertexShader(name);
}

void DrawSystem::loadAndUseFragmentShader(const std::string & name, const std::string & path)
{
  loadFragmentShader(name, path);
  useFragmentShades(name);
}

void DrawSystem::useVertexShader(const std::string & name)
{
  render_.useVertexShader(*vertexShaders_.at(name));
}

void DrawSystem::useFragmentShades(const std::string & name)
{
  render_.useFragmentShader(*fragmentShaders_.at(name));
}

void DrawSystem::unloadVertexShader(const std::string & name)
{
  vertexShaders_.erase(name);
}

void DrawSystem::unloadFragmentShader(const std::string & name)
{
  fragmentShaders_.erase(name);
}


bool DrawSystem::loadNextTexture(RES_MAP<std::string, Texture>::iterator & curr)
{
  if (curr == textures_.end())
    curr = textures_.begin();

  if (curr != textures_.end())
  {
    Log<RenderInfo>("Binding texture %s", curr->first.c_str());

    curr->second->bind();
    curr->second->unbind();

    ++curr;
  }

  return curr == textures_.end();
}

std::unique_lock<std::mutex> DrawSystem::makeCurrent()
{
  return render_.makeCurrent();
}

size_t DrawSystem::getViewWidth() const
{
  return render_.getWidth();
}

size_t DrawSystem::getViewHeight() const
{
  return render_.getHeight();
}

size_t DrawSystem::getViewOffsetX() const 
{
  return render_.getX();
}

size_t DrawSystem::getViewOffsetY() const
{
  return render_.getY();
}

void DrawSystem::resize(size_t x, size_t y, size_t width, size_t height)
{
  render_.resize(x, y, width, height);
}

DrawGroup & DrawSystem::getDrawGroup(DrawLayer layer)
{
  return *layers_.at(layer);
}

DrawGroup & DrawSystem::newDrawGroup(DrawLayer layer, const COMPFUNC & comp)
{
  auto it = layers_.find(layer);

  if (it != layers_.end())
    throw std::runtime_error("Group with the given layer already exists!");

  loadResource(layers_, layer, comp);

  return getDrawGroup(layer);
}

DrawToken DrawSystem::newElement(DrawLayer layer, const glm::vec2 & pos, const glm::vec2 & scale, float rot, const std::string & mesh, const std::string & surface, const glm::vec4 & shade)
{
  const RMesh * mes = getMesh(mesh);
  const Texture * tex = getTexture(surface);
  auto element = layers_.at(layer)->newElement(pos, scale, rot, mes, tex, shade);
  layers_.at(layer)->getElement(element.id_).layer = layer;

  return element;
}

DrawToken DrawSystem::newElement(DrawLayer layer, const std::string & mesh, const std::string & surface)
{
  const RMesh * mes = getMesh(mesh);
  const Texture * tex = getTexture(surface);
  auto element = layers_.at(layer)->newElement(mes, tex);

  layers_.at(layer)->getElement(element.id_).layer = layer;

  return element;
}

void DrawSystem::update()
{
  for (auto & layer : layers_)
  {
    layer.second->draw(render_);
  }
}

void DrawSystem::swap(float r, float g, float b, float a)
{
  render_.swap(r, g, b, a);
}



