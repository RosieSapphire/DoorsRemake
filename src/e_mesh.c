#include "e_mesh.h"
#include <malloc.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct mesh mesh_create_data(uint vert_cnt, uint indi_cnt,
		struct vertex *verts, uint *indis)
{
	struct mesh m;
	m.vert_cnt = vert_cnt;
	m.indi_cnt = indi_cnt;
	m.verts = malloc(sizeof(*verts) * vert_cnt);
	m.indis = malloc(sizeof(*indis) * indi_cnt);
	return m;
}

void mesh_destroy(struct mesh *m)
{

}
