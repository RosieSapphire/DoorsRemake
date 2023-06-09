#ifndef ENGINE_CAMERA_H_
#define ENGINE_CAMERA_H_

#include "e_context.h"

struct camera {
	vec3 pos_real, pos_bob, vel;
	float yaw, pitch, bob_timer, grounded_timer, jump_timer;
	bool init_jump, jump_queued, first_jump;
};

struct camera camera_update_axis(struct camera c, struct input i);
struct camera camera_move(struct camera c, struct input i, float dt);
void camera_get_forw(struct camera c, vec3 o);
void camera_get_up(vec3 o);
void camera_get_right(vec3 forw, vec3 up, vec3 o);
void camera_get_focus(struct camera c, vec3 forw, vec3 o);
struct camera camera_get_mat4(struct camera c, mat4 o, float dt);

#endif
