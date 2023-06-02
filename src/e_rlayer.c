#include "e_rlayer.h"
#include <stdio.h>
#include <stdlib.h>

struct rlayer rlayer_create(uint width, uint height, enum rlayer_formats fmt)
{
	struct rlayer layer;
	glGenFramebuffers(1, &layer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, layer.fbo);

	glGenTextures(1, &layer.tex);
	glBindTexture(GL_TEXTURE_2D, layer.tex);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, width, height,
			0, fmt, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, layer.tex, 0);

	glGenRenderbuffers(1, &layer.rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, layer.rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
			width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
			GL_RENDERBUFFER, layer.rbo);

	int fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(fbo_status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Failed to create framebuffer: %d\n",
				fbo_status);
		exit(EXIT_FAILURE);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return layer;
}

void rlayer_bind_and_clear(struct rlayer l, float r, float g, float b, float a)
{
	glBindFramebuffer(GL_FRAMEBUFFER, l.fbo);
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void rlayer_destroy(struct rlayer *layer)
{
	glDeleteFramebuffers(1, &layer->fbo);
	glDeleteTextures(1, &layer->tex);
}
