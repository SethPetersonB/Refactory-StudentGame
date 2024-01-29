// Primary Author : Kento Muraski
// 
// Co-authors:
//    Philip Nygard (Implementation of dynamic systems)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#pragma once

#include "Messages.h"

#include "glm/glm/glm.hpp"
#include "glm/glm/vec2.hpp"
#include "glm/glm/mat4x4.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/ext.hpp"

namespace luabind
{
  struct scope;
}

namespace Engine
{
  class Camera
  {
  public:
  Camera();

  ~Camera();

  void Update();

  //glm::mat4 getMat(void) { return cameraMat_; }
  glm::vec2 ScreenToWorld(const glm::vec2 & position);
  void Init();
  const glm::mat4 & CameraToWorld() { return cameraToWorld_; }
  const glm::mat4 & WorldToCamera() { return worldToCamera_; }
  const glm::mat4 & NDCToCamera() { return NDCToCamera_; }
  const glm::mat4 & CameraToNDC() { return cameraToNDC_; }

  void SetScreenSize(float x, float y) { screenWidth_ = x, screenHeight_ = y; needsUpdating = true; }
  void Zoom(float amount);
  void Translate(const glm::vec2 & amount);
  void Rotate(float rot) { rot_ += rot; needsUpdating = true; };
  void setIsoRotate(float goal, float speed);
  void rotateIsometric(float rot);
  void updateLerp(float dt);
  //const glm::vec2 & Up() { return up_; }
  //const glm::vec2 & Right() { return right_; }

  //void setScale(float scale) { scale_ = scale; needsUpdating = true; }
  void setPosition(const glm::vec2& newPosition) { position_ = newPosition; needsUpdating = true; }

  void PassUniform();

  const glm::vec2 & getPosition() { return position_; }
  float getRotation() const { return rot_; }
  float getIsoRot() const { return isoRot_; }

  // lua stuff
  static luabind::scope GetLuaRegisters();

  protected:
    void UpdateIsometrics();

  private:
    const int maxZoom = 40;
    const int minZoom = 5;
    //Messenger objMessenger_;
    // Doesn't make much sense to copy a camera at the moment
    Camera(const Camera &) = delete;
    Camera & operator=(const Camera &) = delete;

    bool needsUpdating;
    float screenHeight_;
    float screenWidth_;
    float camWidth_;
    float camHeight_;
    float rot_;
    float isoRot_;
    float rotGoal_;
    float rotSpeed_;
    bool isLerping_;

    glm::vec2 position_;
    //glm::mat4 orthoMat_;
    glm::mat4 cameraToWorld_;
    glm::mat4 worldToCamera_;
    glm::mat4 cameraToNDC_;
    glm::mat4 NDCToCamera_;

    glm::mat4 worldTrans_;
    glm::mat4 tileTrans_;

    //glm::mat4 ortho_;
    //glm::mat4 cameraMat_;
  };
}