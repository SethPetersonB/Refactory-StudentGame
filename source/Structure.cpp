// Primary Author : Kento Murawski
// 
// Co-authors:
//    Dylan Weber (StructureTile class)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Structure.h"
#include "../include/Stage.h"
#include <fstream>

namespace Engine
{
  using namespace Logger;

  StructureTile::StructureTile() : _x(0), _y(0), _group(0), _type(0), _height(0)
  {

  }

  StructureTile::StructureTile(int group, int type = 0, int height = 0) 
    : _x(0), _y(0), _group(group), _type(type), _height(height)
  {

  }

  /****************************************************************************/
  /*!
  \brief
  Default Structure constructor

  */
  /****************************************************************************/
  Structure::Structure() : height_(1), width_(1), zMax_(1), type_(0), group_(0), 
    structEntity_(0)
  {
    std::vector<std::shared_ptr<StructureTile> > row;
    row.push_back(std::shared_ptr<StructureTile>(new StructureTile(1, 0, 1)));
    grid_.push_back(row);
  }

  /****************************************************************************/
  /*!
  \brief
  Conversion Structure constructor

  \param group
  group of Structure

  \param childTiles
  vector of tiles that correspond to the Structure

  */
  /****************************************************************************/
  Structure::Structure(int group, const std::vector<unsigned long>& childTiles)
  : height_(1), width_(1), zMax_(1), type_(0), group_(group), structEntity_(0), 
    childTiles_(childTiles)
  {
    std::vector<std::shared_ptr<StructureTile> > row;
    row.push_back(std::shared_ptr<StructureTile>(new StructureTile(1, 0, 1)));
    grid_.push_back(row);
    //stage_ = Stage::GetStage("TestStage1"); // not dynamic
  }

  /****************************************************************************/
  /*!
  \brief
  Conversion Structure constructor

  \param path
  the .strct file to load the Structure from

  */
  /****************************************************************************/
  Structure::Structure(const std::string& path) : 
   height_(1), width_(1), type_(0), group_(-1), structEntity_(0)
  {
    std::vector<std::shared_ptr<StructureTile> > row;
    row.push_back(std::shared_ptr<StructureTile>(new StructureTile(1, 0, 1)));
    grid_.push_back(row);
    LoadStructureFromPath(path);
  }

  /****************************************************************************/
  /*!
  \brief
  Structure deconstructor - May be memory leak, check later

  */
  /****************************************************************************/
  Structure::~Structure()
  {
    if (structEntity_ != 0)
    {
      try
      {
        stage_->removeGameInstance(structEntity_);
      }
      catch (const std::out_of_range &) {}
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Loads the Structure from the given path (should be a .strct file)

  \param path
  The .strct file to load the Structure from

  */
  /****************************************************************************/
  void Structure::LoadStructureFromPath(const std::string& path)
  {

    std::ifstream sFile;
    int width, height;
    int maxHeight = 0;

    sFile.open(path);

    if (sFile.is_open() == false)
      return;

    sFile >> width;
    sFile >> height;

    SetWidth(width);
    SetHeight(height);


    for (int col = 0; col < height_; col++)
    {
      for (int row = 0; row < width_; row++)
      {
        int _height;
        sFile >> _height;
        grid_[col][row]->SetHeight(_height);

        if (_height > maxHeight)
          maxHeight = _height;
      }
    }

    //PrintStructure();
  }

  /****************************************************************************/
  /*!
  \brief
  Sets the gameinstance of the Structure

  \param ent
  The entity to point to

  */
  /****************************************************************************/
  void Structure::SetEntity(GameInstance * ent)
  {
    structEntity_ = ent->getId();
  }

  /****************************************************************************/
  /*!
  \brief
  Prints the Structure to the log stream

  */
  /****************************************************************************/
  void Structure::PrintStructure()
  {
    Log<Info>("Grid:");
    Log<Info>("Width: %d", width_);
    Log<Info>("Height: %d", height_);
    Log<Info>("---");

    std::stringstream stream;
    stream << std::endl;

    for (int i = 0; i < width_; ++i)
    {
      stream << std::setw(2) << i << " ";
    }
    stream << std::endl;

    for (int i = 0; i < width_; ++i)
    {
      stream << "---";
    }
    stream << std::endl;

    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        stream << std::setw(2) << grid_[i][j]->GetHeight() << " ";
      }

      stream << std::endl;
    }

    for (int i = 0; i < width_; ++i)
    {
      stream << "---";
    }

    Log<Info>("%s", stream.str().c_str());
  }

  /****************************************************************************/
  /*!
  \brief
  Prints the Structure to the log stream

  */
  /****************************************************************************/
  void Structure::PrintStructure() const
  {
    Log<Info>("Grid:");
    Log<Info>("Width: %d", width_);
    Log<Info>("Height: %d", height_);
    Log<Info>("---");

    std::stringstream stream;
    stream << std::endl;

    for (int i = 0; i < width_; ++i)
    {
      stream << std::setw(2) << i << " ";
    }
    stream << std::endl;

    for (int i = 0; i < width_; ++i)
    {
      stream << "---";
    }
    stream << std::endl;

    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        stream << std::setw(2) << grid_[i][j]->GetHeight() << " ";
      }

