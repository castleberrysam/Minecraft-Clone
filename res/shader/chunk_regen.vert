#version 400 core

layout(location=0) in vec4 pos;

void main(void)
{
  gl_Position = pos;
}
