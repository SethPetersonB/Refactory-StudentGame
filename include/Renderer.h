// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include<memory>
#include <mutex>

#include "glm/glm/mat4x4.hpp"
#include "Shader.h"
#include "Draw_fwd.h"

class Renderer
{
public:
  Renderer(SDL_Window * win, size_t x, size_t y, size_t width, size_t height);
  ~Renderer();

  Renderer(const Renderer &) = delete;
  Renderer & operator=(const Renderer &) = delete;

  void useVertexShader(const Shader & shader);
  void useFragmentShader(const Shader & shader);

  void clean();

  void setWindow(SDL_Window * win);
  void init(size_t x, size_t y, size_t width, size_t height);

  void resize(size_t x, size_t y, size_t width, size_t height);

  void draw(const RMesh & mesh, const glm::mat4 & mat, const glm::vec4 & shade, const DrawSurface * tex = nullptr, unsigned currFrame = 0, GLenum drawMode = GL_TRIANGLES);
  void swap(float r, float g, float b, float a = 1);
  bool reloadShader();
  std::unique_lock<std::mutex> makeCurrent();

  size_t getWidth() const;
  size_t getHeight() const;
  size_t getX() const;
  size_t getY() const;

private:  
  size_t width_;
  size_t height_;
  size_t x_;
  size_t y_;

  SDL_Window * dev_;
  void * devcon_;
  Shader vertexShader_;
  Shader fragmentShader_;

  GLuint compShader_;
};
