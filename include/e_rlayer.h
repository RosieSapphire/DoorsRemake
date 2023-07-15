#ifndef ENGINE_RENDER_LAYER_H_
#define ENGINE_RENDER_LAYER_H_

#include "e_util.h"
#include "e_gldef.h"

enum rlayer_formats {
	REN_NONE = GL_NONE,
	REN_ALPHA = GL_ALPHA,
	REN_RG = GL_RG,
	REN_RGB = GL_RGB,
	REN_RGBA = GL_RGBA,
};

/* Render layer for displaying pixels and adding shaders and shit */
struct rlayer {
	uint fbo, rbo, tex, width, height;
};

void rlayers_init(void);
struct rlayer rlayer_create(uint width, uint height, enum rlayer_formats fmt);
void rlayer_bind(struct rlayer l);
void rlayer_unbind_all(void);
void rlayer_draw(struct rlayer l);
void rlayer_destroy(struct rlayer *layer);
void rlayers_terminate(void);

#endif
