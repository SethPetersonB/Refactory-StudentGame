// Primary Author : Kento Murawski
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include <GL/glew.h>
#include <GL/GL.h>

#include <vector>
#include <string> // string
#include <memory> // shared_ptr
class Shader
{
public:
  Shader();
  Shader(const std::string & path, GLenum type);

  GLuint location() const;
  GLenum shaderType() const;
  bool isShaderLoaded() const;
  const std::vector<std::string> & getUniforms();

  bool loadShader(const std::string & path, GLenum shaderType);

  void unloadShader();

private:
  struct shader_manager
  {
    //shader_manager();
    ~shader_manager();

    std::string path;
    GLuint location;
    GLenum type;
    std::vector<std::string> uniforms;
  };

  std::shared_ptr<shader_manager> ptr_;
};
