#version 430


layout(location = 0)in vec2 position;
layout(location = 1)in vec4 color;
layout(location = 2)in vec2 inTex;

uniform mat4x4 transform;

out vec4 fragColor;
out vec2 outTex;

void main() 
{
  gl_Position = transform * vec4(position.x, position.y, 0, 1);
  fragColor = color;
  outTex = vec2(inTex.x, 1.0f - inTex.y);;
}
