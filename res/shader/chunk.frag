#version 400 core

uniform samplerCubeArray textures;
in vec4 pos;
flat in int tex;
out vec4 color;

void main(void)
{
  color = texture(textures, vec4((pos.xyz * 2) - vec3(1.0), float(tex)));
}
