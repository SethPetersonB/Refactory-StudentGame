// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "DrawUtils.h"
#include "DrawSystem.h"
#include "SDL2/SDL.h"

#include "../include/json/json.h"
#include "../include/Logger.h"

#include <fstream>

using namespace Logger;

namespace DrawUtils
{
  /****************************************************************************/
  /*!
    \brief
      Loads texture data in from the given file path

    \param defPath
      Path to the json file to load data from
  */
  /****************************************************************************/
  void R_LoadTextures(DrawSystem * sys, const std::string & defPath)
  {
    Log<Info>("Loading...");

    std::ifstream textureDefinitions(defPath);
    Json::Reader reader;
    Json::Value root;
    const Json::Value defValue;

    if (textureDefinitions.is_open())
    {
      reader.parse(textureDefinitions, root);

      std::vector<std::string> textureFolders = root.getMemberNames();

      // get each folder
      for (auto & folderPath : textureFolders)
      {
        Json::Value folder = root.get(folderPath, defValue);

        if (folder != defValue)
        {
          // Get each texture in the folder
          std::vector<std::string> textureDefs = folder.getMemberNames();

          for (auto & textureDef : textureDefs)
          {
            Json::Value texture = folder.get(textureDef, defValue);

            // error
            if (texture == defValue)
            {
              Log<Warning>("Error parsing texture definitions in file '%s'", defPath.c_str());
              throw(std::runtime_error("Error parsing texture definitions"));
            }

            // Get the alias of the texture
            std::string & alias = textureDef;

            // Get the relative path of the texture
            std::string texPath = folderPath + texture.get("path", defValue).asString();

            // Get the number of frames in the texture
            size_t numFrames = texture.get("frames", defValue).asUInt();

            // Add the texture to the texture map
            //auto lock = sys->makeCurrent();

            sys->loadTexture(alias, texPath, (numFrames > 0) ? numFrames : 1);
            //lock.unlock();
            //std::this_thread::yield();
          }
        }
        else
        {
          Log<Warning>("Error parsing texture definitions in file '%s'", defPath.c_str());
          throw(std::runtime_error("Error parsing texture definitions"));
        }
      }
    }
    else
    {
      Log<Error>("Failed to open texture definitions file '%s'", defPath.c_str());
      throw(std::runtime_error("Could not open texture definitions!"));
    }
  }
}
