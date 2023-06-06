#include "e_model.h"
#include "e_mesh.h"
#include "e_util.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdio.h>
#include <stdlib.h>

static void assimp_mat_from_node(mat4 o, const struct aiNode *node)
{
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			float *matf_cur = (float *)&node->mTransformation;
			o[i][j] = matf_cur[j * 4 + i];
		}
	}
}

static void model_mul_mesh_matrices(struct model model,
		const struct aiNode *node, mat4 parent_mat)
{
	struct mesh *mesh = model_find_mesh_by_name(model, node->mName.data);
	if(!mesh)
		return;

	mat4 local_mat;
	mat4 final_mat;
	assimp_mat_from_node(local_mat, node);
	mat4_mul(parent_mat, local_mat, final_mat);
	mat4_trans(final_mat, mesh->pos);
	mat4_rot_euler(final_mat, mesh->rot);
	mat4_copy(final_mat, mesh->matrix);

	for(uint i = 0; i < node->mNumChildren; i++)
		model_mul_mesh_matrices(model, node->mChildren[i],
				mesh->matrix);
}

void model_recalc_mesh_matrices(struct model m)
{
	model_mul_mesh_matrices(m, m.scene->mRootNode, MAT4_IDENTITY);
}

struct model model_load(const char *path)
{
	struct model model;
	model.scene = aiImportFile(path,
			aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices);

	if(!model.scene) {
		fprintf(stderr, "Failed to load model from '%s'\n", path);
		exit(EXIT_FAILURE);
	}

	model.mesh_cnt = model.scene->mNumMeshes;
	model.meshes = malloc(model.mesh_cnt * sizeof(*model.meshes));
	for(uint i = 0; i < model.mesh_cnt; i++) {
		const struct aiMesh *ai_m = model.scene->mMeshes[i];
		struct mesh *m = model.meshes + i;
		strncpy(m->name, ai_m->mName.data, MESH_NAME_MAXLEN);
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

	model_recalc_mesh_matrices(model);

	/* default transform */
	vec3_copy((vec3){0, 0, 0}, model.pos);
	vec3_copy((vec3){0, 0, 0}, model.rot);
	vec3_copy((vec3){1, 1, 1}, model.scale);
	return model;
}

struct mesh *model_find_mesh_by_name(struct model m, const char *name)
{
	for(uint i = 0; i < m.mesh_cnt; i++) {
		struct mesh *cur = m.meshes + i;
		if(!strcmp(name, cur->name))
			return cur;
	}

	fprintf(stderr, "Failed to find mesh '%s' in array size %d\n",
			name, m.mesh_cnt);
	return NULL;
}

float model_mesh_dist_point(struct model model, const char *mesh_name, vec3 p)
{
	struct mesh *mesh = model_find_mesh_by_name(model, mesh_name);
	vec3 world_pos;
	mat4_get_pos(mesh->matrix, world_pos);
	mat4 model_trans = MAT4_IDENTITY_INIT;
	mat4_trans(model_trans, model.pos);
	mat4_mul_vec3(model_trans, world_pos);
	vec3 dist_vec;
	vec3_sub(world_pos, p, dist_vec);
	return vec3_len(dist_vec);
}

void model_get_mat4(struct model m, mat4 o)
{
	mat4_identity(o);
	mat4_trans(o, m.pos);
	mat4_rot_euler(o, m.rot);
}

void model_draw(struct model m, uint tex, uint shader, mat4 proj, mat4 view)
{
	for(uint i = 0; i < m.mesh_cnt; i++) {
		mat4 model_mat = MAT4_IDENTITY_INIT;
		mat4_trans(model_mat, m.pos);
		mat4_rot_euler(model_mat, m.rot);
		mesh_draw(m.meshes[i], tex, shader, proj, view, model_mat);
	}
}

void model_unload(struct model *m)
{
	for(uint i = 0; i < m->mesh_cnt; i++)
		mesh_destroy(m->meshes + i);
}
