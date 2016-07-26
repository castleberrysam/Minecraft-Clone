#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string.h>
#include "font.h"
#include "shader.h"
#include "matrix.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

static FT_Library library = NULL;
static int x_dpi = 0;
static int y_dpi = 0;
static GLuint program;
static GLuint vao;

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

  GLuint shaders[2];
  shaders[0] = load_shader("res/shader/font.vert", GL_VERTEX_SHADER);
  shaders[1] = load_shader("res/shader/font.frag", GL_FRAGMENT_SHADER);
  program = compile_program(2, shaders);
  glGenVertexArrays(1, &vao);
  
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
  
  FT_Long nglyphs = font->face->num_glyphs;
  FT_BitmapGlyph *glyphs = malloc(sizeof(FT_BitmapGlyph) * nglyphs);
  font->maxwidth = 0;
  font->maxheight = 0;
  for(int i=0;i<nglyphs;++i) {
    // XXX rendering the bitmaps twice (possibly?)
    FT_Error error = FT_Load_Glyph(font->face, i, FT_LOAD_RENDER);
    if(error != FT_Err_Ok) {
#ifdef DEBUG_GRAPHICS
      fprintf(stderr, "[FONT ] error while loading size %d glyph %d from font %s\n", size, i, filepath);
#endif
      glyphs[i] = glyphs[0];
      continue;
    }
    
    error = FT_Get_Glyph(font->face->glyph, (FT_Glyph *) &glyphs[i]);
    if(error != FT_Err_Ok) {
      #ifdef DEBUG_GRAPHICS
      fprintf(stderr, "[FONT ] error while getting size %d glyph %d from font %s\n", size, i, filepath);
      #endif
      continue;
    }

    if(((FT_Glyph) glyphs[i])->format != FT_GLYPH_FORMAT_BITMAP) {
      error = FT_Glyph_To_Bitmap((FT_Glyph *) &glyphs[i], FT_RENDER_MODE_NORMAL, 0, true);
      if(error != FT_Err_Ok) {
#ifdef DEBUG_GRAPHICS
	fprintf(stderr, "[FONT ] error while converting size %d glyph %d to bitmap from font %s\n",
		size, i, filepath);
#endif
	continue;
      }
    }

    font->maxwidth = MAX(font->face->glyph->bitmap.width, font->maxwidth);
    font->maxheight = MAX(font->face->glyph->bitmap.rows, font->maxheight);
  }

  glGenTextures(1, &font->atlas);
  glBindTexture(GL_TEXTURE_2D, font->atlas);
  font->awidth = font->maxwidth * 16;
  font->aheight = font->maxheight * ((nglyphs / 16) + 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->awidth, font->aheight, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  font->width = malloc(sizeof(GLuint) * nglyphs);
  font->height = malloc(sizeof(GLuint) * nglyphs);
  font->left = malloc(sizeof(FT_Int) * nglyphs);
  font->top = malloc(sizeof(FT_Int) * nglyphs);
  font->advance = malloc(sizeof(FT_Pos) * nglyphs);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for(int i=0;i<nglyphs;++i) {
    FT_Bitmap bmp = glyphs[i]->bitmap;
    font->width[i] = bmp.width;
    font->height[i] = bmp.rows;
    font->left[i] = glyphs[i]->left;
    font->top[i] = glyphs[i]->top;
    font->advance[i] = glyphs[i]->root.advance.x;

    glTexSubImage2D(GL_TEXTURE_2D, 0, font->maxwidth*(i%16), font->maxheight*(i/16),
		    bmp.width, bmp.rows, GL_RED, GL_UNSIGNED_BYTE, bmp.buffer);
    FT_Done_Glyph((FT_Glyph) glyphs[i]);
  }
  free(glyphs);
  
  return true;
}

void font_delete(Font *font)
{
  glDeleteTextures(1, &font->atlas);
  free(font->width);
  free(font->height);
  free(font->left);
  free(font->top);
  free(font->advance);
}

void font_text_draw(Font *font, char *text)
{
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  int len = strlen(text);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*16*len, NULL, GL_STREAM_DRAW);
  GLfloat *data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  double off = 0.0;
  for(int i=0;i<len;++i) {
    FT_UInt glyph = FT_Get_Char_Index(font->face, text[i]);
    double xpos = off + font->left[glyph];
    double ypos = font->top[glyph];
    GLint width = font->width[glyph];
    GLint height = font->height[glyph];
    GLfloat spos = (glyph % 16) * font->maxwidth;
    GLfloat tpos = (glyph / 16) * font->maxheight;

    int idata = i * 16;
    data[idata+0] = xpos;
    data[idata+1] = ypos;
    data[idata+2] = spos / font->awidth;
    data[idata+3] = tpos / font->aheight;
    
    data[idata+4] = xpos;
    data[idata+5] = ypos - height;
    data[idata+6] = spos / font->awidth;
    data[idata+7] = (tpos+font->height[glyph]) / font->aheight;
    
    data[idata+8] = xpos + width;
    data[idata+9] = ypos;
    data[idata+10] = (spos+font->width[glyph]) / font->awidth;
    data[idata+11] = tpos / font->aheight;
    
    data[idata+12] = xpos + width;
    data[idata+13] = ypos - height;
    data[idata+14] = (spos+font->width[glyph]) / font->awidth;
    data[idata+15] = (tpos+font->height[glyph]) / font->aheight;
    
    off += font->advance[glyph] / 65536.0;
  }
  glUnmapBuffer(GL_ARRAY_BUFFER);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(program);
  glUniform1i(glGetUniformLocation(program, "sampler"), 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, font->atlas);
  matrix_update_mvp(glGetUniformLocation(program, "mvp"));
  glBindVertexArray(vao);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, (const GLvoid *) 0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4, (const GLvoid *) (sizeof(GLfloat)*2));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4*len);

  glDeleteBuffers(1, &buffer);
}

int font_text_width(Font *font, char *text)
{
  double width = 0.0;
  for(int i=0;i<strlen(text);++i) {
    FT_UInt glyph = FT_Get_Char_Index(font->face, text[i]);
    width += font->advance[glyph];
  }
  return (int) (width / 65536.0);
}
