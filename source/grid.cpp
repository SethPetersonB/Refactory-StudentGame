// Primary Author : Kento Murawski
// 
// Co-authors:
//    Dylan Weber (Add/remove rows, columns, tiles)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <fstream>
#include "../include/grid.h"
//#include <functional>
//#include "Structure.h"
#include "../include/GSM.h"
#include "../include/Stage.h"

static int cycles = 0;

static unsigned char northColl = 1 << 0;
static unsigned char  westColl = 1 << 1;
static unsigned char southColl = 1 << 2;
static unsigned char  eastColl = 1 << 3;

using namespace Logger;

namespace Engine
{
  Grid::Grid() : stage_(nullptr), width_(0), height_(0), cycles_(0), groupIndex_(0), row_offset_(0), column_offset_(0), num_blocks_(0)
  {

  }

  Grid::Grid(Stage* stage, int width, int height) : stage_(stage), row_offset_(0), column_offset_(0), num_blocks_(0)
  {
    width_ = width;
    height_ = height;
    groupIndex_ = 0;
    cycles_ = 0;
    row_.reserve(height);
    CreateGrid(width, height);
    ParseGrid();
  }

  std::vector<unsigned long>& Grid::operator[](int rhs) 
  {
    return row_[rhs];
  }

  std::vector<unsigned long>& Grid::at(int rhs) 
  {
    return row_.at(rhs);
  }

  void Grid::CreateGrid(int width, int height)
  {
    for (int j = 0; j < height; ++j)
    {
      std::vector<unsigned long> vec;
      vec.reserve(width);
      
      for (int i = 0; i < width; ++i)
      {
        unsigned int tile = CreateTile(j, i);
        vec.push_back(tile);
      }
      row_.push_back(vec);
    }
  }

  unsigned int Grid::CreateTile(int x, int y)
  {
    const std::string type = (y != 0) ? "Tile" : "Indicator";

    GameInstance& tile = stage_->addGameInstance(type);
    dynamic_cast<Tile*>(tile.getComponent("Tile"))->SetTileXY(x, y);
    return tile.getId();
  }

  void Grid::DrawTile(unsigned int Tile, int i, int j)
  {
    GameInstance& tile = stage_->getInstanceFromID(Tile);
    glm::vec2 scale = tile.RequestData<glm::vec2>("TileScale");
    // set width and height of tiles
    tile.PostMessage("ScaleSet", Message<glm::vec2>(scale));

    // offsets because position is set from the center of tile
    float offsetX = tile.RequestData<float>("Width") / 2.0f;
    float offsetY = tile.RequestData<float>("Height") / 2.0f;
    float x = offsetX * row_offset_;
    float y = offsetY * column_offset_;
    glm::vec2 pos((i - width_ / 2.0f) * scale.x + offsetX + x, (j - height_ / 2.0f)  * scale.y + offsetY + y);
    tile.PostMessage("PositionSet", Message<glm::vec2>(pos));
  }

  void Grid::DrawGrid()
  {
    // loop through every tile and draw them
    for (int j = 0; j < height_ ; ++j)
      for (int i = 0; i < width_; ++i)
        DrawTile(row_[j][i], i, j);
  }

  void Grid::DrawRow(const std::vector<unsigned long>& vec)
  {
    // draw top row
    for (int i = 0; i < width_; ++i)
      DrawTile(vec[i], i, height_);

    ++column_offset_;
  }

  void Grid::AddRow()
  {
    std::vector<unsigned long> vec;
    vec.reserve(width_);
    
    
    for (int i = 0; i < width_; ++i)
    {
      unsigned int tile = CreateTile(height_, i);
      vec.push_back(tile);
    }

    row_.push_back(vec);
    
    DrawRow(vec);
    ++height_;
   
  }

  void Grid::DrawColumn(const std::vector<unsigned long>& vec)
  {
    for (int j = 0; j < height_; ++j)
      DrawTile(vec[j], width_, j);

    ++row_offset_;
  }

  void Grid::AddColumn()
  {
    std::vector<unsigned long> vec;
    vec.reserve(height_);
    
    for (int i = 0; i < height_; ++i)
    {
      unsigned int tile = CreateTile(i, width_);
      vec.push_back(tile);
      row_[i].push_back(tile);
      
    }
    
    DrawColumn(vec);
    ++width_;
  }

