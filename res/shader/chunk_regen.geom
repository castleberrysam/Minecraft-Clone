#version 400 core

uniform isamplerBuffer blocks;
layout(points) in;
layout(triangle_strip, max_vertices=24) out;
out vec3 off;
out vec3 pos;
out int tex;

const vec3 vertices[8] = vec3[](
  vec3(0.0, 0.0, 0.0), // 0
  vec3(0.0, 0.0, 1.0), // 1
  vec3(0.0, 1.0, 0.0), // 2
  vec3(0.0, 1.0, 1.0), // 3
  vec3(1.0, 0.0, 0.0), // 4
  vec3(1.0, 0.0, 1.0), // 5
  vec3(1.0, 1.0, 0.0), // 6
  vec3(1.0, 1.0, 1.0)  // 7
);

const int indices[24] = int[](
  6, 4, 2, 0, // 0  (NORTH)
  7, 5, 6, 4, // 4  (EAST)
  3, 1, 7, 5, // 8  (SOUTH)
  2, 0, 3, 1, // 12 (WEST)
  2, 3, 6, 7, // 16 (TOP)
  4, 5, 0, 1  // 20 (BOTTOM)
);

int getblock(int x, int y, int z)
{
  vec4 offset = gl_in[0].gl_Position + vec4(x+1, y+1, z+1, 0.0);
  int index = int(offset.x + (offset.y * 18) + (offset.z * 324));
  return texelFetch(blocks, index).x;
}

void main(void)
{
  int current = getblock(0,0,0);
  if(current == -1) {return;}
  if(getblock(0,0,-1) == -1) {
    for(int i=0;i<4;++i) {
      off = gl_in[0].gl_Position.xyz;
      pos = vertices[indices[i]];
      tex = current;
      EmitVertex();
    }
    EndPrimitive();
  }
  if(getblock(1,0,0) == -1) {
    for(int i=0;i<4;++i) {
      off = gl_in[0].gl_Position.xyz;
      pos = vertices[indices[i+4]];
      tex = current;
      EmitVertex();
    }
    EndPrimitive();
  }
  if(getblock(0,0,1) == -1) {
    for(int i=0;i<4;++i) {
      off = gl_in[0].gl_Position.xyz;
      pos = vertices[indices[i+8]];
      tex = current;
      EmitVertex();
    }
    EndPrimitive();
  }
  if(getblock(-1,0,0) == -1) {
    for(int i=0;i<4;++i) {
      off = gl_in[0].gl_Position.xyz;
      pos = vertices[indices[i+12]];
      tex = current;
      EmitVertex();
    }
    EndPrimitive();
  }
  if(getblock(0,1,0) == -1) {
    for(int i=0;i<4;++i) {
      off = gl_in[0].gl_Position.xyz;
      pos = vertices[indices[i+16]];
      tex = current;
      EmitVertex();
    }
    EndPrimitive();
  }
  if(getblock(0,-1,0) == -1) {
    for(int i=0;i<4;++i) {
      off = gl_in[0].gl_Position.xyz;
      pos = vertices[indices[i+20]];
      tex = current;
      EmitVertex();
    }
    EndPrimitive();
  }
}
