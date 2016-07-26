#version 330 core

uniform sampler2D textures;
in vec2 vs_texcoords;
flat in int vs_tex;
out vec4 color;

void main(void)
{
  vec2 coord = vec2(((vs_tex%16)+vs_texcoords.x)/16.0, ((vs_tex/16)+vs_texcoords.y)/16.0);
  color = texture(textures, coord);
}
