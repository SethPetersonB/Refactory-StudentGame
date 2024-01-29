// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

//#define NLOGGING

#ifndef NLOGGING
#include <functional>
#include "Logger.h"
#include "glm/glm/mat4x4.hpp"

// Rendering logging levels 

struct RenderInfo : Logger::Info
{
  static const char * levelId() { return "RENDER INFO"; }
};

struct RenderWarning : Logger::Warning
{
  static const char * levelId() { return "RENDER WARNING"; }
};

struct RenderError : Logger::Error
{
  static const char * levelId() { return "RENDER ERROR"; }
};
#endif

enum class ScaleReference { XX, XY, YY };

using DrawLayer = unsigned;

class Renderer;
class Shader;
class RMesh;
class DrawSurface;
class DrawGroup;
class DrawToken;
class DrawSystem;

struct SDL_Window;
union SDL_Event;


using COMPFUNC = std::function<bool(const DrawToken &, const DrawToken &)>;

// Vertex modification function. Takes a reference to an object to modify and
// returns two values. The first is the modification matrix, the second is whether it
// augments the objects transfomration matrix (false) or if it overrides it (true)
using MODFUNC = std::function<std::pair<glm::mat4, bool>(const DrawToken &)>;
