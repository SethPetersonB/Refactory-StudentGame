// Primary Author : Kento Murawski
// 
// Co-authors:
//    Dylan Weber (Add/remove rows, columns, tiles)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include <iomanip>
#include <vector>
#include <map>
#include "GameInstance.h"
#include "Logger.h"
#include "Tile.h"
#include "Structure.h"
#include <set>

namespace Engine
{
  class Grid;
  class Stage;

  class Grid
  {
  public:
    Grid(); // default ctor for stages without grids
    Grid(Stage* stage, int width, int height);
    virtual ~Grid() {};

    void DrawGrid();
    std::vector<unsigned long>& operator[](int rhs);
    std::vector<unsigned long>& at(int rhs);
    
    void AddRow();
    void AddColumn();
    void RemoveTile(int x, int y);
    void AddTile(int x, int y);
    void RemoveRow();
    void RemoveColumn();

    bool CheckIfTileIsOccupied(int x, int y);
    bool CheckIfTileIsGrouped(int x, int y);
    bool CheckIfTileIsNotGrouped(int x, int y);
    bool CheckIfTileIsValid(int x, int y);
    unsigned char CheckAround(int x, int y);
    unsigned char CheckUngrouped(int x, int y);
    int PushTile(int x, int y, unsigned long ID);
    int PopTile(int x, int y);

    int GetGridWidth() const { return width_; }
    int GetGridHeight() const { return height_; }
    int GetCycles() const { return cycles_; }
    int GetGridIndex() const { return groups_.size(); }
    std::map<unsigned int, std::shared_ptr<Structure> > GetStructList() { return structList_; }
    std::set<int> GetGroupList() { return groups_; }
    Stage* GetStage() { return stage_; }
    int GetColumnOffset() const { return column_offset_; }
    int GetRowOffset() const { return row_offset_; }
    int GetNumBlocks() const { return num_blocks_; }

    void Group(int x, int y, int index);
    void ChooseNextPosition(int x, int y, unsigned char possibleMoves, int* newX, int* newY);
    void ResetGrid();
    void PrintGrid();
    std::set<int> FindModifiedGroups(int x, int y);
    void ParseGroups(const std::set<int>& tGroups);
    int FindModifiedGroupOnRemove(int x, int y);
    void ParseRemovedGroup(int group);
    void UpdateStructure(int group);

    void LoadStructureData(const std::string& defPath);

  private:
    friend class Tile;
    void CreateGrid(int width, int height);
    unsigned int CreateTile(int x, int y);
    void DrawTile(unsigned int Tile, int i, int j);
    void DrawRow(const std::vector<unsigned long>& vec);
    void DrawColumn(const std::vector<unsigned long>& vec);
    void GroupStructureFrom(int x, int y);
    void GroupStructures();
    void ParseGrid();
    void SetTileData();
    void ResetGroups();

    glm::vec2 GetFirstPosInstanceOfGroup(int group);
    int GetStructWidthFromGroup(int gX, int gY, int group);
    int GetStructHeightFromGroup(int gX, int gY, int group);
    void CopyStructFromGridFromTo(Structure& nStruct, int iX, int iY, int lX, int lY);
    std::vector<unsigned long> GetTilePtrsFromGroup(int group);
   
    
    void GroupUngroupedStructures();
    
    
    void ResetGroupInGrid(int group);
    void SetUngroupedStructureTo(int group);
    void SetUngroupedStructureAdditionTo(int group);
    
    void CreateStructure(int group);
    void RemoveStructure(int group);
    void CullStructures();

    int FindNextOpenGroupNumber();

    void ListGroups();
    void PrintStructures();

    int width_;
    int height_;
    int groupIndex_;
    int cycles_;
    Stage* stage_;
    std::set<int> groups_;
    std::map<unsigned int, std::shared_ptr<Structure> > structList_;
    std::vector<std::vector<unsigned long> > row_;
    int row_offset_;
    int column_offset_;
    int num_blocks_; // number of blocks on grid

  };
}
