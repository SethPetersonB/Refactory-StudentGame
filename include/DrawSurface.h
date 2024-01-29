// Primary Author : Philip Nygard
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

class DrawSurface
{
public:
  virtual ~DrawSurface() {};

  virtual void bind() const = 0;
  virtual void unbind() const = 0;
};