  void Grid::RemoveTile(int x, int y)
  {
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
      return; // out of bounds

    if (x >= row_[y].size() || row_[y][x] == 0)
      return; // out of bounds

    GameInstance& tile = stage_->getInstanceFromID(row_[y][x]);
    int height = tile.RequestData<int>("TileHeight");
    for (int i = 0; i < height; ++i)
      tile.PostMessage("PopTile", Message<unsigned long>(tile.getId()));

    stage_->removeGameInstance(tile.getId());
    row_[y][x] = 0; // set it to zero, deleting it would mess up tile indices
 
  }

  // for now, this will just replace deleted tiles.
  // i doubt we will need to add random single tiles on their own outside the grid
  void Grid::AddTile(int x, int y)
  {
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
      return; // out of bounds
    
    if (x >= row_[y].size() || row_[y][x] != 0)
      return; // out of bounds
  
    unsigned int tile = CreateTile(x, y);
    DrawTile(tile, x, y);
    row_[y][x] = tile;
  }

  void Grid::RemoveRow()
  {
    if (height_ < 1)
      return;

    for (int i = 0; i < width_; ++i)
    {
      RemoveTile(i, height_ - 1);
    }
    row_.pop_back();
    --height_;
    --column_offset_;
  }

  void Grid::RemoveColumn()
  {
    if (width_ < 1)
      return;

    for (int i = 0; i < height_; ++i)
    {
      RemoveTile(width_ - 1, i);
      row_[i].pop_back();
    }
    --width_;
    --row_offset_;
  }

  bool Grid::CheckIfTileIsOccupied(int x, int y)
  {
    if (x >= width_ || y >= height_ || x < 0 || y < 0)
    {
      return true;
    }
    else
    {
      if (stage_->getInstanceFromID(row_[y][x]).RequestData<int>("TileGroup") != 0)
      {
        return true;
      }
    }

    return false;
  }

  bool Grid::CheckIfTileIsGrouped(int x, int y)
  {
    if (CheckIfTileIsOccupied(x, y))
    {
      if (stage_->getInstanceFromID(row_[y][x]).RequestData<int>("TileGroup") == 1)
      {
        return false;
      }
      else
      {
        return true;
      }
    }

    return false;
  }

