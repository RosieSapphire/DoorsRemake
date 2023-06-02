#ifndef ENGINE_MESH_H_
#define ENGINE_MESH_H_

#include "e_vertex.h"

struct mesh {
	uint vert_cnt, indi_cnt;
	struct vertex *verts;
	uint *indis;
	uint vao, vbo, ebo;
};

struct mesh mesh_create(uint vert_cnt, uint indi_cnt,
		struct vertex *verts, uint *indis);
void mesh_draw(struct mesh m, uint tex);
void mesh_destroy(struct mesh *m);

#endif /* ENGINE_MESH_H_ */
