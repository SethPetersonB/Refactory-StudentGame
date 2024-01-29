#version 430

in vec4 fragColor;
in vec2 outTex;

out vec4 outColor;

uniform sampler2D diffuse;
uniform bool textured;

void main() 
{
 
  if(textured)
    outColor = texture(diffuse, outTex) * fragColor;
  else 
    outColor = fragColor;
}
