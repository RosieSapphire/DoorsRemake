#include "e_colmesh.h"
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

#include "e_gldef.h"
#include "e_shader.h"

static uint vao, vbo;
static uint indis[] = { 0, 1, 2 };

struct collision_mesh collision_mesh_create(struct mesh bmesh)
{
	struct collision_mesh cmesh;
	cmesh.face_cnt = bmesh.indi_cnt / 3;
	cmesh.faces = malloc(sizeof(*cmesh.faces) * cmesh.face_cnt);

	for(uint i = 0; i < cmesh.face_cnt; i++) {
		for(uint j = 0; j < 3; j++) {
			int ind = bmesh.indis[i * 3 + j];
			struct vertex vert = bmesh.verts[ind];
			cmesh.faces[i].verts[j] = vert;
		}
	}

	printf("Collision Mesh Faces:\n");
	for(uint i = 0; i < cmesh.face_cnt; i++) {
		printf("\tFace %d:\n", i);
		for(uint j = 0; j < 3; j++) {
			vec3 *vpos = &cmesh.faces[i].verts[j].pos;
			printf("\t\tVert %d (%d): %f, %f, %f\n", j, i * 3 + j,
					vpos[0][0], vpos[0][1], vpos[0][2]);
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	return cmesh;
}

static void collision_face_get_center(struct collision_face f, vec3 o)
{
	vec3_zero(o);
	for(int i = 0; i < 3; i++)
		vec3_add(o, f.verts[i].pos, o);
	vec3_scale(o, 1.0f / 3.0f);
}

uint collision_mesh_get_closest_face(struct collision_mesh m, vec3 p)
{
	float nearest_dist = INFINITY;
	uint nearest_ind = 0;
	for(uint i = 0; i < m.face_cnt; i++) {
		vec3 center;
		collision_face_get_center(m.faces[i], center);
		vec3 pdist;
		vec3_sub(center, p, pdist);
		float face_dist = vec3_len(pdist);
		if(fabsf(vec3_dot(m.faces[i].verts[0].norm,
						(vec3){0, 1, 0})) > 0.001f)
			continue;

		if(face_dist < nearest_dist) {
			nearest_dist = face_dist;
			nearest_ind = i;
		}
	}

	return nearest_ind;
}

void collision_face_draw(struct collision_face f, uint s, vec3 offset)
{
	vec3 verts[3];
	for(int i = 0; i < 3; i++)
		vec3_copy(f.verts[i].pos, verts[i]);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(s);
	uint model_loc = shader_get_loc(s, "u_model");
	mat4 model_mat = MAT4_IDENTITY_INIT;
	mat4_trans(model_mat, offset);
	shader_uni_mat4(model_loc, model_mat);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glBindVertexArray(0);
}

void collision_mesh_destory(struct collision_mesh *m)
{
	free(m->faces);
}
