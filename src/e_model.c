#include "e_model.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdio.h>
#include <stdlib.h>

struct model model_load(const char *path)
{
	const struct aiScene *scene = aiImportFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices);

	if(!scene) {
		fprintf(stderr, "Failed to load model from '%s'\n", path);
		exit(EXIT_FAILURE);
	}

	struct model model;
	model.mesh_cnt = scene->mNumMeshes;
	model.meshes = malloc(model.mesh_cnt * sizeof(*model.meshes));
	for(uint i = 0; i < model.mesh_cnt; i++) {
		const struct aiMesh *ai_m = scene->mMeshes[i];
		struct mesh *m = model.meshes + i;
		uint vert_cnt = ai_m->mNumVertices;
		uint face_cnt = ai_m->mNumFaces;
		uint indi_cnt = face_cnt * 3;
		size_t verts_size = vert_cnt * sizeof(*m->verts);
		size_t indis_size = indi_cnt * sizeof(*m->indis);

		struct vertex *verts = malloc(verts_size);
		for(uint j = 0; j < vert_cnt; j++) {
			struct vertex *cur = verts + j;
			cur->pos[0] = ai_m->mVertices[j].x;
			cur->pos[1] = ai_m->mVertices[j].y;
			cur->pos[2] = ai_m->mVertices[j].z;
			cur->uv[0] = ai_m->mTextureCoords[0][j].x;
			cur->uv[1] = ai_m->mTextureCoords[0][j].y;
			cur->norm[0] = ai_m->mNormals[j].x;
			cur->norm[1] = ai_m->mNormals[j].y;
			cur->norm[2] = ai_m->mNormals[j].z;
		}

		uint *indis = malloc(indis_size);
		for(uint j = 0; j < face_cnt; j++) {
			for(int k = 0; k < 3; k++) {
				uint *cur = indis + j * 3;
				cur[0] = ai_m->mFaces[j].mIndices[0];
				cur[1] = ai_m->mFaces[j].mIndices[1];
				cur[2] = ai_m->mFaces[j].mIndices[2];
			}
		}

		*m = mesh_create(vert_cnt, indi_cnt, verts, indis);

		free(verts);
		free(indis);
	}

	/* default transform */
	vec3_copy((vec3){0, 0, 0}, model.pos);
	vec3_copy((vec3){0, 0, 0}, model.rot);
	vec3_copy((vec3){1, 1, 1}, model.scale);
	return model;
}

void model_get_mat4(struct model m, mat4 o)
{
	mat4_identity(o);
	mat4_trans(o, m.pos);
	mat4_rot(o, (vec3){1, 0, 0}, m.rot[0]);
	mat4_rot(o, (vec3){0, 1, 0}, m.rot[1]);
	mat4_rot(o, (vec3){0, 0, 1}, m.rot[2]);
}

void model_draw(struct model m, uint tex)
{
	for(uint i = 0; i < m.mesh_cnt; i++)
		mesh_draw(m.meshes[i], tex);
}

void model_unload(struct model *m)
{
	for(uint i = 0; i < m->mesh_cnt; i++)
		mesh_destroy(m->meshes + i);
}
