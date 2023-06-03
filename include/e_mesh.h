#ifndef ENGINE_MESH_H_
#define ENGINE_MESH_H_

#include "e_vertex.h"

#define MESH_NAME_MAXLEN 128

struct mesh {
	uint vert_cnt, indi_cnt;
	struct vertex *verts;
	uint *indis;
	uint vao, vbo, ebo;
	char name[MESH_NAME_MAXLEN];
	mat4 matrix;
};

struct mesh mesh_create(const char *name, uint vert_cnt, uint indi_cnt,
		struct vertex *verts, uint *indis);
void mesh_draw(struct mesh m, uint tex, uint shader, mat4 proj, mat4 view);
void mesh_destroy(struct mesh *m);

#endif /* ENGINE_MESH_H_ */
