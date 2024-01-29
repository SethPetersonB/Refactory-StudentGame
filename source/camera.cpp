// Primary Author : Kento Muraski
// 
// Co-authors:
//    Philip Nygard (Implementation of dynamic systems)
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <luabind/luabind.hpp>
#include "../include/camera.h"
#include "../include/display.h"
//#include "../include/shader.h"
#include "../include/GSM.h"
//#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "../include/Logger.h"
#include "DrawUtils.h"

#define EPSILON 10e-6

using namespace Logger;

static float Clamp2Pi(float val)
{
  float  pi2 = glm::pi<float>() * 2;

  val = (fmod(val, pi2));

  return (val > 0) ? val : (2 * glm::pi<float>()) - val;
}

static glm::mat4 IsoMatrix()
{
  glm::mat4 isoScale = glm::scale(glm::vec3(1.0f, glm::tan(glm::radians(30.0f)), 1.0f));
  glm::mat4 isoRot = glm::rotate(glm::radians(-45.0f), glm::vec3(0, 0, 1));
  glm::mat4 isoShear;

  return isoScale * isoRot;
}


namespace Engine
{
  
  Camera::Camera() :
    needsUpdating(true),
    screenHeight_(0),
    screenWidth_(0),
    camWidth_(30),
    camHeight_(30),
    rot_(0),
    isoRot_(0),
    position_(0.0f, 0.0f)
    //orthoMat_(1.0f),
    //cameraMat_(1.0f)
  {
    //Update();
  }

  Camera::~Camera()
  {
    
  }

  void Camera::Init()
  {
    screenHeight_ = static_cast<float>(GSM::get().getRenderer().getViewHeight());
    screenWidth_ = static_cast<float>(GSM::get().getRenderer().getViewWidth());
  }

  void Camera::Zoom(float amount)
  {
    camWidth_ += amount;
    camHeight_ += amount;

    if (camWidth_ < minZoom || camHeight_ < minZoom)
    {
      camWidth_ = static_cast<float>(minZoom);
      camHeight_ = static_cast<float>(minZoom);
    }
    else if (camWidth_ > maxZoom || camHeight_ > maxZoom)
    {
      camWidth_ = static_cast<float>(maxZoom);
      camHeight_ = static_cast<float>(maxZoom);
    }

    needsUpdating = true;
  }

  void Camera::Translate(const glm::vec2 & amount)
  {
    //glm::vec3 trans3{ amount.x, amount.y, 0 };
    glm::vec4 trans4{ amount.x, amount.y, 0, 0 };

    trans4 = glm::rotate(-isoRot_, glm::vec3{ 0, 0, 1 }) * glm::inverse(DrawUtils::IsoMatrix()) *  trans4;
    glm::vec2 translate{ trans4.x, trans4.y };

    position_ += translate;

    needsUpdating = true;
  }

  void Camera::setIsoRotate(float goal, float speed)
  {
    if (!isLerping_)
      rotGoal_ = (isoRot_ + goal);
    else
      rotGoal_ = (rotGoal_ + goal);

    //rotDelta_ = Clamp2Pi(rotDelta_ + goal);
    rotSpeed_ = speed;
    isLerping_ = true;
  }

  void Camera::rotateIsometric(float rot)
  {
    isoRot_ = Clamp2Pi(isoRot_ + rot);
  }

  void Camera::updateLerp(float dt)
  {
    if (isLerping_)
    {
      float pi = glm::pi<float>();
      float pi2 = 2 * pi;
      float pi3 = 3 * pi;
      float shortest_angle = (fmod((fmod((rotGoal_ - isoRot_), pi2) + pi3), pi2) - pi);

      if (abs(shortest_angle) <= EPSILON)
      {
        rotateIsometric(shortest_angle);
        isLerping_ = false;
      }

      rotateIsometric(shortest_angle * rotSpeed_ * dt);

      needsUpdating = true;
    }
  }

  void Camera::Update()
  {
    updateLerp(GSM::get().getDisplay().GetFrameTime());

    if(needsUpdating)
    {
      float ar = screenWidth_ / screenHeight_;

      glm::mat4 trans = glm::translate(glm::vec3(position_.x, position_.y, 0));
      glm::mat4 scale = glm::scale(glm::vec3( camWidth_, camHeight_, 1));
      glm::mat4 rot = glm::rotate(rot_, glm::vec3(0.0, 0.0, 1.0));
      
      cameraToWorld_ = trans * rot * scale;
      worldToCamera_ = glm::inverse(cameraToWorld_);

      //ortho_ = glm::ortho(0.0f, static_cast<float>(disp.GetWidth()), 0.0f, static_cast<float>(disp.GetHeight()));

      if(screenWidth_ >= screenHeight_)
      cameraToNDC_ = glm::scale(glm::vec3(2 / (ar * camWidth_), 2 / camHeight_ , 1.0f));
      else
        cameraToNDC_ = glm::scale(glm::vec3(2  / camWidth_, 2 * ar / camHeight_, 1.0f));

      NDCToCamera_ = glm::inverse(cameraToNDC_);

      GSM::get().getRenderer().getDrawGroup(DrawUtils::RL_WORLD).setTransformation(cameraToNDC_ * worldToCamera_);
      GSM::get().getRenderer().getDrawGroup(DrawUtils::RL_TILE).setTransformation(cameraToNDC_ * worldToCamera_);

      needsUpdating = false;
    }

    UpdateIsometrics();
  }

  static Camera& get()
  {
    return GSM::get().getCamera();
  }

  static void ZoomBind(Camera& cam, float amount)
  {
    cam.Zoom(amount);
  }

  static void RotateBind(Camera& cam, float x, float y)
  {
    //GFXPipe::get().SetLerpIsometric(x, y);
  }

  
  luabind::scope Camera::GetLuaRegisters()
  {
    using namespace luabind;
    return class_<Camera>("Camera")
      .scope[def("getSystem", get)]
      .property("position", &Camera::getPosition, &Camera::setPosition)
      .def("Zoom", ZoomBind)
      .def("Rotate", &Camera::setIsoRotate)
      .def("Translate", &Camera::Translate);
  }


  void Camera::UpdateIsometrics()
  {

  }
}


