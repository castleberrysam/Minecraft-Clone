#ifndef SHADER_H
#define SHADER_H

GLuint load_shader(char *filepath, GLenum type);
GLuint compile_program(int num, GLuint *shaders);

#endif /* SHADER_H */
