#ifndef FONT_H
#define FONT_H

#include <stdbool.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

typedef struct {
  FT_Face face;
  GLuint atlas;
  int awidth;
  int aheight;
  int maxwidth;
  int maxheight;
  int *width;
  int *height;
  FT_Int *left;
  FT_Int *top;
  FT_Pos *advance;
} Font;

bool font_init(Font *font, char *filepath, int size);
void font_delete(Font *font);
void font_text_draw(Font *font, char *text);
int font_text_width(Font *font, char *text);

#endif /* FONT_H */
