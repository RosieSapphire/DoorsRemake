#ifndef ENGINE_RENDER_LAYER_H_
#define ENGINE_RENDER_LAYER_H_

#include "e_util.h"
#include "e_gldef.h"

enum rlayer_formats {
	REN_NONE,
	REN_ALPHA,
	REN_RG,
	REN_RGB,
	REN_RGBA,
};

/* Render layer for displaying pixels and adding shaders and shit */
struct rlayer {
	uint fbo, rbo, tex;
};

struct rlayer rlayer_create(uint width, uint height, enum rlayer_formats fmt);
void rlayer_bind_and_clear(struct rlayer l, float r, float g, float b, float a);
void rlayer_destroy(struct rlayer *layer);

#endif
