#version 330 core

uniform mat4 mvp;
layout(location=0) in vec4 vertex;
layout(location=1) in vec2 texcoords;
out vec2 vs_texcoords;

void main(void)
{
  gl_Position = mvp * vertex;
  vs_texcoords = texcoords;
}
