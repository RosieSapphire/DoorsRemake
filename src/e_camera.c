#include "e_camera.h"
#include <math.h>
#include "e_context.h"
#include "e_util.h"
#include <stdio.h>

#define CAM_SENS 0.04f
#define RUN_MULTIPLIER 3
#define CAM_HEIGHT 2.6f

#define FORW_SPEED 200.0f
#define SIDE_SPEED 350.0f
#define FRICTION 4
#define STOP_SPEED 100.0f
#define MAX_SPEED 320.0f
#define AIR_MAX_SPEED 30.0f
#define ACCELERATE 30.0f
#define JUMP_FORCE 270.0f
#define MOVE_SCALAR 0.03f
#define GRAVITY 800 
#define HEADBOB_SPEED 8

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
		c = camera_accelerate(c, wish_dir, wish_speed, ACCELERATE, dt);
		c.vel[1] -= GRAVITY * dt;
		c.grounded_timer += dt * 20.94393f;
	} else {
		c = camera_air_accelerate(c, wish_dir,
				wish_speed, ACCELERATE, dt);
		c.vel[1] -= GRAVITY * dt;
		c.grounded_timer = 0;
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

	bool grounded = camera_is_grounded(c);
	bool jump_pressed = i.jump_diff && i.jump_last;

	if(grounded) {
		if(!jump_pressed && !c.jump_queued)
			c.first_jump = false;
		else {
			if(i.jump_last)
				c.init_jump = true;

			c.jump_queued = false;
		}

		c.pos_real[1] = 0;
	} else {
		if(jump_pressed)
			c.jump_queued = true;
	}

	// float jump_timer_last = c.jump_timer;
	if(c.init_jump) {
		if(!c.first_jump) {
			if(c.jump_timer >= 2 * PI) {
				c.vel[1] += JUMP_FORCE;
				c.first_jump = true;
				c.jump_timer = 0;
				c.init_jump = false;
			}
		} else {
			if(!c.jump_timer) {
				c.vel[1] += JUMP_FORCE;
				c.jump_timer = 0;
				c.init_jump = false;
			}
		}

		if(!c.first_jump)
			c.jump_timer += dt * 24;
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
	vec3_muladd(headbob, side,
			sinf(c.bob_timer * HEADBOB_SPEED) * 0.3f, headbob);
	vec3_muladd(headbob, up,
			-cosf(c.bob_timer * HEADBOB_SPEED * 2) * 0.2f, headbob);
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

	float jump_bounce = (cosf(c.jump_timer) - 1.0f) * 0.4f;
	float land_bob = (cosf(c.grounded_timer) - 1.0f);
	if(c.grounded_timer > 0.0f) {
		land_bob /= c.grounded_timer;
	}
	land_bob = lerpf(land_bob, 0.0f, minf(c.grounded_timer / 18.0f, 1.0f));

	vec3_muladd(c.pos_bob, (vec3){0, 1, 0}, CAM_HEIGHT, c.pos_bob);
	vec3_muladd(focus, (vec3){0, 1, 0}, CAM_HEIGHT, focus);
	vec3_muladd(c.pos_bob, (vec3){0, 1, 0}, land_bob, c.pos_bob);
	vec3_muladd(focus, (vec3){0, 1, 0}, land_bob, focus);
	vec3_muladd(c.pos_bob, (vec3){0, 1, 0}, jump_bounce, c.pos_bob);
	vec3_muladd(focus, (vec3){0, 1, 0}, jump_bounce, focus);

	static float side_vel_lerp = 0.0f;
	float side_vel_cur = vec3_dot(c.vel, side) / MAX_SPEED;
	side_vel_lerp = lerpf(side_vel_lerp, side_vel_cur * 0.08f, dt * 16);
	vec3 side_vel_vec;
	vec3_scale_to(side, side_vel_lerp, side_vel_vec);
	vec3_add(up, side_vel_vec, up);
	vec3_norm(up);
	mat4_lookat(o, c.pos_bob, focus, up);
	return c;
}
