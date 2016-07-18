#include <stdlib.h>
#include <png.h>
#include "texture.h"

GLuint texture_array = 0;
static int num_textures = 0;

int texture_load(char *filepath)
{
  if(texture_array == 0) {
    glGenTextures(1, &texture_array);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, texture_array);
    // XXX only about 150MB, but probably should just allocate as needed
    //    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_RGB8, 64, 64, 6*16);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_RGB, 64, 64, 6*16,
		 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid *) NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  
  FILE *file = fopen(filepath, "rb");
  if(!file) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[TEXTR] path %s does not exist\n", filepath);
#endif
    return -1;
  }

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[TEXTR] png_structp allocation failed while loading %s\n", filepath);
#endif
    fclose(file);
    return -1;
  }
  png_infop info = png_create_info_struct(png);
  if(!info) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[TEXTR] png_infop allocation failed while loading %s\n", filepath);
#endif
    png_destroy_read_struct(&png, (png_infopp) NULL, (png_infopp) NULL);
    fclose(file);
    return -1;
  }
  
  if(setjmp(png_jmpbuf(png))) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[TEXTR] libpng error while loading %s\n", filepath);
#endif
    png_destroy_read_struct(&png, &info, (png_infopp) NULL);
    fclose(file);
    return -1;
  }

  png_init_io(png, file);
  
  png_read_info(png, info);
  int width = png_get_image_width(png, info);
  int height = png_get_image_height(png, info);
  png_byte color_type = png_get_color_type(png, info);
  png_byte bit_depth = png_get_bit_depth(png, info);

  // read any color_type into 8-bit depth RGBA format
  if(bit_depth == 16) {png_set_strip_16(png);}
  if(color_type == PNG_COLOR_TYPE_PALETTE) {png_set_palette_to_rgb(png);}
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {png_set_expand_gray_1_2_4_to_8(png);}
  if(png_get_valid(png, info, PNG_INFO_tRNS)) {png_set_tRNS_to_alpha(png);}
  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE) {
    png_set_filler(png, 0xff, PNG_FILLER_AFTER);
  }
  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    png_set_gray_to_rgb(png);
  }
  png_read_update_info(png, info);

  int rowbytes = png_get_rowbytes(png, info);
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, rowbytes*height, NULL, GL_STREAM_DRAW);
  png_byte *data = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
  png_bytep rows[height];
  for(int y=0;y<height;++y) {rows[height-1-y] = &data[y*rowbytes];}
  png_read_image(png, rows);
  glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

  png_destroy_read_struct(&png, &info, (png_infopp) NULL);
  fclose(file);

  glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, texture_array);
  //  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid *) 0);
  for(int i=0;i<6;++i) {
    glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, (num_textures*6)+i, width, height, 1,
		    GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid *) 0);
  }
  
#ifdef DEBUG_GRAPHICS
  fprintf(stderr, "[TEXTR] path %s, width %d, height %d, rowbytes %d\n", filepath, width, height, rowbytes);
#endif
  glDeleteBuffers(1, &buffer);
  //  return texture;
  return num_textures++;
}
