#ifndef ENGINE_GL_DEF_H_
#define ENGINE_GL_DEF_H_

#include <GLFW/glfw3.h>
#include <stdbool.h>
#include "e_util.h"

/* GENERAL */
void glViewport(int x, int y, int width, int height);

/* SHADERS */
uint glCreateShader(int type);
void glShaderSource(uint shader, size_t count,
		const char **string, const int *length);
void glCompileShader(uint shader);
void glGetShaderiv(uint shader, int param_in, int *param_out);
void glGetShaderInfoLog(uint shader, int max_length, int *length, char *log);
uint glCreateProgram(void);
void glAttachShader(uint program, uint shader);
void glLinkProgram(uint program);
void glGetProgramiv(uint program, int param_in, int *param_out);
void glGetProgramInfoLog(uint program, int max_length, int *length, char *log);
void glUseProgram(uint shader);
void glDeleteShader(uint shader);
void glDeleteProgram(uint program);

/* UNIFORMS */
int glGetUniformLocation(uint program, const char *name);
void glUniformMatrix4fv(int loc, int num, bool norm, float v[4][4]);
void glUniform2fv(int loc, int num, float v[2]);
void glUniform3fv(int loc, int num, float v[3]);
void glUniform1i(int loc, int num);

/* VAO */
void glGenVertexArrays(uint count, uint *vaos);
void glBindVertexArray(uint vao);
void glDeleteVertexArrays(int count, uint *vaos);

/* BUFFERS */
void glGenBuffers(uint count, uint *buffers);
void glBindBuffer(int buffer, uint id);
void glBufferData(int buffer, size_t size, void *data, int draw_type);
void glEnableVertexAttribArray(int index);
void glVertexAttribPointer(uint	index, int size,
		int type, bool normalized, int stride, const void *pointer);
void glDeleteBuffers(uint count, uint *buffers);

/* FRAMEBUFFERS (specifically) */
void glGenFramebuffers(uint count, uint *fbos);
void glDeleteFramebuffers(uint count, uint *fbos);
void glBindFramebuffer(uint target, uint fbo);
void glFramebufferTexture2D(uint target, uint attach,
		uint tex_target, uint tex_id, uint mip_level);
int glCheckFramebufferStatus(uint target);

/* RENDERBUFFERS */
void glGenRenderbuffers(uint num, uint *rbos);
void glBindRenderbuffer(uint target, uint rbo);
void glRenderbufferStorage(uint target, uint param, uint w, uint h);
void glFramebufferRenderbuffer(uint target, uint attach,
		uint ren_target, uint rbo_id);

#endif /* ENGINE_GL_DEF_H_ */
