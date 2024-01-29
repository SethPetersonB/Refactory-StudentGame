// Primary Author : Dylan Weber
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/Tile.h"
#include "../include/audio_test.h"
#include "../include/audio_startup.h"
#include "../include/Stage.h"
#include "../include/GSM.h"
#include "../include/BuildLogic.h"
#include "../include/Particles.h"

namespace Engine
{
  using namespace Logger;

  Tile::Tile(GameInstance* owner) : Component(owner, "Tile")
  {
    x_ = 0;
    y_ = 0;
    group_ = 0;
    height_ = 0;
    maxheight_ = 100;
    type_ = 0;
    scaleX_ = 75;
    scaleY_ = 75;
    stage_ = owner->getStage();
    grid_ = &owner->getStage()->GetGrid();
    bottom_color_ = glm::vec4(255, 255, 255, 255);

    // can only have as many objects on tile as max height
    game_instance_ID_.reserve(maxheight_);
    game_instance_ID_.push_back(owner->getId());

  }

  Tile::Tile(GameInstance* owner, Stage* stage, Grid* grid, int group, int type, int maxheight) : Component(owner, "Tile")
  {
    x_ = 0;
    y_ = 0;
    group_ = group;
    type_ = type;
    height_ = 0;
    scaleX_ = 75;
    scaleY_ = 75;
    maxheight_ = maxheight;
    stage_ = stage;
    grid_ = grid;
    bottom_color_ = glm::vec4(255, 255, 255, 255);

    // can only have as many objects on tile as max height
    game_instance_ID_.reserve(maxheight_);
    game_instance_ID_.push_back(owner->getId());

  }

  void Tile::Push(unsigned long ID)
  {
    if (height_ >= maxheight_)
    {
      stage_->removeGameInstance(ID);
      return;
      // handle error, can't push object beyond max height
    }

    GameInstance& obj = stage_->getInstanceFromID(ID);
    GameInstance* pc = stage_->getMessenger().Request<GameInstance*>("PlayerController");

    if (height_ == 0) // if first tile being built
    {
      DrawToken item = obj.RequestData<DrawToken>("Graphic");
      glm::vec4 color = item.getShade();
      bottom_color_ = color;
    }

    if (!pc->RequestData<bool>("IsGod") && pc->RequestData<int>("AvailableWalls") <= 0)
    {
      stage_->removeGameInstance(ID);
      GetAudioEngine()->PlaySounds("ResourceEmpty.wav", Vector3(), -18.0f);
      return; // if out of blocks, build nothing
    }

    if (group_ == 0)
      group_ = 1;

    game_instance_ID_.push_back(ID);

    pc->PostMessage("PlaceWall", Message<bool>(true)); // send event to decrement available walls
    obj.PostMessage("TransformDepthSet", Message<float>(static_cast<float>(height_++))); // set depth of structure
    glm::vec2 pos = getParent().RequestData<glm::vec2>("Position");
    obj.PostMessage("PositionSet", Message<glm::vec2>(pos)); // set position of structure
    obj.PostMessage("ScaleSet", Message<glm::vec2>(glm::vec2(scaleX_, scaleY_))); // set scale of structure

    Audio_Engine* AEngine = GetAudioEngine();

    DrawToken item = obj.RequestData<DrawToken>("Graphic");
    glm::vec4 color = item.getShade();

    /* Placement sound. */
    AEngine->PlaySounds("place_block.wav", Vector3(), -12.0f);

    // suscribe to clicked event
    REQUEST_ACTION getxy = std::bind(&Tile::XYRequest, this, std::placeholders::_1);
    obj.getMessenger().SetupRequest("TilePos", getxy);

    REQUEST_ACTION getheight = std::bind(&Tile::HeightRequest, this, std::placeholders::_1);
    obj.getMessenger().SetupRequest("TileHeight", getheight);

    SUBSCRIBER_ACTION clickFace = std::bind(Logic::OnBlockClicked, &obj, std::placeholders::_1);
    obj.getMessenger().Subscribe(obj.getMessenger(), "Clicked", clickFace);

    obj.getMessenger().Subscribe(obj.getMessenger(), "RightClicked", [this](const Packet& payload) {this->Pop();});

    SUBSCRIBER_ACTION pop = std::bind(&Tile::PopEvent, this, std::placeholders::_1);
    obj.getMessenger().Subscribe(obj.getMessenger(), "PopTile", pop);

    obj.PostMessage("CreateParticles", Message<std::string>("spiral")); // create particles when structure is built

    std::set<int> tGroups = grid_->FindModifiedGroups(x_, y_);
    grid_->ParseGroups(tGroups);
    ++grid_->num_blocks_;
  }

  void Tile::Pop()
  {
    if (height_ <= 0)
    {
      // error handling
      return;
    }

    // change color of bottom block to normal and delete top block
    if (height_ > 1)
    {
      int MaxHp = stage_->getInstanceFromID(game_instance_ID_[1]).RequestData<int>("MaxHP");
      stage_->getInstanceFromID(game_instance_ID_[1]).PostMessage("HpSet", Message<int>(MaxHp));
      DrawToken item = stage_->getInstanceFromID(game_instance_ID_[1]).RequestData<DrawToken>("Graphic");
      item.setShade(bottom_color_); 
    }
    int ModifiedGroup = grid_->FindModifiedGroupOnRemove(x_, y_);

    stage_->getInstanceFromID(game_instance_ID_[1]).PostMessage("CreateParticles", Message<std::string>("explosion"));
    stage_->removeGameInstance(game_instance_ID_.back());

    game_instance_ID_.pop_back();
    --height_;

    if (height_ == 0)
      group_ = 0;

    if (ModifiedGroup)
    {
      grid_->ParseRemovedGroup(ModifiedGroup);
    }
    else
    {
      grid_->UpdateStructure(ModifiedGroup);
    }
    --grid_->num_blocks_;
  }

