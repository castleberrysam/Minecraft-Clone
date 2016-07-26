#ifndef SHADER_H
#define SHADER_H

GLuint load_shader(char *filepath, GLenum type);
GLuint compile_program(int num, GLuint *shaders);
void set_xfb_varyings(GLuint program, int num, char **varyings, GLenum order);

#endif /* SHADER_H */
