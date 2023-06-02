#include "e_mesh.h"
#include "e_gldef.h"
#include <malloc.h>
#include <string.h>

struct mesh mesh_create(uint vert_cnt, uint indi_cnt,
		struct vertex *verts, uint *indis)
{
	struct mesh m;
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indis_size, indis, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return m;
}

void mesh_draw(struct mesh m, uint tex)
{
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
