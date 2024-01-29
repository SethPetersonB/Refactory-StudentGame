// Primary Author : Kento Murawski
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include "../include/imgui_wrapper.h"
#include "../include/imgui_impl.h"
#include "../include/grid.h"
#include "../include/Input.h"
#include "../include/Messages.h"

#include <iostream>
#include <sstream>

static int BaseWindowSize_X;
static int BaseWindowSize_Y;

static bool show_imgui = false;
static bool show_grid_view = false;
static bool show_grid_heightmap = false;
static bool show_framerate = false;
static bool show_stage_info = false;
static bool show_struct_info = false;

static Display& disp = Engine::GSM::get().getDisplay();
//static Engine::Grid& grid = Engine::Stage::GetStage("TestStage1").GetGrid();;

/****************************************************************************/
/*!
\brief
Updates the ImGui window each frame

\param window
The SDL_Window to output imgui to

*/
/****************************************************************************/
void UpdateImGui(SDL_Window* window)
{
  ImGui_ImplSdlGL3_NewFrame(window);

  if (Engine::InputSystem::KeyPressed(SDL_SCANCODE_F3))
  {
    ToggleImgui();
  }

  if (show_imgui)
  {
    UpdateMain();
  }
}

/****************************************************************************/
/*!
\brief
Governs logic of updating the imgui interface
4 Buttons / Windows currently
- Framerate
- Grid View / Group View
- Stage info (Counts game objects only so far)
- Structure info (Lists all structures on the grid currently)

*/
/****************************************************************************/
void UpdateMain()
{
  Engine::Stage& stage = Engine::Stage::GetStage("TestStage1");
  Engine::Grid& grid = stage.GetGrid();
  {
    // Set Window Properties
   
    // Begin Window
    ImGui::Begin("Debug", &show_imgui);

    // Window Logic
    if (ImGui::Button("Framerate")) show_framerate ^= 1;
    if (ImGui::Button("Grid View")) show_grid_view ^= 1;
    if (ImGui::Button("Stage Info")) show_stage_info ^= 1;
    if (ImGui::Button("Structure Info")) show_struct_info ^= 1;

    // End Window
    ImGui::End();
  }

  if (show_framerate)
  {
    // Set Window Properties
    ImGui::SetNextWindowSize(ImVec2(20, 20), ImGuiSetCond_FirstUseEver);

    // Begin Window
    ImGui::Begin("Framerate", &show_framerate);

    // Window Logic
    ImGui::Text("FPS: %.3f", 1 / disp.GetFrameTime());

    // End Window
    ImGui::End();
  }

  if (show_grid_view)
  {
    // Set Window Properties
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(300, 300), ImGuiSetCond_FirstUseEver);
    static int e;

    // Begin Window
    ImGui::Begin("Grid", &show_grid_view);

    // Window Logic
    if (e != 2)
    {
      for (int i = grid.GetGridHeight() - 1; i >= 0; --i)
      {
        for (int j = 0; j < grid.GetGridWidth(); ++j)
        {
          int group = stage.getInstanceFromID(grid[i][j]).RequestData<int>("TileGroup");
          if (stage.getInstanceFromID(grid[i][j]).RequestData<int>("TileGroup") > 1)
          {
            if (e == 1)
            {
              int height = stage.getInstanceFromID(grid[i][j]).RequestData<int>("TileHeight");
              ImGui::TextColored(ImVec4(1.0f - height / 10.0f, 
                                        height / 3.0f, 
                                        height / 10.0f, 1.0f), 
                                        "%3d ", height); ImGui::SameLine(0, 0);
            }
            else if (e == 0)
            {
              
              ImGui::TextColored(ImVec4(1.0f - group / 10.0f, 
                                        group / 3.0f, 
                                        group / 10.0f, 1.0f), 
                                        "%3d ", group); ImGui::SameLine(0, 0);
            }
          }
          else
          {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.15f), "%3d ", group); ImGui::SameLine(0, 0);
          }
        }

        ImGui::Text("\n");
      }
    }
    else
    {
      for (int i = grid.GetGridHeight() - 1; i >= 0; --i)
      {
        for (int j = 0; j < grid.GetGridWidth(); ++j)
        {
          glm::vec2 XY = stage.getInstanceFromID(grid[i][j]).RequestData<glm::vec2>("TilePos");
          ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "(%2d, %2d) ", static_cast<int>(XY.x), static_cast<int>(XY.y)); ImGui::SameLine(0, 0);
        }

        ImGui::Text("\n");
      }
    }

    ImGui::Text("Number of cycles last parse: %d", grid.GetCycles());
    ImGui::Text("Number of total groups: %d", grid.GetGroupList().size());

    ImGui::RadioButton("group view", &e, 0); ImGui::SameLine();
    ImGui::RadioButton("height view", &e, 1); ImGui::SameLine();
    ImGui::RadioButton("xy view", &e, 2);

    if (ImGui::Button("Fill Grid"))
    {
      for (int i = grid.GetGridHeight() - 1; i >= 0; --i)
      {
        for (int j = 1; j < grid.GetGridWidth(); ++j)
        {
          stage.getInstanceFromID(grid[i][j]).PostMessage("Clicked", Engine::Message<glm::vec2>());
        }
      }
    }

    // End Window
    ImGui::End();
  }

  if (show_stage_info)
  {
    ImGui::Begin("Stage Debug", &show_stage_info);

    if (ImGui::CollapsingHeader("Stages"))
    {
      int i = 0;
      for (auto & order : Engine::Stage::StageList)
      {
        if (ImGui::TreeNode((void*)(intptr_t)i, "%d", order.first))
        {
          int j = 0;
          for (auto & stage : order.second)
          {
            if (ImGui::TreeNode((void*)(intptr_t)j, "%s", stage->getStageName().c_str()))
            {
              ImGui::Text("Game Objects In Stage: %i", stage->GetGameObjectCount());

              ImGui::TreePop();
            }

            ++j;
          }

          ImGui::TreePop();
        }

        ++i;
      }
    }

    ImGui::End();
  }

  if (show_struct_info)
  {
    ImGui::Begin("Structure Debug", &show_struct_info);

    std::map<unsigned int, std::shared_ptr<Engine::Structure> > tempList = grid.GetStructList();
    std::set<int> groupList = grid.GetGroupList();

    ImGui::Text("Number of Groups in Game: %d", groupList.size());
    ImGui::Text("Number of Structures in Game: %d", tempList.size());

    if (ImGui::CollapsingHeader("Possible Structures"))
    {
      int i = 0;

      Engine::Stage* objStage = &stage;

      Engine::GameInstance* gameObject = objStage->getMessenger().Request<Engine::GameInstance*>("PlayerController");

      Engine::STRUCT_LIST * LSL = gameObject->RequestData<Engine::STRUCT_LIST*>("GetParsedStructureList");

      for (auto& iter : *LSL)
      {
        if (ImGui::TreeNode((void*)(intptr_t)i, "Structure: %s", iter.first.c_str()))
        {
          int j = 0;
          for (int i = 0; i < iter.second.GetX(); ++i)
          {
            for (int j = 0; j < iter.second.GetY(); ++j)
            {
              int l = iter.second.GetGrid()[j][i]->GetHeight();
              if (l)
              {
                ImGui::Text(" %2i", l); ImGui::SameLine();
              }
              else
              {
                ImGui::Text("   "); ImGui::SameLine();
              }
            }
            ImGui::Text("\n");
          }

          ++j;

          ImGui::TreePop();
        }

        ++i;
      }
    }

    if (ImGui::CollapsingHeader("Structures"))
    {
      int i = 0;

      for (auto & order : tempList)
      {
        if (ImGui::TreeNode((void*)(intptr_t)i, "Structure #%d: %s", order.second->GetGroup(), 
          (order.second->GetEntity()) ? order.second->GetEntity()->getObjectType().c_str() : "Unknown Pattern"))
        {
          int j = 0;
          for (int i = 0; i < order.second->GetX(); ++i)
          {
            for (int j = 0; j < order.second->GetY(); ++j)
            {
              int l = order.second->GetGrid()[j][i]->GetHeight();
              if (l)
              {
                ImGui::Text(" %2i", l); ImGui::SameLine();
              }
              else
              {
                ImGui::Text("   "); ImGui::SameLine();
              }
            }
            ImGui::Text("\n");
          }

          ++j;

          ImGui::TreePop();
        }

        ++i;
      }
    }

    ImGui::End();
  }
}

/****************************************************************************/
/*!
\brief
Toggles whether or not imgui shows to the screen

*/
/****************************************************************************/
void ToggleImgui()
{
  show_imgui ^= 1;
}
