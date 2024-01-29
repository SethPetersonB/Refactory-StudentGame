// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once
#include "GameInstance.h"
#include "glm/glm/glm.hpp"
#include "Logger.h"
#include <vector>

namespace Engine
{
  class Grid;
  class Stage;

  class Tile : public Component
  {
  public:
    Tile(GameInstance* owner);
    Tile(GameInstance* owner, Stage* stage, Grid* grid, int group, int type, int maxheight = 3);

    int GetGroup() const { return group_; }
    int GetType() const { return type_; }
    int GetHeight() const { return height_; }
    int GetMaxHeight() const { return maxheight_; }
    glm::vec2 GetXY() const { return glm::vec2(x_, y_); }
    glm::vec2 GetScale() const { return glm::vec2(scaleX_, scaleY_); }
    std::vector<unsigned long> GetObjects() const { return game_instance_ID_; }
    unsigned long operator[](int i) const { return game_instance_ID_[i]; }
    Stage* GetStage() { return stage_; }

    void ScaleRequest(Packet& data);
    void GroupRequest(Packet& data);
    void HeightRequest(Packet& data);
    void XYRequest(Packet& data);
    void MaxHeightRequest(Packet& data);
 

    void Push(unsigned long ID);
    void Pop();

    void SetGroup(int group) { group_ = group; }
    void SetType(int type) { type_ = type; }
    void SetHeight(int height);
    void SetMaxHeight(int maxheight);
    void SetTileXY(int x, int y) { x_ = x; y_ = y; }
    void SetStage(Stage* stage) { stage_ = stage; }
    void SetGrid(Grid* grid) { grid_ = grid; }

    void PushEvent(const Packet& payload);
    void TileClicked(const Packet& payload);
    void PopEvent(const Packet& payload);
    void MaxHeightEvent(const Packet& payload);
    void HeightEvent(const Packet& payload);
    void GroupEvent(const Packet& payload);
    void XYEvent(const Packet& payload);

    virtual ~Tile() {};

  private:
    Tile(const Tile &) = delete;
    Tile & operator=(const Tile &) = delete;

    int x_;
    int y_;
    int group_;
    int height_; // how many objects are on this tile
    int maxheight_;
    int type_;
    float scaleX_;
    float scaleY_;
    Stage* stage_;
    glm::vec4 bottom_color_; // color of bottom tile
    Grid* grid_; // tile should know what grid its on
    std::vector<unsigned long> game_instance_ID_;
  };

  class TileHandler : public ComponentHandler
  {
  public:
    TileHandler(Stage* stage);
    virtual ~TileHandler() {};
    void update();
    void getLuaRegisters() override;

  protected:
    void ConnectEvents(Component * base_sub);
  };
}
