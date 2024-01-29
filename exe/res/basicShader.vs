#version 130

in vec3 vertexPosition;
in vec4 vertexColor;

varying vec3 vertPos;
varying vec4 fragColor;
varying mat4 transFort;

uniform mat4 transform;

void main()
{
  vertPos = vertexPosition;
  fragColor = vertexColor;
  transFort = transform;
  //gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
  gl_Position = transform * vec4(vertexPosition, 1.0f);
}