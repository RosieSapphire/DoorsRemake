#ifndef ENGINE_SHADER_H_
#define ENGINE_SHADER_H_

#include "e_util.h"

uint shader_compile(const char *path, int type);
uint shader_create(const char *vert_path, const char *frag_path);
uint shader_get_loc(uint s, const char *loc);
void shader_use(uint s);
void shader_uni_mat4(uint loc, mat4 m4);
void shader_uni_vec3(uint loc, vec3 v3);
void shader_uni_int(uint loc, int i);
void shader_destroy(uint shader);

#endif /* ENGINE_SHADER_H_ */
