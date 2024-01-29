// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <fstream>

#include "Texture.h"
#include "SOIL.h"
#include "Draw_fwd.h"
#ifndef NLOGGING
using namespace Logger;
#endif

  //unsigned Texture::count = 0;

  /****************************************************************************/
  /*!
  \brief
  Constructor for textures that initializes their data

  \param image
  The image file to load from and use as the texture

  \param frames
  How many frames a texture has, if it can be used as a spritesheet
  */
  /****************************************************************************/
Texture::Texture(const std::string& image, size_t frames) : 
  imagePath_(image), frames_(frames)
{
  Log<Info>("Loading Texture : %s", image.c_str());
  std::ifstream testFile(image, std::ios::binary);
  bmp = std::vector<unsigned char>{ std::istreambuf_iterator<char>(testFile),
    std::istreambuf_iterator<char>() };
}
  
  Texture::~Texture()
  {
    // delete texture
    if(texture_ == NULL)
      glDeleteTextures(1, &texture_);

//#ifndef N_LOGGING
//    Log<Info>("Deleting texture %s", mImage.c_str());
//#endif
  }
  
  /****************************************************************************/
  /*!
  \brief
  Allows changing of active textures so current one will be drawn
  */
  /****************************************************************************/
  void Texture::bind() const
  {
    if (texture_ == NULL)
      loadTexture();

    glBindTexture(GL_TEXTURE_2D, texture_);
  }

  void Texture::unbind() const
  {
    glBindTexture(GL_TEXTURE_2D, NULL);
  }

  void Texture::loadTexture() const
  {
    glActiveTexture(GL_TEXTURE0);
    //glGenTextures(1, &texture_);

    // Bind texture
    //glBindTexture(GL_TEXTURE_2D, texture_);

    // Set texture properteis
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //unsigned char * imageData = SOIL_load_image(image.c_str(), &imageWidth_, &imageHeight_, 0, SOIL_LOAD_RGBA);

    //if (imageData == nullptr)

    // Send texture data
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth_, imageHeight_, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    //glGenerateTextureMipmap(GL_TEXTURE_2D);

    // Free loaded texture data
    //SOIL_free_image_data(imageData);
    texture_ = SOIL_load_OGL_texture_from_memory(bmp.data(), bmp.size(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    Log<RenderInfo>("SOIL: %s: %s", SOIL_last_result(), imagePath_.c_str());

    glBindTexture(GL_TEXTURE_2D, NULL);
    //#ifndef N_LOGGING
    //
    //  Log<Info>("Creating texture %s", image.c_str());
    //#endif
  }
