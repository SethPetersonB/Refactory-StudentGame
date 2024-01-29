// Primary Author : Kento Murawski
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#include <SDL2/SDL.h>

#include "display.h"
//#include "../include/Logger.h"


#ifdef NDEBUG
#define START_FULLSCREEN true
#else
#define START_FULLSCREEN false
#endif // NDEBUG


// Frametime is calculated by a weighted average of the last few frames
// frametimes is an array of the last couple values, equal to FRAME_VALUES

/****************************************************************************/
/*!
\brief
Default display constructor
*/
/****************************************************************************/
Display::Display() :
  externalEvent_{ nullptr }, isClosed{ true }, 
  m_window{ nullptr }, m_winWidth{ 0 }, m_winHeight{ 0 }
{}

/****************************************************************************/
/*!
\brief
Sets the size of the display

\param x
width of display

\param y
height of display
*/
/****************************************************************************/
void Display::SetSize(int x, int y)
{
  m_winWidth = x;
  m_winHeight = y;
}

void Display::setFullScreen(bool fullscreen)
{
  Uint32 flag;

  if (fullscreen)
    flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
  else
    flag = 0;

  SDL_SetWindowFullscreen(m_window, flag);

  isFullScreen_ = fullscreen;
}

void Display::setEventHandler(const EVENT_HANDLER & handler)
{
  externalEvent_ = handler;
}

/****************************************************************************/
/*!
\brief
Initializes the display

\param width
Width of display

\param height
Height of display

\param win_title
The title of the display
*/
/****************************************************************************/
void Display::Initialize(int width, int height, const std::string& win_title)
{
  SDL_Init(SDL_INIT_EVERYTHING);


  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);
  int Width = width;// DM.w;
  int Height = height;// DM.h;

  m_window = SDL_CreateWindow(win_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  
  setFullScreen(START_FULLSCREEN);

  isClosed = false;

  m_winWidth = Width;
  m_winHeight = Height;

  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

 // Set all frame times to 0ms.
  memset(frametimes, 0, sizeof(frametimes));
  framecount = 0;
  framespersecond = 0;
  frametimelast = SDL_GetTicks();
}

/****************************************************************************/
/*!
\brief
Updates the framtime of the display
*/
/****************************************************************************/
void Display::UpdateFrameTime()
{
  size_t frametimesindex;
  size_t getticks;
  size_t count;
  size_t i;

  // This value rotates back to 0 after it hits FRAME_VALUES.
  frametimesindex = framecount % FRAME_VALUES;

  // store the current time
  getticks = SDL_GetTicks();

  // save the frame time value
  frametimes[frametimesindex] = getticks - frametimelast;

  // save the last frame time for the next update
  frametimelast = getticks;

  // increment the frame count
  framecount++;

  if (framecount < FRAME_VALUES) {

    count = framecount;

  }
  else {

    count = FRAME_VALUES;

  }

  // add up all the values and divide to get the average frame time.
  framespersecond = 0;
  for (i = 0; i < count; i++) {

    framespersecond += frametimes[i];

  }

  framespersecond /= count;

  // Convert to FPS
  framespersecond = 1000.0f / framespersecond;

}

/****************************************************************************/
/*!
\brief
Returns the frametime

\return
1 / frames per second (If you want things to move 5 per second, multiply the 5 by frametime)
*/
/****************************************************************************/
float Display::GetFrameTime()
{
  return 1.0f / framespersecond;
}

/****************************************************************************/
/*!
\brief
Destroys and reinitializes the display

\param width
Width of display

\param height
Height of display

\param win_title
The title of the display
*/
/****************************************************************************/
void Display::Reinitialize(int width, int height, const std::string& win_title)
{
  Destroy();
  Initialize(width, height, win_title);
}


/****************************************************************************/
/*!
\brief
*/
/****************************************************************************/
Display::~Display()
{
  Destroy();
}

void Display::processEvent(const SDL_Event & event)
{
  switch (event.type)
  {
    // Window exit
  case SDL_QUIT:
  {
    //isClosed = true;
  }
  break;

  case SDL_WINDOWEVENT:
  {
    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
      SetSize(event.window.data1, event.window.data2);
    }
    break;

  default:
    break;
  }
  }
}


/****************************************************************************/
/*!
\brief
Updates the display
*/
/****************************************************************************/
bool Display::Update()
{
  UpdateFrameTime();

  SDL_Event e;

  while(SDL_PollEvent(&e))
  {
    if (externalEvent_)
      externalEvent_(e);

    processEvent(e);
  }

  return !isClosed;
}

/****************************************************************************/
/*!
\brief
Deinitializes the display
*/
/****************************************************************************/
void Display::Destroy()
{
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

/****************************************************************************/
/*!
\brief
Returns whether or not the display is closed

\return
bool of closed value
*/
/****************************************************************************/
bool Display::IsClosed()
{
  return isClosed;
}

/****************************************************************************/
/*!
\brief
Sets window title to given string (Does not work for some reason)
\param title
The string to set the window to 
*/
/****************************************************************************/
void Display::SetWindowTitle(const std::string& title)
{
  SDL_SetWindowTitle(m_window, title.c_str());
}
