// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <SDL2/SDL.h>

#include <GL/glew.h>
#include <GL/GL.h>

#include <stdexcept>

#include "glm/glm/mat4x4.hpp"

#include "Renderer.h"
#include "RMesh.h"
#include "DrawSurface.h"
#include "Shader.h"
#include "Texture.h"


using namespace Logger;

static std::mutex LOCKER;

Renderer::Renderer(SDL_Window * win, size_t x, size_t y, size_t width, size_t height) :
  dev_{ nullptr }, devcon_{ nullptr }, compShader_{ NULL }
{
  setWindow(win);
  init(x, y, width, height);
}

Renderer::~Renderer()
{
  clean();
}

void Renderer::clean()
{
  if (devcon_)
    SDL_GL_DeleteContext(devcon_);
  
  if (compShader_ != NULL)
    glDeleteProgram(compShader_);
}

void Renderer::setWindow(SDL_Window * win)
{
  if (devcon_ != nullptr)
  {
    clean();
    dev_ = win; // Not sure if order matters here, but it's just a precaution
    init(x_, y_, width_, height_);
  }
  else
    dev_ = win;
}

void Renderer::init(size_t x, size_t y, size_t width, size_t height)
{
  clean();
  

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetSwapInterval(1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  devcon_ = SDL_GL_CreateContext(dev_);

  auto lock = makeCurrent();

  glEnable(GL_LINE_SMOOTH);
  //glEnable(GL_POLYGON_SMOOTH);  // Causes black lines between tris
  //glEnable(GL_MULTISAMPLE);
  //glEnable(GL_DEPTH_TEST);

  if (!devcon_)
  {
#ifndef N_LOGGING
    Log<RenderError>("Failed to create GL context!");
#endif

    throw std::runtime_error("Failed to create the OpenGL context");
  }

  GLenum status = glewInit();

  if (status != GLEW_OK)
  {
#ifndef N_LOGGING
    Log<RenderError>("Glew failed to Initialize!");
#endif

    throw std::runtime_error("Glew failed to initialize");
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  compShader_ = glCreateProgram();
  
  //lock.unlock();

  resize(x, y, width, height);
}

void Renderer::useVertexShader(const Shader & shader)
{
  if (shader.isShaderLoaded())
  {
    vertexShader_ = shader;
  }

   reloadShader();

}

void Renderer::useFragmentShader(const Shader & shader)
{
  if (shader.isShaderLoaded())
  {
    fragmentShader_ = shader;
  }

  reloadShader();
}


void Renderer::resize(size_t x, size_t y, size_t width, size_t height)
{
  x_ = x;
  y_ = y;
  width_ = width;
  height_ = height;

  auto lock = makeCurrent();

  glViewport(x, y, width, height);
}

void Renderer::draw(const RMesh & mesh, const glm::mat4 & mat, const glm::vec4 & shade, const DrawSurface * tex, unsigned currFrame, GLenum drawMode)
{
  makeCurrent();
  GLuint vaoID;
  glGenVertexArrays(1, &vaoID);

 /* if (count > 1)
  {
    Log<RenderInfo>("Frame count: %u \n Current frame: %u", count, currFrame);
  }*/
  bool textured = tex != nullptr;

  // Bind textures before VAO
  if (textured)
    tex->bind();

  glBindVertexArray(vaoID);

  glUseProgram(compShader_);

  GLuint vbo;
  GLuint faces;

  // Generate vertex buffer
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &faces);

  if (vbo != NULL && faces != NULL)
  {
    GLint texturedLocation = glGetUniformLocation(compShader_, "textured");
    GLint transformLocation = glGetUniformLocation(compShader_, "transform");
    GLint colorLocation = glGetUniformLocation(compShader_, "shade");
    GLint cFrame = glGetUniformLocation(compShader_, "currFrame");
    GLint frameCount = glGetUniformLocation(compShader_, "frameCount");

    //GLint currFrameLocation = GSM::get().getColorShader().getUniformLocation("currFrame");
    //GLint frameCountLocation = GSM::get().getColorShader().getUniformLocation("frameCount");

    // Send whether the object is textured
    glUniform1i(texturedLocation, textured);

    // Send vertex transform information
    glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &mat[0][0]);

    glUniform4f(colorLocation, shade.r, shade.g, shade.b, shade.a);

    glUniform1ui(cFrame, currFrame);
    glUniform1ui(frameCount, (tex != nullptr) ? static_cast<const Texture *>(tex)->FrameCount() : 1);

    // Bind vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // send color data
    //glUniform3f(ucolor_, );

    // Send vertex data
    glBufferData(GL_ARRAY_BUFFER,
      sizeof(Vertex) * mesh.verts.size(), &(mesh.verts[0]),
      GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0,                                  // Which index to use ( We enabled Attrib Array # 0 )
      2,                                  // We have 3 data points (x and y, z, w)
      GL_FLOAT,                           // Which are floats
      GL_FALSE,                           // We don't want points to be normalized
      sizeof(Vertex),
      (void*)offsetof(Vertex, point));    // We send the offset before the data points used (position) within the Vertex struct

                                          // Send color data GPU
    glVertexAttribPointer(1,                                  // Which index to use ( We enabled Attrib Array # 1 )
      4,                                  // We have 4 data points (r, g, b, and a)
      GL_FLOAT,                           // Which are GL unsigned bytes
      GL_FALSE,                            // We want points to be normalized
      sizeof(Vertex),                     // We send data a vertex at a time (3 floats and 4 GLuints)
      (void*)offsetof(Vertex, color));    // We send the offset before the data points used (color) within the Vertex struct
    


    glVertexAttribPointer(2,                                  // Which index to use ( We enabled Attrib Array # 1 )
      2,                                  // We have 4 data points (r, g, b, and a)
      GL_FLOAT,                           // Which are GL unsigned bytes
      GL_FALSE,                            // We want points to be normalized
      sizeof(Vertex),                     // We send data a vertex at a time (3 floats and 4 GLuints)
      (void*)offsetof(Vertex, uv));    // We send the offset before the data points used (color) within the Vertex struct


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faces);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.tris.size() * sizeof(Tri),
      (unsigned *)(&mesh.tris.front()), GL_STATIC_DRAW);

    glDrawElements(drawMode, 3 * mesh.tris.size(), GL_UNSIGNED_INT, 0);
    //glDrawArrays(drawMode, 0, mesh.verts.size());

    

    if (textured)
      tex->unbind();
  }

  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &faces);
  glDeleteVertexArrays(1, &vaoID);

  glBindVertexArray(NULL);
}

