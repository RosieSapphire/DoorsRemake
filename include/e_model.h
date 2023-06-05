#ifndef ENGINE_MODEL_H_
#define ENGINE_MODEL_H_

#include "e_mesh.h"
#include <assimp/scene.h>

struct model {
	uint mesh_cnt;
	struct mesh *meshes;
	vec3 pos;
	vec3 rot;
	vec3 scale;
	const struct aiScene *scene;
};

struct model model_load(const char *path);
struct mesh *model_find_mesh_by_name(struct model m, const char *name);
float model_get_mesh_dist_from_point(struct model model,
		const char *mesh_name, vec3 p);
void model_recalc_mesh_matrices(struct model m);
void model_get_mat4(struct model m, mat4 o);
void model_draw(struct model m, uint tex, uint shader, mat4 proj, mat4 view);
void model_unload(struct model *m);

#endif /* ENGINE_MODEL_H_ */
