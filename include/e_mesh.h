#ifndef ENGINE_MESH_H_
#define ENGINE_MESH_H_

#include "e_vertex.h"

struct mesh {
	uint vert_cnt, indi_cnt;
	struct vertex *verts;
	uint *indis;
};

struct mesh mesh_create_data(uint vert_cnt, uint indi_cnt,
		struct vertex *verts, uint *indis);
struct mesh mesh_create_file(const char *path);
void mesh_destroy(struct mesh *m);

#endif /* ENGINE_MESH_H_ */
