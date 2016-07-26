#version 330 core

uniform mat4 mvp;
layout(location=0) in vec3 pos;
layout(location=1) in vec2 texcoords;
layout(location=2) in int tex;
out vec2 vs_texcoords;
flat out int vs_tex;

void main(void)
{
  gl_Position = mvp * vec4(pos, 1.0);
  vs_texcoords = texcoords;
  vs_tex = tex;
}
