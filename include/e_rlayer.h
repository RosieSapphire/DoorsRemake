#ifndef ENGINE_RENDER_LAYER_H_
#define ENGINE_RENDER_LAYER_H_

#include "e_util.h"
#include "e_gldef.h"

enum rlayer_formats {
	ERL_NONE,
	ERL_ALPHA,
	ERL_RG,
	ERL_RGB,
	ERL_RGBA,
};

/* Render layer for displaying pixels and adding shaders and shit */
struct rlayer {
	uint fbo;
	uint tex;
	uint8_t *pixels;
};

struct rlayer rlayer_create(uint width, uint height, enum rlayer_formats fmt);
void rlayer_destroy(struct rlayer *layer);

#endif
