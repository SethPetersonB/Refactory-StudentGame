// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include <GL/glew.h>
#include <GL/GL.h>
#include <vector>

#include "DrawSurface.h"

class Texture : public DrawSurface
{
public:
  Texture(const std::string& image, size_t frames = 1);
  Texture(const Texture &) = delete;
  Texture & operator=(const Texture &) = delete;

  virtual ~Texture();

  void bind() const override;
  void unbind() const override;

  const std::string& getImage() const { return imagePath_; }

  const size_t & FrameCount() const { return frames_; }
  void setNumFrames(size_t num) { frames_ = num; }

private:
  void loadTexture() const;

  std::string imagePath_; // name of image file

  int imageWidth_;
  int imageHeight_;
  int channels_;

  std::vector<unsigned char> bmp;
  mutable GLuint texture_ = NULL;

  size_t frames_;

  //static unsigned count; // how many textures exist
};
