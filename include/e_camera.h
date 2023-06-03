#ifndef ENGINE_CAMERA_H_
#define ENGINE_CAMERA_H_

#include "e_context.h"

struct camera {
	vec3 pos;
	float yaw, pitch;
};

void camera_update_input(struct camera *c, struct input i);
void camera_get_forw(struct camera c, vec3 o);
void camera_get_up(vec3 o);
void camera_get_right(vec3 forw, vec3 up, vec3 o);
void camera_get_focus(struct camera c, vec3 forw, vec3 o);
void camera_get_mat4(struct camera c, mat4 o);

#endif