  void Tile::SetHeight(int height)
  {
    height_ = height;
    if (maxheight_ < height_)
      maxheight_ = height_;

    while (height_ > height)
      Pop();
  }

  void Tile::SetMaxHeight(int maxheight)
  {
    // if setting maxheight to lower value
    while (maxheight_ > maxheight)
      Pop();

    maxheight_ = maxheight;
    game_instance_ID_.reserve(maxheight);
  }

  void Tile::MaxHeightRequest(Packet& data)
  {
    dynamic_cast<Message<int>&>(data).data = GetMaxHeight();
  }

  void Tile::MaxHeightEvent(const Packet& payload)
  {
    int height = dynamic_cast<const Message<int>*>(&payload)->data;
    SetMaxHeight(height);
  }

  void Tile::HeightEvent(const Packet& payload)
  {
    int data = dynamic_cast<const Message<int>*>(&payload)->data;
    SetHeight(data);
  }

  void Tile::ScaleRequest(Packet& data)
  {
    dynamic_cast<Message<glm::vec2> &>(data).data = GetScale();
  }

  void Tile::GroupRequest(Packet& data)
  {
    dynamic_cast<Message<int> &>(data).data = GetGroup();
  }

  void Tile::XYRequest(Packet& data)
  {
    dynamic_cast<Message<glm::vec2>&>(data).data = GetXY();
  }

  void Tile::HeightRequest(Packet& data)
  {
    dynamic_cast<Message<int>&>(data).data = GetHeight();
  }

  void Tile::PushEvent(const Packet& payload)
  {
    unsigned long ID = dynamic_cast<const Message<unsigned long> *>(&payload)->data;
    Push(ID);
  }

  void Tile::GroupEvent(const Packet& payload)
  {
    int group = dynamic_cast<const Message<int>*>(&payload)->data;
    SetGroup(group);
  }

  void Tile::PopEvent(const Packet& payload)
  {
    Pop();
  }

  void Tile::XYEvent(const Packet& payload)
  {
    glm::vec2 pos = dynamic_cast<const Message<glm::vec2>*>(&payload)->data;
    SetTileXY(pos.x, pos.y);
  }

  void Tile::TileClicked(const Packet& payload)
  {
    // TODO: we need to not hard code building walls, pass ID
    GameInstance& game = stage_->addGameInstance("Wall");
    unsigned long ID = game.getId();
    Push(ID);
  }

  TileHandler::TileHandler(Stage* stage) : ComponentHandler(stage, "Tile")
  {
    dependencies_ = { "Transform", "Sprite" };
  }

  void TileHandler::getLuaRegisters()
  {
    using namespace luabind;

    getStage()->registerLuaModule(
      class_<Tile, Component>("Tile")
      .def_readonly("tilescale", &Tile::GetScale)
      );
  }

  void TileHandler::update()
  {
    
  }

  void TileHandler::ConnectEvents(Component* base_sub)
  {
    Tile* sub = dynamic_cast<Tile*>(base_sub);
    using namespace std::placeholders;
    Messenger& objMessenger = sub->getParent().getMessenger();

    SUBSCRIBER_ACTION push = std::bind(&Tile::PushEvent, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION pop = std::bind(&Tile::PopEvent, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION clickFace = std::bind(&Tile::TileClicked, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION setmaxheight = std::bind(&Tile::MaxHeightEvent, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION setheight = std::bind(&Tile::HeightEvent, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION setgroup = std::bind(&Tile::GroupEvent, sub, std::placeholders::_1);
    SUBSCRIBER_ACTION setpos = std::bind(&Tile::XYEvent, sub, std::placeholders::_1);

    REQUEST_ACTION scale = std::bind(&Tile::ScaleRequest, sub, std::placeholders::_1);
    REQUEST_ACTION group = std::bind(&Tile::GroupRequest, sub, std::placeholders::_1);
    REQUEST_ACTION getheight = std::bind(&Tile::HeightRequest, sub, std::placeholders::_1);
    REQUEST_ACTION getxy = std::bind(&Tile::XYRequest, sub, std::placeholders::_1);
    REQUEST_ACTION getmaxheight = std::bind(&Tile::MaxHeightRequest, sub, std::placeholders::_1);
    REQUEST_ACTION getChildren = [sub](Packet & data) {data.setData(sub->GetObjects()); };

    objMessenger.Subscribe(objMessenger, "PushTile", push);
    objMessenger.Subscribe(objMessenger, "PopTile", pop);
    objMessenger.Subscribe(objMessenger, "Clicked", clickFace);
    objMessenger.Subscribe(objMessenger, "SetTileMaxHeight", setmaxheight);
    objMessenger.Subscribe(objMessenger, "SetTileHeight", setheight);
    objMessenger.Subscribe(objMessenger, "SetTileGroup", setgroup);
    objMessenger.Subscribe(objMessenger, "SetTilePos", setpos);

    objMessenger.SetupRequest("TileScale", scale);
    objMessenger.SetupRequest("TileGroup", group);
    objMessenger.SetupRequest("TileHeight", getheight);
    objMessenger.SetupRequest("TilePos", getxy);
    objMessenger.SetupRequest("TileMaxHeight", getmaxheight);
    objMessenger.SetupRequest("TileEntities", getChildren);
  }
}
