#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string.h>
#include "font.h"

static FT_Library library = NULL;
static int x_dpi = 0;
static int y_dpi = 0;

static FT_Error prepare_lib(void)
{
  if(library != NULL) {return FT_Err_Ok;}
  FT_Error error = FT_Init_FreeType(&library);
  if(error != FT_Err_Ok) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[FONT ] FreeType init failed, error %d\n", error);
#endif
    return error;
  }

  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  if(monitor == NULL) {goto dpi_fail;}
  const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);
  if(vidmode == NULL) {goto dpi_fail;}
  glfwGetMonitorPhysicalSize(monitor, &x_dpi, &y_dpi);
  if(x_dpi == 0 || y_dpi == 0) {
  dpi_fail:
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[FONT ] monitor dpi calculation failed\n");
#endif
    return -1;
  }
  x_dpi = (int) round(vidmode->width / (x_dpi / 25.4));
  y_dpi = (int) round(vidmode->height / (y_dpi / 25.4));
#ifdef DEBUG_GRAPHICS
  fprintf(stderr, "[FONT ] monitor dpi is %d by %d\n", x_dpi, y_dpi);
#endif
  return FT_Err_Ok;
}

bool font_init(Font *font, char *filepath, int size)
{
  if(prepare_lib() != FT_Err_Ok) {return false;}
  
  FT_Error error = FT_New_Face(library, filepath, 0, &font->face);
#ifdef DEBUG_GRAPHICS
  if(error == FT_Err_Unknown_File_Format) {
    fprintf(stderr, "[FONT ] path %s is an unsupported font format\n", filepath);
    return false;
  } else if(error != FT_Err_Ok) {
    fprintf(stderr, "[FONT ] path %s does not exist or is corrupted\n", filepath);
    return false;
  }
#else
  if(error != FT_Err_Ok) {return false;}
#endif
  
  error = FT_Set_Char_Size(font->face, 0, size*64, x_dpi, y_dpi);
  if(error != FT_Err_Ok) {
#ifdef DEBUG_GRAPHICS
    fprintf(stderr, "[FONT ] error while setting size %d for font %s\n", size, filepath);
#endif
    return !error;
  }    

  font->width = malloc(sizeof(FT_Pos) * font->face->num_glyphs);
  font->height = malloc(sizeof(FT_Pos) * font->face->num_glyphs);
  font->left = malloc(sizeof(FT_Pos) * font->face->num_glyphs);
  font->top = malloc(sizeof(FT_Pos) * font->face->num_glyphs);
  font->advance = malloc(sizeof(FT_Pos) * font->face->num_glyphs);
  font->textures = malloc(sizeof(GLuint) * font->face->num_glyphs);
  glGenTextures(font->face->num_glyphs, font->textures);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for(int i=0;i<font->face->num_glyphs;++i) {
    GLuint texture = font->textures[i];

    FT_Error error = FT_Load_Glyph(font->face, i, FT_LOAD_RENDER);
    if(error != FT_Err_Ok) {
#ifdef DEBUG_GRAPHICS
      fprintf(stderr, "[FONT ] error while loading size %d glyph %d from font %s\n", size, i, filepath);
#endif
      font->width[i] = font->width[0];
      font->height[i] = font->height[0];
      font->left[i] = font->left[0];
      font->top[i] = font->top[0];
      font->advance[i] = font->advance[0];
      glDeleteTextures(1, &font->textures[i]);
      font->textures[i] = font->textures[0];
      continue;
    }

    font->width[i] = font->face->glyph->metrics.width;
    font->height[i] = font->face->glyph->metrics.height;
    font->left[i] = font->face->glyph->metrics.horiBearingX;
    font->top[i] = font->face->glyph->metrics.horiBearingY;
    font->advance[i] = font->face->glyph->metrics.horiAdvance;

    FT_Bitmap bmp = font->face->glyph->bitmap;
    unsigned char *tmp = malloc(bmp.width * bmp.rows * 4);
    for(int i=0;i<bmp.width*bmp.rows*4;i+=4) {
      memset(&tmp[i], bmp.buffer[i>>2], 4);
    }
    
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp.width, bmp.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(tmp);
  }
  return true;
}

void font_delete(Font *font)
{
  glDeleteTextures(font->face->num_glyphs, font->textures);
  free(font->textures);
  free(font->width);
  free(font->height);
  free(font->left);
  free(font->top);
  free(font->advance);
}

void font_draw_text(Font *font, char *text)
{
  double off = 0.0;
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_TEXTURE_2D);
  for(int i=0;i<strlen(text);++i) {
    FT_UInt glyph = FT_Get_Char_Index(font->face, text[i]);
    double xpos = off + (font->left[glyph] / 64.0);
    double ypos = font->top[glyph] / 64.0;
    double width = font->width[glyph] / 64.0;
    double height = font->height[glyph] / 64.0;

    glBindTexture(GL_TEXTURE_2D, font->textures[glyph]);
    glBegin(GL_QUADS);
    glTexCoord2d(0.0, 0.0);
    glVertex2d(xpos, ypos);
    glTexCoord2d(0.0, 1.0);
    glVertex2d(xpos, ypos-height);
    glTexCoord2d(1.0, 1.0);
    glVertex2d(xpos+width, ypos-height);
    glTexCoord2d(1.0, 0.0);
    glVertex2d(xpos+width, ypos);
    glEnd();
    
    off += font->advance[glyph] / 64.0;
  }
}
