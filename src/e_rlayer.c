#include "e_rlayer.h"
#include <stdlib.h>

struct rlayer rlayer_create(uint width, uint height, enum rlayer_formats fmt)
{
	struct rlayer layer;
	glGenFramebuffers(1, &layer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, layer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &layer.tex);
	glBindTexture(GL_TEXTURE_2D, layer.tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	layer.pixels = calloc(width * height, sizeof(*layer.pixels));
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height,
			0, fmt, GL_UNSIGNED_BYTE, layer.pixels);
	glBindTexture(GL_TEXTURE_2D, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		exit(EXIT_FAILURE);

	return layer;
}

void rlayer_destroy(struct rlayer *layer)
{
	glDeleteFramebuffers(1, &layer->fbo);
	glDeleteTextures(1, &layer->tex);
	free(layer->pixels);
}
