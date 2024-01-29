// Primary Author : Seth Peterson
//
// © Copyright 1996-2017, DigiPen Institute of Technology (USA). All rights reserved.
// FMOD Sound System Copyright © Firelight Technologies Pty, Ltd., 2017.
// ---------------------------------------------------------------------------------

#include "../include/EnemyPathing.h"

namespace Engine
{
  /* Only ensures the object is moving in the X direction,
     and eliminates any acceleration in that direction.
  */
  movement PathNormal(const movement& data)
  {
    glm::vec2 velocity = data.first;
    glm::vec2 acceleration = data.second;
    /* Makes sure the object is going forwards */
    if (velocity.x <= 0)
    {
      velocity.x = 50;
    }
    /* Makes sure there is no acceleration */
    if (acceleration.x != 0)
    {
      acceleration.x = 0;
    }
    return movement(velocity, acceleration);
  }

  /* Changes lane in +Y direction. "Magic number" implementation. */
  movement PathSwitchLane(const movement& data)
  {
    glm::vec2 velocity = data.first;
    glm::vec2 acceleration = data.second;
    /* Start lane change "upwards" and slow down. */
    if (velocity.y == 0)
    {
      velocity.y = 50;

      acceleration.y = -(3 + velocity.y / 4);
    }
    /* Make sure it is stopped */
    else if (acceleration.y != 0 && velocity.y <= 0)
    {
      acceleration.y = 0;
      velocity.y = 0;
    }
    return movement(velocity, acceleration);
  }

  /* Moves more quickly than normal. */
  movement PathFast(const movement& data)
  {
    glm::vec2 velocity = data.first;
    glm::vec2 acceleration = data.second;
    /* Makes sure the object is going forwards */
    if (velocity.x <= 0)
    {
      velocity.x = 100;
    }
    /* Makes sure there is no acceleration */
    if (acceleration.x != 0)
    {
      acceleration.x = 0;
    }
    return movement(velocity, acceleration);
  }

  movement PathAccel(const movement& data)
  {
    glm::vec2 velocity = data.first;
    glm::vec2 acceleration = data.second;
    /* Makes sure there is constant acceleration */
    
    acceleration.x = 1 + acceleration.x;
    
    return movement(velocity, acceleration);
  }

  /* Changes lane in +Y direction. "Magic number" implementation. */
  movement PathBump(const movement& data)
  {
    glm::vec2 velocity = data.first;
    glm::vec2 acceleration = data.second;
    /* Start lane change "upwards" and slow down. */
    if (velocity.x > 10)
    {
      velocity.x = -50;

      acceleration.x = (3 - velocity.x);
    }
    /* Make sure it is stopped */
    else if (acceleration.x != 0 && velocity.x >= 0)
    {
      acceleration.x = 0;
      velocity.x = 0;
    }
    return movement(velocity, acceleration);
  }


}