  bool Grid::CheckIfTileIsNotGrouped(int x, int y)
  {
    if (x >= width_ || y >= height_ || x < 0 || y < 0)
    {
      return false;
    }
    else if (stage_->getInstanceFromID(row_[y][x]).RequestData<int>("TileGroup") == 1)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool Grid::CheckIfTileIsValid(int x, int y)
  {
    if (x >= width_ || y >= height_ || x < 0 || y < 0)
    {
      return false;
    }
    else
    {
      return CheckIfTileIsGrouped(x, y);
    }
  }

  unsigned char Grid::CheckAround(int x, int y)
  {
    unsigned char bitfield = 0;

    if (CheckIfTileIsValid(x, y + 1))
      bitfield |= northColl;
    if (CheckIfTileIsValid(x - 1, y))
      bitfield |= westColl;
    if (CheckIfTileIsValid(x, y - 1))
      bitfield |= southColl;
    if (CheckIfTileIsValid(x + 1, y))
      bitfield |= eastColl;

    return bitfield;
  }

  unsigned char Grid::CheckUngrouped(int x, int y)
  {
    unsigned char bitfield = 0;

    if (CheckIfTileIsNotGrouped(x, y + 1))
      bitfield |= northColl;
    if (CheckIfTileIsNotGrouped(x - 1, y))
      bitfield |= westColl;
    if (CheckIfTileIsNotGrouped(x, y - 1))
      bitfield |= southColl;
    if (CheckIfTileIsNotGrouped(x + 1, y))
      bitfield |= eastColl;

    cycles++;
    return bitfield;
  }

  int Grid::PushTile(int x, int y, unsigned long ID)
  {
    Log<Info>("PushTile Called");
    if (x < 0 || x >= width_ || y < 0 || y >= height_)
    {
      return 0;
    }
    else
    {
      GameInstance& tile = stage_->getInstanceFromID(row_[y][x]);
      tile.PostMessage("PushTile", Message<unsigned long>(ID));
      return 1;
    }
  }

  int Grid::PopTile(int x, int y)
  {
    if (x >= width_ || y >= height_ || x < 0 || y < 0)
    {
      return 0;
    }
    else if (!stage_->getInstanceFromID(row_[y][x]).RequestData<int>("TileHeight"))
    {
      return 0;
    }
    else
    {
      GameInstance& tile = stage_->getInstanceFromID(row_[y][x]);
      tile.PostMessage("PopTile", Message<bool>(true));

      return 1;
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Groups the structure given by x and y to the given index.
  Works when given any point within the ungrouped grid as input
  Uses a recursive backtracking maze generation algorithm to traverse the structure
  Catalogues how many cycles it took to parse the entire structure

  \param x
  An x value that is in the ungrouped structure

  \param y
  A y value that is in the ungrouped structure

  \param index
  The group to set the group to

  */
  /****************************************************************************/
  void Grid::Group(int x, int y, int index)
  {
    unsigned char PossibleMoves = CheckUngrouped(x, y);

    while (PossibleMoves)
    {
      int newX;
      int newY;

      stage_->getInstanceFromID(row_[y][x]).PostMessage("SetTileGroup", Message<int>(-1));
      ChooseNextPosition(x, y, PossibleMoves, &newX, &newY);
      Group(newX, newY, index);

      PossibleMoves = CheckUngrouped(x, y);
    }

    stage_->getInstanceFromID(row_[y][x]).PostMessage("SetTileGroup", Message<int>(index));
    cycles_ += cycles;
    cycles = 0;
    return;
  }

  /****************************************************************************/
  /*!
  \brief
  Calculates which direction to traverse to when given a list of possible moves

  \param x
  The current x location

  \param y
  The current y location

  \param possibleMoves
  The bitfield of possible directions to go

  \param newX
  The location to place the next position X

  \param newY
  The location to place the next position Y

  */
  /****************************************************************************/
  void Grid::ChooseNextPosition(int x, int y, unsigned char possibleMoves, int* newX, int* newY)
  {
    if (southColl & possibleMoves)
    {
      *newX = x;
      *newY = y - 1;
      return;
    }
    if (eastColl & possibleMoves)
    {
      *newX = x + 1;
      *newY = y;
      return;
    }
    if (northColl & possibleMoves)
    {
      *newX = x;
      *newY = y + 1;
      return;
    }
    if (westColl & possibleMoves)
    {
      *newX = x - 1;
      *newY = y;
      return;
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Resets the groupings and heights of every tile in the grid and clears
  the structlist and groups containers

  */
  /****************************************************************************/
  void Grid::ResetGrid()
  {
    groupIndex_ = 1;

    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        GameInstance& tile = stage_->getInstanceFromID(row_[i][j]);
        if (tile.RequestData<int>("TileGroup") > 1)
        {
          tile.PostMessage("SetTileGroup", Message<int>(0));
          tile.PostMessage("SetTileHeight", Message<int>(0));
        }
      }
    }

    structList_.clear();
    groups_.clear();
  }

  

  /****************************************************************************/
  /*!
  \brief
  Increments the group index and groups the structure from given position within
  the structure. OLD CODE DO NOT USE

  \param x
  x pos in grid

  \param y
  y pos in grid

  */
  /****************************************************************************/
  void Grid::GroupStructureFrom(int x, int y)
  {
    groupIndex_++;
    Group(x, y, groupIndex_);
  }

  /****************************************************************************/
  /*!
  \brief
  Finds every non-grouped structure and groups them into a group

  */
  /****************************************************************************/
  void Grid::GroupStructures()
  {
    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        if (CheckIfTileIsNotGrouped(j, i))
        {
          GroupStructureFrom(j, i);
        }
      }
    }
  }

  void Grid::ParseGrid()
  {
    cycles_ = 0;
    ResetGroups();
    SetTileData();
    GroupStructures();
  }

  /****************************************************************************/
  /*!
  \brief
  Sets every tile's XY data so it knows where it is in the grid

  */
  /****************************************************************************/
  void Grid::SetTileData()
  {
    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        stage_->getInstanceFromID(row_[i][j]).PostMessage("SetTilePos", Message<glm::vec2>(glm::vec2(j, i)));
      }
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Resets every group in the grid
  Sets every grouped value's group to 1(ungrouped)

  */
  /****************************************************************************/
  void Grid::ResetGroups()
  {
    groupIndex_ = 1;

    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        GameInstance& tile = stage_->getInstanceFromID(row_[i][j]);
        if (tile.RequestData<int>("TileGroup") > 1)
        {
          tile.PostMessage("SetTileGroup", Message<int>(1));
        }
      }
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Finds the first grid position instance of the given group

  \return
  The vec2 of grid position x and y where the first instance of the group is

  \param group
  The group to find the first instance of

  */
  /****************************************************************************/
  glm::vec2 Grid::GetFirstPosInstanceOfGroup(int group)
  {
    bool foundTop = false;
    int minX = width_;
    int maxY = -1;

    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        if (stage_->getInstanceFromID(row_[i][j]).RequestData<int>("TileGroup") == group)
        {
          if (!foundTop)
          {
            maxY = i;
            foundTop = true;
          }

          if (j < minX)
          {
            minX = j;
          }
        }
      }
    }

