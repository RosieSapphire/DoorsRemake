#ifndef ENGINE_TEXTURE_H_
#define ENGINE_TEXTURE_H_

#include "e_util.h"

uint texture_load(const char *path);
uint texture_create_empty(int width, int height);
void texture_unload(uint *tex);

#endif /* ENGINE_TEXTURE_H_ */
