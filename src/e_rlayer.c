#include "e_rlayer.h"
#include "e_shader.h"
#include "e_mesh.h"
#include <stdio.h>
#include <stdlib.h>

static uint shader;
static struct mesh quad;

void rlayers_init(void)
{
	shader = shader_create("shaders/fbo.vert", "shaders/fbo.frag");

	struct vertex verts[] = {
		{{-1, -1, 0}, {0, 0}, {0}, {0}, {0}},
		{{ 1, -1, 0}, {1, 0}, {0}, {0}, {0}},
		{{-1,  1, 0}, {0, 1}, {0}, {0}, {0}},
		{{ 1,  1, 0}, {1, 1}, {0}, {0}, {0}},
	};

	uint indis[] = { 0, 1, 2, 2, 1, 3 };
	uint vert_cnt = sizeof(verts) / sizeof(*verts);
	uint indi_cnt = sizeof(indis) / sizeof(*indis);
	quad = mesh_create(NULL, vert_cnt, indi_cnt, verts, indis);
}

struct rlayer rlayer_create(uint width, uint height, enum rlayer_formats fmt)
{
	struct rlayer layer;
	glGenFramebuffers(1, &layer.fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, layer.fbo);

	layer.width = width;
	layer.height = height;
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
		fprintf(stderr, "Failed to create framebuffer: %d "
				"(Occured at line %d)\n",
				fbo_status, __LINE__);
		exit(EXIT_FAILURE);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return layer;
}

void rlayer_bind(struct rlayer l)
{
	glBindFramebuffer(GL_FRAMEBUFFER, l.fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void rlayer_unbind_all(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void rlayer_draw(struct rlayer l)
{
	int tex_loc = shader_get_loc(shader, "u_tex");
	if(tex_loc == -1) {
		fprintf(stderr, "Failed to find shader loc 'u_tex' "
				"in shader %d.\n", shader);
		exit(EXIT_FAILURE);
	}

	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(quad.vao);
	shader_use(shader);
	shader_uni_int(tex_loc, 0);
	glBindTexture(GL_TEXTURE_2D, l.tex);
	glDrawElements(GL_TRIANGLES, quad.indi_cnt, GL_UNSIGNED_INT, quad.indis);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void rlayer_destroy(struct rlayer *layer)
{
	glDeleteFramebuffers(1, &layer->fbo);
	glDeleteTextures(1, &layer->tex);
}

void rlayers_terminate(void)
{
	shader_destroy(shader);
	mesh_destroy(&quad);
}
