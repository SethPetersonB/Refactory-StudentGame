#version 430


layout(location = 0)in vec2 position;
layout(location = 1)in vec4 color;
layout(location = 2)in vec2 inTex;

uniform mat4x4 transform;
uniform uint currFrame;
uniform uint frameCount;

out vec4 fragColor;
out vec2 outTex;

void main() 
{
  vec2 unTex = vec2(inTex.x * (1.0f / frameCount), inTex.y);

  gl_Position = transform * vec4(position.x, position.y, 0, 1);
  fragColor = color;
  outTex = vec2(unTex.x, unTex.y) + vec2(float(currFrame) / frameCount, 0);
}
