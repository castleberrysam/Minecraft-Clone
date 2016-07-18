#version 400 core

uniform sampler2D sampler;
in vec2 vs_texcoords;
out vec4 color;

void main(void)
{
  color = texture(sampler, vs_texcoords).rrrr;
}
