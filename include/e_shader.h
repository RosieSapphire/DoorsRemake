#ifndef ENGINE_SHADER_H_
#define ENGINE_SHADER_H_

#include "e_util.h"

uint shader_compile(const char *path, int type);
uint shader_create(const char *vert_path, const char *frag_path);
uint shader_get_loc(unsigned int s, const char *loc);
void shader_bind(unsigned int s);
void shader_uni_mat4(unsigned int loc, mat4 m4);
void shader_uni_vec3(unsigned int loc, vec3 v3);
void shader_uni_int(unsigned int loc, int i);

#endif /* ENGINE_SHADER_H_ */