      stream << std::endl;
    }

    for (int i = 0; i < width_; ++i)
    {
      stream << "---";
    }

    Log<Info>("%s", stream.str().c_str());
  }

  /****************************************************************************/
  /*!
  \brief
  Checks equality between Structures

  \return
  true -  Structures are identical
  false - Structures are not identical

  \param rhs
  The Structure to check equality against

  */
  /****************************************************************************/
  bool Structure::operator==(const Structure& rhs) const
  {
    if (group_ == rhs.group_)
    {
      if (width_ == rhs.width_ && height_ == rhs.height_)
      {
        for (int i = 0; i < height_; ++i)
        {
          for (int j = 0; j < width_; ++j)
          {
            if (grid_[i][j]->GetHeight() != rhs.grid_[i][j]->GetHeight())
            {
              Log<Info>("Mismatch at tile (%d, %d)", j, i);
              return false;
            }
          }
        }

        return true;
      }
    }

    return false;
  }

  /****************************************************************************/
  /*!
  \brief
  Compares < operator for groups for Structure use in sets

  \return
  true -  this Structure is less than given Structure
  false - this Structure is more than given Structure

  \param rhs
  The Structure to check less than against

  */
  /****************************************************************************/
  bool Structure::operator<(const Structure& rhs) const
  {
    if (group_ < rhs.group_)
      return true;
    else
      return false;
  }

  /****************************************************************************/
  /*!
  \brief
  Sets the width of the Structure grid

  \param width
  The width to set to

  */
  /****************************************************************************/
  void Structure::SetWidth(int width)
  {
    if (width_ < width)
    {
      for (int j = 0; j < height_; ++j)
      {
        for (int i = width_; i < width; ++i)
        {
          grid_[j].push_back(std::shared_ptr<StructureTile>(new StructureTile(0, 0, 0)));
        }
      }
    }
    else if (width_ > width)
    {
      for (int j = 0; j < height_; ++j)
      {
        for (int i = width_; i > width; --i)
        {
          grid_[j].pop_back();
        }
      }
    }

    width_ = width;
  }

  /****************************************************************************/
  /*!
  \brief
  Sets the height of the Structure grid

  \param height
  The height to set to

  */
  /****************************************************************************/
  void Structure::SetHeight(int height)
  {
    if (height_ < height)
    {
      for (int i = height_; i < height; ++i)
      {
        std::vector<std::shared_ptr<StructureTile> > row;

        for (int i = 0; i < width_; i++)
          row.push_back(std::shared_ptr<StructureTile>(new StructureTile(0, 0)));

        grid_.push_back(row);

      }
    }
    else if (height_ > height)
    {
      for (int i = height_; i > height; --i)
      {
        grid_.pop_back();
      }
    }

    height_ = height;
  }

  /****************************************************************************/
  /*!
  \brief
  Compares the current Structure with the given Structure ignoring group value

  \return
  true -  Structures are identical
  false - Structures are not identical

  \param rhs
  The Structure to check equality against

  */
  /****************************************************************************/
  bool Structure::CompareGrids(const Structure& rhs) const
  {
   // Log<Info>("Comparing Structures %d and %d", group_, rhs.group_);

    if (width_ == rhs.width_ && height_ == rhs.height_)
    {
      for (int i = 0; i < height_; ++i)
      {
        for (int j = 0; j < width_; ++j)
        {
          if (grid_[i][j]->GetHeight() != rhs.grid_[i][j]->GetHeight())
          {
            return false;
          }
        }
      }

      return true;
    }

    return false;
  }

  /****************************************************************************/
  /*!
  \brief
  Compares the Structure with the list of possible predetermined Structures and
  creates a game instance and sets the game instance accordingly

  */
  /****************************************************************************/
  void Structure::SetStructureEntity()
  {
    GameInstance* gameObject = stage_->getMessenger().Request<GameInstance*>("PlayerController");

    std::unordered_map<std::string, Structure>* LSL = gameObject->RequestData<std::unordered_map<std::string, Structure>*>("GetParsedStructureList");

    //Log<Info>("Total number of loaded Structures at time of comparison: %d", LSL->size());

    GameInstance & newObj = stage_->addGameInstance("Structure");

    structEntity_ = newObj.getId();

    for (auto& PossibleStructIter : *LSL)
    {
      if (CompareGrids(PossibleStructIter.second))
      {
        // Oddly convoluted process to get entities from the structure
        for (auto & tileId : childTiles_)
        {
          GameInstance& tileObj = stage_->getInstanceFromID(tileId);
          auto instances = tileObj.RequestData <std::vector<unsigned long>>("TileEntities");

          for (auto inst : instances)
          {
            if (inst != tileId) // Why is the tile itself part of the stack?
            {
              GameInstance& obj = stage_->getInstanceFromID(inst);
              newObj.PostMessage("AddStructureInstance", inst);
            }
          }
        }

        newObj.PostMessage("LoadStructure", PossibleStructIter.first);
        break;
      }
    }

    SetArt();

    if (structEntity_ != 0)
    {
     // Log<Info>("Successfully set Structure to type %s", structEntity_->getObjectType().c_str());
    }
    else
    {
     // Log<Info>("Unknown Structure type");
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Sets the texture of the current Structure's blocks

  */
  /****************************************************************************/
  void Structure::SetArt()
  {
    // Doesn't do anything anymore
  }

  GameInstance* Structure::GetEntity() const
  { 
    return &stage_->getInstanceFromID(structEntity_); 
  }


}
