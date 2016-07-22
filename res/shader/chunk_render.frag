#version 400 core

uniform samplerCubeArray textures;
in vec3 vs_pos;
flat in int vs_tex;
out vec4 color;

void main(void)
{
  // need to convert from [0, 1] to [-1 ,1]
  color = texture(textures, vec4((vs_pos * 2) - vec3(1.0), float(vs_tex)));
}
