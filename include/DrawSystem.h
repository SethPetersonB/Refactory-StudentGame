// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <memory>

#include "Renderer.h"
#include "DrawGroup.h"
//#include "RMesh.h"
#include "Texture.h"
#include "Draw_fwd.h"

class DrawSystem
{
public:
  template<typename KEY, typename VAL>
  using RES_MAP = std::unordered_map < KEY, std::unique_ptr<VAL>>;

  DrawSystem(SDL_Window * disp, size_t x, size_t y, size_t width, size_t height);

  DrawSystem(const DrawSystem &) = delete;
  DrawSystem & operator=(const DrawSystem &) = delete;

  const Texture * getTexture(const std::string & name) const;
  const RMesh * getMesh(const std::string & name) const;

  void loadMesh(const std::string & name, const RMesh & mesh);
  void loadTexture(const std::string & name, const std::string & path, size_t frames = 1);
  
  void loadVertexShader(const std::string & name, const std::string & path);
  void loadFragmentShader(const std::string & name, const std::string & path);

  void loadAndUseVertexShader(const std::string & name, const std::string & path);
  void loadAndUseFragmentShader(const std::string & name, const std::string & path);

  void useVertexShader(const std::string & name);
  void useFragmentShades(const std::string & name);

  void unloadVertexShader(const std::string & name);
  void unloadFragmentShader(const std::string & name);

  RES_MAP<std::string, Texture>::iterator firstTexture()
  {
    return textures_.begin();
  }

  bool loadNextTexture(RES_MAP<std::string, Texture>::iterator & curr);

  std::unique_lock<std::mutex> makeCurrent();

  size_t getViewWidth() const;
  size_t getViewHeight() const;
  size_t getViewOffsetX() const;
  size_t getViewOffsetY() const;

  void resize(size_t x, size_t y, size_t width, size_t height);

  DrawGroup & getDrawGroup(DrawLayer layer);
  DrawGroup & newDrawGroup(DrawLayer layer, const COMPFUNC & comp = nullptr);

  DrawToken newElement(DrawLayer layer,
    const glm::vec2 & pos, const glm::vec2 & scale, float rot,
    const std::string & mesh, const std::string & surface = std::string{},
    const glm::vec4 & shade = { 1, 1, 1, 1 });

  DrawToken newElement(DrawLayer layer, const std::string & mesh, const std::string & surface = "");

  void update();
  void swap(float r, float g, float b, float a = 1.f);

private:
  
  Renderer render_;

  RES_MAP<DrawLayer, DrawGroup> layers_;
  RES_MAP<std::string, RMesh> meshes_;
  RES_MAP<std::string, Shader> vertexShaders_;
  RES_MAP<std::string, Shader> fragmentShaders_;
  RES_MAP<std::string, Texture> textures_;
};