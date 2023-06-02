#ifndef ENGINE_MODEL_H_
#define ENGINE_MODEL_H_

#include "e_mesh.h"

struct model {
	uint mesh_cnt;
	struct mesh *meshes;
};

struct model model_create_file(const char *path);
void model_destroy(struct model *m);

#endif /* ENGINE_MODEL_H_ */
