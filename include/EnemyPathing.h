// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------
#pragma once

#include "../include/GameInstance.h"
#include "../include/Physics.h"


typedef std::pair<glm::vec2, glm::vec2> movement;

namespace Engine
{
    movement PathNormal( const movement& data);
    movement PathSwitchLane( const movement& data );
    movement PathFast( const movement& data);
    movement PathAccel( const movement& data);
    movement PathBump( const movement& data);
}
