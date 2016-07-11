#ifndef FONT_H
#define FONT_H

#include <stdbool.h>
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
  FT_Face face;
  GLuint *textures;
  FT_Pos *width;
  FT_Pos *height;
  FT_Pos *left;
  FT_Pos *top;
  FT_Pos *advance;
} Font;

bool font_init(Font *font, char *filepath, int size);
void font_delete(Font *font);
void font_draw_text(Font *font, char *text);

#endif /* FONT_H */