void Renderer::swap(float r, float g, float b, float a)
{
  auto lock = makeCurrent();

  // Swap back and front buffers
  SDL_GL_SwapWindow(dev_);

  // Clear the backbuffer
  glClearDepth(0.0f);
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Renderer::reloadShader()
{
  if (!vertexShader_.isShaderLoaded() || !fragmentShader_.isShaderLoaded())
    return false;

  GLuint vs = vertexShader_.location();
  GLuint ps = fragmentShader_.location();

  //glBindAttribLocation(compShader_, 0, "position");
  //glBindAttribLocation(compShader_, 1, "color");

  glAttachShader(compShader_, vertexShader_.location());
  glAttachShader(compShader_, fragmentShader_.location());


  GLint value;

  glLinkProgram(compShader_);


  glGetShaderiv(vertexShader_.location(), GL_COMPILE_STATUS, &value);

  if (!value)
  {
    //GLint logSize = 0;
    throw std::runtime_error("Vertex shader");
  }

  glGetShaderiv(fragmentShader_.location(), GL_COMPILE_STATUS, &value);

  if (!value)
    throw std::runtime_error("Fragment shader");

  int status;

  glGetShaderiv(compShader_, GL_LINK_STATUS, &status);

  //if (!status)
  //{
  //  GLint logSize = 0;
  //  std::vector<char> log;

  //  glGetShaderiv(compShader_, GL_INFO_LOG_LENGTH, &logSize);

  //  log.resize(logSize);
  //  glGetShaderInfoLog(compShader_, logSize, &logSize, &log.front());

  //  std::cout << &log.front() << std::endl;
  //}

  //sampler_ = glGetUniformLocation(compShader_, "diffuse");

  //aposition_ = glGetAttribLocation(compShader_, "position");
  //acolor_ = glGetAttribLocation(compShader_, "color");

  //glDetachShader(compShader_, vertexShader_.location());
  //glDetachShader(compShader_, fragmentShader_.location());

  //ucolor_ = glGetUniformLocation(program, "color");
  //utransform = glGetUniformLocation(program, "transform");

  //glEnableVertexAttribArray(aposition_);
  //glEnableVertexAttribArray(acolor_);

  return compShader_ != NULL;
}

std::unique_lock<std::mutex> Renderer::makeCurrent()
{
  std::unique_lock<std::mutex> lock{LOCKER, std::defer_lock};

  try {
    //lock.lock();

    SDL_GL_MakeCurrent(dev_, devcon_);

  }
  catch (const std::exception & e)
  {
     Log<RenderError>("%s", e.what());
  }
  return lock;
}

size_t Renderer::getWidth() const
{
  return width_;
}

size_t Renderer::getHeight() const
{
  return height_;
}

size_t Renderer::getX() const
{
  return x_;
}

size_t Renderer::getY() const
{
  return y_;
}
