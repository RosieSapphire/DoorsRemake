#ifndef ENGINE_COLLISION_MESH_H_
#define ENGINE_COLLISION_MESH_H_

#include "e_mesh.h"

struct collision_face {
	struct vertex verts[3];
};

struct collision_mesh {
	struct collision_face *faces;
	uint face_cnt;
};

struct collision_mesh collision_mesh_create(struct mesh bmesh);
uint collision_mesh_get_closest_face(struct collision_mesh m, vec3 p);
void collision_face_draw(struct collision_face f, uint s, vec3 offset);
void collision_mesh_destory(struct collision_mesh *m);

#endif /* ENGINE_COLLISION_MESH_H_ */
