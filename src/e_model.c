#include "e_model.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <malloc.h>

struct model model_create_file(const char *path)
{
	const struct aiScene *scene = aiImportFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices);

	struct model model;
	model.mesh_cnt = scene->mNumMeshes;
	model.meshes = malloc(model.mesh_cnt * sizeof(*model.meshes));
	for(uint i = 0; i < model.mesh_cnt; i++) {
		const struct aiMesh *ai_m = scene->mMeshes[i];
		struct mesh *m = model.meshes + i;
		m->indi_cnt = ai_m->mNumFaces * 3;
		m->vert_cnt = ai_m->mNumVertices;


	}

	return m;
}

void model_destroy(struct model *m)
{

}