    return glm::vec2(minX, maxY);
  }

  /****************************************************************************/
  /*!
  \brief
  Gets the width of the structure in the grid

  \return
  The width of the structure

  \param gX
  The first x position of the structure

  \param gY
  The first y position of the structure

  \param group
  The group to find the width of

  */
  /****************************************************************************/
  int Grid::GetStructWidthFromGroup(int gX, int gY, int group)
  {
    int maxX = -1;

    for (int i = gY; i < height_; ++i)
    {
      for (int j = gX; j < width_; ++j)
      {
        if (stage_->getInstanceFromID(row_[i][j]).RequestData<int>("TileGroup") == group)
        {
          if (j > maxX)
          {
            maxX = j;
          }
        }
      }
    }

    return maxX - gX + 1;
  }

  /****************************************************************************/
  /*!
  \brief
  Gets the height of the structure in the grid

  \return
  The height of the structure

  \param gX
  The first x position of the structure

  \param gY
  The first y position of the structure

  \param group
  The group to find the height of

  */
  /****************************************************************************/
  int Grid::GetStructHeightFromGroup(int gX, int gY, int group)
  {
    int maxY = -1;

    for (int i = gY; i < height_; ++i)
    {
      for (int j = gX; j < width_; ++j)
      {
        if (stage_->getInstanceFromID(row_[i][j]).RequestData<int>("TileGroup") == group)
        {
          if (i > maxY)
          {
            maxY = i;
          }
        }
      }
    }

    return maxY - gY + 1;
  }

  /****************************************************************************/
  /*!
  \brief
  Copies over the height values from the grid onto the given struct by giving the
  topleft and bottomright xy positions in the grid

  \param nStruct
  The structure to copy to

  \param iX
  The topleft X

  \param iY
  The topleft Y

  \param lX
  The bottomright X

  \param lY
  The bottomright Y

  */
  /****************************************************************************/
  void Grid::CopyStructFromGridFromTo(Structure& nStruct, int iX, int iY, int lX, int lY)
  {
    for (int i = iY, k = 0; i < lY, k < nStruct.GetY(); ++i, ++k)
    {
      for (int j = iX, l = 0; j < lX, l < nStruct.GetX(); ++j, ++l)
      {
        nStruct.GetGrid()[k][l]->SetHeight(stage_->getInstanceFromID(row_[i][j]).RequestData<int>("TileHeight"));
      }
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Gets all the tiles from a given group and returns them as a vector

  \return
  the vector of tiles that correspond to the structure

  \param group
  The group to gather the tiles for

  */
  /****************************************************************************/
  std::vector<unsigned long> Grid::GetTilePtrsFromGroup(int group)
  {
    std::vector<unsigned long> children;

    for (int i = 0; i < width_; ++i)
    {
      for (int j = 0; j < height_; ++j)
      {
        if (stage_->getInstanceFromID(row_[j][i]).RequestData<int>("TileGroup") == group)
        {
          children.push_back(row_[j][i]);
        }
      }
    }

    return children;
  }

  std::set<int> Grid::FindModifiedGroups(int x, int y)
  {
    std::set<int> tGroups;
    int CurrGroup = stage_->getInstanceFromID(row_[y][x]).RequestData<int>("TileGroup");

    if (CurrGroup > 1)
    {
      tGroups.insert(CurrGroup);
    }
    else
    {
      int nGroup = 0, wGroup = 0, sGroup = 0, eGroup = 0;

      if (CheckIfTileIsValid(x, y + 1))
        nGroup = stage_->getInstanceFromID(row_[y + 1][x]).RequestData<int>("TileGroup");
      if (CheckIfTileIsValid(x - 1, y))
        wGroup = stage_->getInstanceFromID(row_[y][x - 1]).RequestData<int>("TileGroup");
      if (CheckIfTileIsValid(x, y - 1))
        sGroup = stage_->getInstanceFromID(row_[y - 1][x]).RequestData<int>("TileGroup");
      if (CheckIfTileIsValid(x + 1, y))
        eGroup = stage_->getInstanceFromID(row_[y][x + 1]).RequestData<int>("TileGroup");

      if (nGroup)
      {
        tGroups.insert(nGroup);
      }
      if (wGroup)
      {
        tGroups.insert(wGroup);
      }
      if (sGroup)
      {
        tGroups.insert(sGroup);
      }
      if (eGroup)
      {
        tGroups.insert(eGroup);
      }

      tGroups.erase(0);
    }

    return tGroups;
  }

  int Grid::FindModifiedGroupOnRemove(int x, int y)
  {
    //if (stage_->getInstanceFromID(row_[y][x]).RequestData<int>("TileHeight") == 0)
    //  return 0;

    std::set<int> t = FindModifiedGroups(x, y);

    if (t.size())
      return *(t.begin());
    else
      return 0;
  }

  /****************************************************************************/
  /*!
  \brief
  Finds every ungrouped structure in the grid and groups them into separate
  structures

  */
  /****************************************************************************/
  void Grid::GroupUngroupedStructures()
  {
    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        if (stage_->getInstanceFromID(row_[i][j]).RequestData<int>("TileGroup") == 1)
        {
          int group = FindNextOpenGroupNumber();
          Group(j, i, group);
          CullStructures();
          CreateStructure(group);
        }
      }
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Parses the list of given groups once tile has been added. Much faster than
  reparsing entire grid since it only affects modified groups. Also keeps old
  grouping values so modifying structures is much easier.

  \param tGroups
  The set of groups that are being modified

  */
  /****************************************************************************/
  void Grid::ParseGroups(const std::set<int>& tGroups)
  {
    if (tGroups.size() > 1)
    {
      int GroupToMergeInto = *(tGroups.begin());

      for (auto& GroupsIter : tGroups)
      {
        ResetGroupInGrid(GroupsIter);
      }

      SetUngroupedStructureTo(GroupToMergeInto);

      for (auto iter = tGroups.begin(); iter != tGroups.end(); ++iter)
      {
        if (*iter == GroupToMergeInto)
        {

        }
        else
        {
          groups_.erase(*iter);
        }
      }

      CullStructures();
      UpdateStructure(GroupToMergeInto);
    }
    else if (tGroups.size() == 1)
    {
      SetUngroupedStructureAdditionTo(*(tGroups.begin()));
      UpdateStructure(*(tGroups.begin()));
    }
    else
    {
      int newGroupIndex = FindNextOpenGroupNumber();

      SetUngroupedStructureTo(newGroupIndex);
      groups_.insert(newGroupIndex);
      CreateStructure(newGroupIndex);
    }
  }

  void Grid::ParseRemovedGroup(int group)
  {
    ResetGroupInGrid(group);
    structList_.erase(group);
    groups_.erase(group);
    GroupUngroupedStructures();
  }

  /****************************************************************************/
  /*!
  \brief
  Resets the given group in the grid by setting every group value to 1

  \param group
  The group to reset

  */
  /****************************************************************************/
  void Grid::ResetGroupInGrid(int group)
  {
    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        GameInstance& tile = stage_->getInstanceFromID(row_[i][j]);
        if (tile.RequestData<int>("TileGroup") == group)
        {
          tile.PostMessage("SetTileGroup", Message<int>(1));
        }
      }
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Finds the first ungrouped grid and groups it to the given group number

  \param group
  The group to set the first ungrouped structure to

  */
  /****************************************************************************/
  void Grid::SetUngroupedStructureTo(int group)
  {
    //Log<Info>("SetUngroupedStructureTo");

    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        if (CheckIfTileIsNotGrouped(j, i))
        {
          Group(j, i, group);
          return;
        }
      }
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Finds the first ungrouped grid and groups it to the given group number

  \param group
  The group to set the first ungrouped structure to

  */
  /****************************************************************************/
  void Grid::SetUngroupedStructureAdditionTo(int group)
  {
    //Log<Info>("SetUngroupedStructureAdditionTo");

    for (int i = 0; i < height_; ++i)
    {
      for (int j = 0; j < width_; ++j)
      {
        if (CheckIfTileIsNotGrouped(j, i))
        {
          stage_->getInstanceFromID(row_[i][j]).PostMessage("SetTileGroup", Message<int>(group));
          return;
        }
      }
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Updates the given group's structure to add more tiles and reparse

  \param group
  The group to turn into a structure

  */
  /****************************************************************************/
  void Grid::UpdateStructure(int group)
  {
    glm::vec2 firstPos = GetFirstPosInstanceOfGroup(group);

    if (int(firstPos.x) == -1 || int(firstPos.y) == -1)
    {
    }
    else
    {
      int structWidth = GetStructWidthFromGroup(static_cast<int>(firstPos.x), static_cast<int>(firstPos.y), group);
      int structHeight = GetStructHeightFromGroup(static_cast<int>(firstPos.x), static_cast<int>(firstPos.y), group);
      std::vector<unsigned long> children = GetTilePtrsFromGroup(group);

      auto ent = structList_.at(group);

      ent->GetEntity()->PostMessage<std::string>("UnloadStructure", "");

      if( structList_.erase(group))
      {
        std::shared_ptr<Structure> nStructPtr = std::make_shared<Structure>(group, children);

        //Log<Info>("Struct %d Width: %d, Height: %d", group, structWidth, structHeight);

        nStructPtr->SetWidth(structWidth);
        nStructPtr->SetHeight(structHeight);
        nStructPtr->SetStage(stage_);

        CopyStructFromGridFromTo(*nStructPtr, static_cast<int>(firstPos.x), static_cast<int>(firstPos.y),
          static_cast<int>(firstPos.x + structWidth), static_cast<int>(firstPos.y + structHeight));

        nStructPtr->SetStructureEntity();
      
        structList_.insert(std::make_pair(group, nStructPtr));
      }
      else
      {
        Log<Error>("Failed to delete old structure instance!");
      }
    }
  }


 void Grid::CreateStructure(int group)
  {
    glm::vec2 firstPos = GetFirstPosInstanceOfGroup(group);

    if (int(firstPos.x) == -1 || int(firstPos.y) == -1)
    {
    }
    else
    {
      int structWidth = GetStructWidthFromGroup(static_cast<int>(firstPos.x), static_cast<int>(firstPos.y), group);
      int structHeight = GetStructHeightFromGroup(static_cast<int>(firstPos.x), static_cast<int>(firstPos.y), group);
      std::vector<unsigned long> children = GetTilePtrsFromGroup(group);
      std::shared_ptr<Structure> nStructPtr(new Structure(group, children));

     // Log<Info>("Struct %d Width: %d, Height: %d", group, structWidth, structHeight);

      nStructPtr->SetWidth(structWidth);
      nStructPtr->SetHeight(structHeight);
      nStructPtr->SetStage(stage_);

      CopyStructFromGridFromTo(*nStructPtr, static_cast<int>(firstPos.x), static_cast<int>(firstPos.y),
        static_cast<int>(firstPos.x + structWidth), static_cast<int>(firstPos.y + structHeight));

      nStructPtr->SetStructureEntity();

      structList_.insert(std::make_pair(group, nStructPtr));
      groups_.insert(group);
    }
  }

  void Grid::RemoveStructure(int group)
  {
    CullStructures();
  }

  /****************************************************************************/
  /*!
  \brief
  Gets rid of no longer needed structures from the structure list

  */
  /****************************************************************************/
  void Grid::CullStructures()
  {
    for (auto structIter = structList_.begin(); structIter != structList_.end(); )
    {
      auto groupIter = std::find(groups_.begin(), groups_.end(), structIter->first);
      if (groupIter != groups_.end())
      {
        ++structIter;
      }
      else
      {
        structIter = structList_.erase(structIter);
      }
    }
  }


 /****************************************************************************/
 /*!
 \brief
 Finds the next open group number that is not bound to a structure

 \return
 the group value that's open

 */
 /****************************************************************************/
  int Grid::FindNextOpenGroupNumber()
  {
    if (groups_.size() == 0)
      return 2;

    int lastGroup = 1;

    for (auto& groupsIter : groups_)
    {
      //Log<Info>("Last Group = %d\nCurrent Group Index = %d", lastGroup, groupsIter);

      if (groupsIter != lastGroup + 1)
      {
        return lastGroup + 1;
      }

      lastGroup = groupsIter;
    }

    return lastGroup + 1;
  }

  /****************************************************************************/
  /*!
  \brief
  Prints every group

  */
  /****************************************************************************/
  void Grid::ListGroups()
  {
    int i = 0;
    for (auto& groupsIter : groups_)
    {
      Log<Info>("Group #%d: %d", ++i, groupsIter);
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Prints every structure in the grid to the log stream

  */
  /****************************************************************************/
  void Grid::PrintStructures()
  {
    for (auto& StructIter : structList_)
    {
      StructIter.second->PrintStructure();
    }
  }

  /****************************************************************************/
  /*!
  \brief
  Prints the grid to Log stream

  */
  /****************************************************************************/
  void Grid::PrintGrid()
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
        stream << std::setw(2) << stage_->getInstanceFromID(row_[i][j]).RequestData<int>("TileGroup") << " ";
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
  Loads the structure data from the given JSON file and loads and adds each
  structure in the path into memory for later comparisons during building

  \param defPath
  The path to the JSON file to read all structures from

  */
  /****************************************************************************/
  void Grid::LoadStructureData(const std::string& defPath)
  {

    //if (loaded == false)
    {
      std::ifstream structDefs(defPath);
      Json::Reader reader;
      Json::Value root;
      const Json::Value defValue;

      if (structDefs.is_open())
      {
        reader.parse(structDefs, root);

        std::vector<std::string> structFolders = root.getMemberNames();

        // get each folder
        for (auto & folderPath : structFolders)
        {
          Json::Value folder = root.get(folderPath, defValue);

          if (folder != defValue)
          {
            // Get each structure path in the folder
            std::vector<std::string> structureDefs = folder.getMemberNames();

            for (auto & structDef : structureDefs)
            {
              Json::Value structure = folder.get(structDef, defValue);

              // error
              if (structure == defValue)
              {
                Log<Warning>("Error parsing structure definitions in file '%s'", defPath.c_str());
                throw(std::runtime_error("Error parsing structure definitions"));
              }

              // Get the alias of the structure
              std::string & alias = structDef;

              // Get the relative path of the structure
              std::string structPath = folderPath + structure.get("path", defValue).asString();

              // Add the texture to the texture map
              Stage* objStage = stage_;

              GameInstance* gameObject = objStage->getMessenger().Request<GameInstance*>("PlayerController");

              STRUCT_LIST * LSL = gameObject->RequestData<STRUCT_LIST*>("GetParsedStructureList");

              (*LSL)[alias] = Structure(structPath);
            }
          }
          else
          {
            Log<Warning>("Error parsing structure definitions in file '%s'", defPath.c_str());
            throw(std::runtime_error("Error parsing structure definitions"));
          }
        }
      }
    }
    /*
    else
    {
    Log<Error>("Failed to open structure definitions file '%s'", defPath.c_str());
    throw(std::runtime_error("Could not open structure definitions!"));
    }
    */
  }

}

