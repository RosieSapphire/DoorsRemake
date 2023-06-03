#include "e_camera.h"
#include <math.h>
#include "e_context.h"
#include <stdio.h>

#define CAM_SENS 0.04f
#define RUN_MULTIPLIER 2
#define CAM_HEIGHT 2.1f

#define FORW_SPEED 200.0f
#define SIDE_SPEED 350.0f
#define FRICTION 6
#define STOP_SPEED 100.0f
#define MAX_SPEED 320.0f
#define AIR_MAX_SPEED 30.0f
#define ACCELERATE 30.0f
#define JUMP_FORCE 270.0f
#define MOVE_SCALAR 0.02f
#define GRAVITY 800 

struct camera camera_update_axis(struct camera c, struct input i)
{
	c.yaw += i.mpos_diff[0] * DEG_TO_RAD * CAM_SENS;
	c.pitch += i.mpos_diff[1] * DEG_TO_RAD * CAM_SENS;
	c.pitch = clampf(c.pitch, -PI_HALF, PI_HALF);
	return c;
}

static bool camera_is_grounded(struct camera c)
{
	return c.pos_real[1] <= 0;
}

static struct camera camera_friction(struct camera c, float dt)
{
	vec3 ground_vel;
	vec3_copy(c.vel, ground_vel);
	ground_vel[1] = 0;

	float speed = vec3_len(ground_vel);
	if(speed < 1) {
		c.vel[0] = 0;
		c.vel[2] = 0;
		return c;
	}

	float drop = 0;
	if(camera_is_grounded(c)) {
		drop = fmaxf(speed, STOP_SPEED) * FRICTION * dt;
	}

	float new_speed = fmaxf(speed - drop, 0) / speed;
	vec3_scale(c.vel, new_speed);
	return c;
}

static struct camera camera_accelerate(struct camera c,
		vec3 wish_dir, float wish_speed, float accel, float dt)
{
	float current_speed = vec3_dot(c.vel, wish_dir);
	float add_speed = wish_speed - current_speed;
	if(add_speed <= 0)
		return c;
	float accel_speed = fminf(accel * dt * wish_speed, add_speed);
	vec3_muladd(c.vel, wish_dir, accel_speed, c.vel);
	return c;
}

static struct camera camera_air_accelerate(struct camera c,
		vec3 wish_dir, float wish_speed, float accel, float dt)
{
	float wish_speed_clamp = fminf(wish_speed, AIR_MAX_SPEED);
	float current_speed = vec3_dot(c.vel, wish_dir);
	float add_speed = wish_speed_clamp - current_speed;
	if(add_speed <= 0)
		return c;
	float accel_speed = fminf(accel * wish_speed * dt, add_speed);
	vec3_muladd(c.vel, wish_dir, accel_speed, c.vel);
	return c;
}

static struct camera camera_air_move(struct camera c, struct input i, float dt)
{
	float forw_speed = FORW_SPEED;
	if(i.run_last)
		forw_speed *= RUN_MULTIPLIER;

	float fmove = (i.s_last - i.w_last) * forw_speed;
	float smove = (i.d_last - i.a_last) * SIDE_SPEED;
	vec3 forw, side, up;
	camera_get_forw(c, forw);
	camera_get_up(up);
	camera_get_right(forw, up, side);
	forw[1] = 0;
	side[1] = 0;
	vec3_norm(forw);
	vec3_norm(side);

	vec3 wish_vel;
	vec3_muladd((vec3){0, 0, 0}, forw, fmove, wish_vel);
	vec3_muladd(wish_vel, side, smove, wish_vel);
	wish_vel[1] = 0;

	vec3 wish_dir;
	vec3_copy(wish_vel, wish_dir);
	float wish_speed = vec3_norm(wish_dir);
	if(wish_speed > MAX_SPEED) {
		vec3_scale(wish_vel, MAX_SPEED / wish_speed);
		wish_speed = MAX_SPEED;
	}

	if(camera_is_grounded(c)) {
		c.vel[1] = 0;
		c.pos_real[1] = 0;
		c = camera_accelerate(c, wish_dir,
				wish_speed, ACCELERATE, dt);
		c.vel[1] -= GRAVITY * dt;
	} else {
		c = camera_air_accelerate(c, wish_dir,
				wish_speed, ACCELERATE, dt);
		c.vel[1] -= GRAVITY * dt;
	}

	return c;
}


struct camera camera_move(struct camera c, struct input i, float dt)
{
	vec3 forw, side, up;
	camera_get_up(up);
	camera_get_forw(c, forw);
	camera_get_right(forw, up, side);
	
	c = camera_friction(c, dt);
	c = camera_air_move(c, i, dt);

	if(camera_is_grounded(c)) {
		c.pos_real[1] = 0;
		if(i.jump_last)
			c.vel[1] += JUMP_FORCE;
	}

	vec3_muladd(c.pos_real, c.vel, dt * MOVE_SCALAR, c.pos_real);
	return c;
}

void camera_get_forw(struct camera c, vec3 o)
{
	o[0] = cosf(c.yaw) * cosf(c.pitch);
	o[1] = sinf(c.pitch);
	o[2] = sinf(c.yaw) * cosf(c.pitch);
	vec3_norm(o);
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
	vec3_add(forw, c.pos_real, o);
}

static struct camera camera_calc_headbob(struct camera c,
		vec3 side, vec3 up, float dt)
{
	vec3_copy(c.pos_real, c.pos_bob);
	vec3 ground_vel;
	vec3_copy(c.vel, ground_vel);
	ground_vel[1] = 0;
	float speed = vec3_len(ground_vel) / MAX_SPEED;

	if(camera_is_grounded(c))
		c.bob_timer += dt * sqrtf(speed) * 0.8f;

	vec3 headbob = {0, 0, 0};
	vec3_muladd(headbob, side, sinf(c.bob_timer * 8) * 0.3f, headbob);
	vec3_muladd(headbob, up, -cosf(c.bob_timer * 16) * 0.2f, headbob);
	vec3_scale(headbob, speed);
	vec3_add(c.pos_real, headbob, c.pos_bob);

	return c;
}

struct camera camera_get_mat4(struct camera c, mat4 o, float dt)
{
	vec3 forw;
	vec3 focus;
	camera_get_forw(c, forw);
	camera_get_focus(c, forw, focus);
	vec3 up;
	camera_get_up(up);
	vec3 side;
	camera_get_right(forw, up, side);
	c = camera_calc_headbob(c, side, up, dt);
	vec3 headbob_cal;
	vec3_sub(c.pos_bob, c.pos_real, headbob_cal);
	vec3_add(focus, headbob_cal, focus);
	vec3_muladd(c.pos_bob, (vec3){0, 1, 0}, CAM_HEIGHT, c.pos_bob);
	vec3_muladd(focus, (vec3){0, 1, 0}, CAM_HEIGHT, focus);
	mat4_lookat(o, c.pos_bob, focus, up);
	return c;
}
