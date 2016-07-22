#version 400 core

uniform mat4 mvp;
layout(location=0) in vec3 off;
layout(location=1) in vec3 pos;
layout(location=2) in int tex;
out vec3 vs_pos;
flat out int vs_tex;

void main(void)
{
  gl_Position = mvp * vec4(pos+off, 1.0);
  vs_pos = pos;
  vs_tex = tex;
}
