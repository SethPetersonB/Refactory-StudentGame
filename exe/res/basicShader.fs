#version 120

varying vec3 vertPos;

varying vec4 fragColor;
varying mat4 transFort;

void main()
{
  //color = vec3(1.0, 0.0, 0.0);
  gl_FragColor = transFort * (fragColor - vec4(cos(vertPos.x), sin(vertPos.y), tan(vertPos.x + vertPos.y), 0.0)); //texture2D(diffuse, texCoord0);
  //gl_FragColor = fragColor;
}