// Primary Author : Kento Murawski
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <fstream>
#include <vector>
#include <streambuf>
#include <iostream>

#include "Shader.h"

Shader::Shader()
{}

Shader::Shader(const std::string & path, GLenum type) 
{
  loadShader(path, type);
}

GLuint Shader::location() const
{
  return (ptr_) ? ptr_->location : NULL;
}

GLenum Shader::shaderType() const
{
  return ptr_->type;
}

bool Shader::isShaderLoaded() const
{
  return (ptr_ && ptr_->location);
}

bool Shader::loadShader(const std::string & path, GLenum shaderType)
{
  if (ptr_)
    unloadShader();
  else
    ptr_ =std::make_shared<shader_manager>();

  std::ifstream file{ path };
  int status = file.is_open();

  if (status)
  {
    ptr_->location = glCreateShader(shaderType);
    ptr_->type = shaderType;
    ptr_->path = path;

    std::string shaderSource{ std::istreambuf_iterator<char>(file),
    std::istreambuf_iterator<char>() };

    //// Read contents of file into string
    //file.seekg(0, std::ios::end);
    //shaderSource.resize(file.tellg());
    //file.seekg(0, std::ios::beg);
    //file.read(&shaderSource[0], shaderSource.size());
    //file.close();

    const GLchar * shaderSourceStrings{ shaderSource.c_str() };  // Ugh
    const GLint length = shaderSource.size();                     // Why?

    if (ptr_->location != NULL)
    {
      glShaderSource(ptr_->location, 1, &shaderSourceStrings, 0);
      glCompileShader(ptr_->location);

      glGetShaderiv(ptr_->location, GL_COMPILE_STATUS, &status);

      if (!status)
      {
        GLint logSize = 0;
        std::vector<char> log;

        glGetShaderiv(ptr_->location, GL_INFO_LOG_LENGTH, &logSize);

        log.resize(logSize);
        glGetShaderInfoLog(ptr_->location, logSize, &logSize, &log.front());

        std::cout << &log.front() << std::endl;
      }
    }

  }
  
  return status;
}

void Shader::unloadShader()
{
  if (ptr_ && ptr_->location != NULL)
  {
    glDeleteShader(ptr_->location);
    ptr_->location = NULL;
  }

  ptr_ = std::make_shared<shader_manager>();
}

Shader::shader_manager::~shader_manager()
{
  if(location != NULL)
    glDeleteShader(location);
}

