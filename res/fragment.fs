#version 430

in vec4 fragColor;
in vec2 outTex;

out vec4 outColor;

uniform sampler2D diffuse;
uniform bool textured;
uniform vec4 shade;
uniform uint currFrame;
uniform uint frameCount;

void main() 
{
  vec4 newColor = fragColor * shade;

  if(textured)
    outColor = texture(diffuse, outTex) * newColor;
  else 
    outColor = newColor;
}
