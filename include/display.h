// Primary Author : Kento Murawski
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include <string>
#include <functional>

#include "Draw_fwd.h"

class Display
{
public:
  using EVENT_HANDLER = std::function<bool(const SDL_Event &)>;

  Display();

  bool Update();
  bool IsClosed();
  bool isFullScreen() const { return isFullScreen_; }

  int GetHeight() const { return m_winHeight; }
  int GetWidth() const { return m_winWidth; }
  inline SDL_Window* GetWindow() { return m_window; }
  void Destroy();
  float GetFrameTime();
  void UpdateFrameTime();
  void SetWindowTitle(const std::string& title);
  void SetSize(int x, int y);
  void setFullScreen(bool fullscreen);

  void setEventHandler(const EVENT_HANDLER & handler);

  void Initialize(int width, int height, const std::string& win_title);
  void Reinitialize(int width, int height, const std::string& win_title);

  virtual ~Display();

private:
  //Engine::Messenger objMessenger_;
  void processEvent(const SDL_Event & event);

  // Number of frames to store for calculating weighted framerate
  static const unsigned FRAME_VALUES = 10;

  int m_winHeight;
  int m_winWidth;
  SDL_Window* m_window;
  bool isClosed;
  bool isFullScreen_;

  EVENT_HANDLER externalEvent_;

  size_t frametimes[FRAME_VALUES];  // Timings for previous frames
  size_t frametimelast;             // Previous frame time
  size_t framecount;                // Number of frames stored

  float framespersecond; // Frametime

};
