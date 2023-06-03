#include "e_model.h"
#include "e_mesh.h"
#include "e_util.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdio.h>
#include <stdlib.h>

static struct mesh *model_find_mesh_by_name(const char *name,
		struct mesh *array, uint array_len)
{
	for(uint i = 0; i < array_len; i++) {
		struct mesh *cur = array + i;
		if(!strcmp(name, cur->name))
			return cur;
	}

	fprintf(stderr, "Failed to find mesh '%s' in array size %d\n",
			name, array_len);
	return NULL;
}

static void model_mul_mesh_matrices(const struct aiNode *node, mat4 m,
		struct mesh *mesh_array, uint mesh_array_len)
{
	printf("%s\n", node->mName.data);
	struct mesh *mesh = model_find_mesh_by_name(node->mName.data,
			mesh_array, mesh_array_len);

	// mat4_copy(&node->mTransformation, mesh->matrix);
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			float *matf = (float *)&node->mTransformation;
			mesh->matrix[i][j] = matf[j * 4 + i];
		}
	}
	mat4_mul(mesh->matrix, m, mesh->matrix);
	mat4_printf(mesh->matrix);

	for(uint i = 0; i < node->mNumChildren; i++) {
		const struct aiNode *cur = node->mChildren[i];
		model_mul_mesh_matrices(cur, mesh->matrix,
				mesh_array, mesh_array_len);
	}
}

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
		strncpy(m->name, ai_m->mName.data, MESH_NAME_MAXLEN);
		printf("Mesh %d (%s):\n", i, m->name);
		uint vert_cnt = ai_m->mNumVertices;
		uint face_cnt = ai_m->mNumFaces;
		uint indi_cnt = face_cnt * 3;
		size_t verts_size = vert_cnt * sizeof(*m->verts);
		size_t indis_size = indi_cnt * sizeof(*m->indis);

		struct vertex *verts = malloc(verts_size);
		printf("\tVert Cnt: %d\n", vert_cnt);
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
			printf("\t\tV %d: (%f, %f, %f)\n", j, cur->pos[0],
					cur->pos[1], cur->pos[2]);
		}

		uint *indis = malloc(indis_size);
		for(uint j = 0; j < face_cnt; j++) {
			uint *cur = indis + j * 3;
			for(int k = 0; k < 3; k++) {
				cur[0] = ai_m->mFaces[j].mIndices[0];
				cur[1] = ai_m->mFaces[j].mIndices[1];
				cur[2] = ai_m->mFaces[j].mIndices[2];
			}
		}

		*m = mesh_create(ai_m->mName.data, vert_cnt,
				indi_cnt, verts, indis);

		free(verts);
		free(indis);
	}

	mat4 iden;
	mat4_identity(iden);
	model_mul_mesh_matrices(scene->mRootNode, iden,
			model.meshes, model.mesh_cnt);

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

void model_draw(struct model m, uint tex, uint shader, mat4 proj, mat4 view)
{
	for(uint i = 0; i < m.mesh_cnt; i++)
		mesh_draw(m.meshes[i], tex, shader, proj, view);
}

void model_unload(struct model *m)
{
	for(uint i = 0; i < m->mesh_cnt; i++)
		mesh_destroy(m->meshes + i);
}
