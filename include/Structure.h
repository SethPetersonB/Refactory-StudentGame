// Primary Author : Kento Murawski
// 
// Co-authors:
//    Dylan Weber (StructureTile class)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include <vector>
#include "Logger.h"
#include <map>
#include "glm/glm/glm.hpp"

namespace Engine
{
  class Grid;
  class Stage;

  // proxy tile class for structures
  class StructureTile
  {
  public:
    StructureTile();
    StructureTile(int group, int type, int height);

    int GetGroup() const { return _group; }
    int GetType() const { return _type; }
    int GetHeight() const { return _height; }
    glm::vec2 GetXY() const { return glm::vec2(_x, _y); }

    void SetGroup(int group) { _group = group; }
    void SetType(int type) { _type = type; }
    void SetHeight(int height) { _height = height; }
    void SetTileXY(int x, int y) { _x = x; _y = y; }

    virtual ~StructureTile() {};
  
  private:
    StructureTile(const StructureTile &) = delete;
    StructureTile & operator=(const StructureTile &) = delete;

    int _x;
    int _y;
    int _group;
    int _height;
    int _type;
   
  };

 
  class Structure 
  {
  public:
    Structure();
    Structure(int group, const std::vector<unsigned long>& childTiles);
    Structure(const std::string& path);

    int GetX() const { return width_; }
    int GetY() const { return height_; }
    int GetZ() const { return zMax_; }
    int GetGroup() const { return group_; }
    std::vector<std::vector<std::shared_ptr<StructureTile> > >& GetGrid() { return grid_; }
    GameInstance* GetEntity() const;

    void SetWidth(int width);
    void SetHeight(int height);
    void SetEntity(GameInstance * ent);
    void SetStage(Stage* stage) { stage_ = stage; }

    bool operator==(const Structure& rhs) const;
    bool operator<(const Structure& rhs) const;

    void PrintStructure();
    void PrintStructure() const;

    bool CompareGrids(const Structure& rhs) const;
    void SetStructureEntity();

    void SetArt();

    virtual ~Structure();
  private:

    void LoadStructureFromPath(const std::string& path);
    int height_;
    int width_;
    int zMax_;

    int type_;
    int group_;

    unsigned long structEntity_;
    std::vector<std::vector<std::shared_ptr<StructureTile> > > grid_;
    Stage* stage_;
    std::vector<unsigned long> childTiles_;
  };

  typedef std::unordered_map<std::string, Structure> STRUCT_LIST;
  typedef std::vector<std::pair<std::pair<int, int>, std::vector<GameInstance*>>> OBJECT_BY_TILE_LIST;
 
}
