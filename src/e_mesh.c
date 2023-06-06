#include "e_mesh.h"
#include "e_gldef.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "e_shader.h"
#include "e_util.h"

struct mesh mesh_create(const char *name, uint vert_cnt, uint indi_cnt,
		struct vertex *verts, uint *indis)
{
	struct mesh m;
	vec3_zero(m.pos);
	vec3_zero(m.rot);
	vec3_one(m.scale);
	m.is_visible = true;
	if(name) {
		if(strlen(name) >= MESH_NAME_MAXLEN) {
			fprintf(stderr, "Mesh name is too fucking long: %s\n",
					name);
			exit(EXIT_FAILURE);
		}
		strncpy(m.name, name, MESH_NAME_MAXLEN);
	}

	m.vert_cnt = vert_cnt;
	m.indi_cnt = indi_cnt;
	size_t verts_size = sizeof(*verts) * vert_cnt; 
	size_t indis_size = sizeof(*indis) * indi_cnt; 
	m.verts = malloc(verts_size);
	m.indis = malloc(indis_size);
	bcopy(verts, m.verts, verts_size);
	bcopy(indis, m.indis, indis_size);

	glGenVertexArrays(1, &m.vao);
	glBindVertexArray(m.vao);
	
	glGenBuffers(1, &m.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
	glBufferData(GL_ARRAY_BUFFER, verts_size, verts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(*verts),
			(void *)offsetof(struct vertex, pos));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(*verts),
			(void *)offsetof(struct vertex, uv));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(*verts),
			(void *)offsetof(struct vertex, norm));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(*verts),
			(void *)offsetof(struct vertex, tan));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(*verts),
			(void *)offsetof(struct vertex, bitan));

	glGenBuffers(1, &m.ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indis_size,
			indis, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return m;
}

void mesh_draw(struct mesh m, uint tex, uint shader,
		mat4 proj, mat4 view, mat4 offset)
{
	if(!m.is_visible)
		return;

	int proj_loc =      shader_get_loc(shader, "u_proj");
	int view_loc =      shader_get_loc(shader, "u_view");
	int model_loc =     shader_get_loc(shader, "u_model");
	int using_tex_loc = shader_get_loc(shader, "u_is_using_tex");

	shader_use(shader);
	mat4 model_mat = MAT4_IDENTITY_INIT;
	mat4_mul(model_mat, offset, model_mat);
	mat4_mul(model_mat, m.matrix, model_mat);
	shader_uni_mat4(model_loc, model_mat);
	shader_uni_mat4(view_loc, view);
	shader_uni_mat4(proj_loc, proj);
	shader_uni_int(using_tex_loc, false);

	glBindVertexArray(m.vao);
	glBindTexture(GL_TEXTURE_2D, tex);
	glDrawElements(GL_TRIANGLES, m.indi_cnt, GL_UNSIGNED_INT, m.indis);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void mesh_destroy(struct mesh *m)
{
	glDeleteVertexArrays(1, &m->vao);
	glDeleteBuffers(1, &m->vbo);
	glDeleteBuffers(1, &m->ebo);
	free(m->verts);
	free(m->indis);
}
