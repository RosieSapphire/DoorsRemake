#include "e_camera.h"
#include <math.h>
#include "e_context.h"
#include <stdio.h>

#define CAM_SENS 0.04f

void camera_update_input(struct camera *c, struct input i)
{
	c->yaw += i.mpos_diff[0] * DEG_TO_RAD * CAM_SENS;
	c->pitch += i.mpos_diff[1] * DEG_TO_RAD * CAM_SENS;
	c->pitch = clampf(c->pitch, -PI_HALF, PI_HALF);
}

void camera_get_forw(struct camera c, vec3 o)
{
	o[0] = cosf(c.yaw) * cosf(c.pitch);
	o[1] = sinf(c.pitch);
	o[2] = sinf(c.yaw) * cosf(c.pitch);
	// vec3_norm(o);
}

void camera_get_up(vec3 o)
{
	o[0] = 0;
	o[1] = 1;
	o[2] = 0;
}

void camera_get_right(vec3 forw, vec3 up, vec3 o)
{
	vec3_cross(up, forw, o);
}

void camera_get_focus(struct camera c, vec3 forw, vec3 o)
{
	vec3_add(forw, c.pos, o);
}

void camera_get_mat4(struct camera c, mat4 o)
{
	vec3 forw;
	vec3 focus;
	camera_get_forw(c, forw);
	camera_get_focus(c, forw, focus);
	mat4_lookat(o, c.pos, focus, (vec3){0, 1, 0});
}
