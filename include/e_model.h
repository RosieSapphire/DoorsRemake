#ifndef ENGINE_MODEL_H_
#define ENGINE_MODEL_H_

#include "e_mesh.h"

struct model {
	uint mesh_cnt;
	struct mesh *meshes;
	vec3 pos;
	vec3 rot;
	vec3 scale;
};

struct model model_load(const char *path);
void model_get_mat4(struct model m, mat4 o);
void model_draw(struct model m, uint tex, uint shader, mat4 proj, mat4 view);
void model_unload(struct model *m);

#endif /* ENGINE_MODEL_H_ */
