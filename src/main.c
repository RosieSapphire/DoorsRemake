#include "e_context.h"
#include "e_rlayer.h"
#include "e_shader.h"
#include "e_model.h"
#include "e_camera.h"

#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1920
#define HEIGHT 1080
#define ASPECT_RATIO ((float)WIDTH / (float)HEIGHT)

int main(void)
{
	context_init(WIDTH, HEIGHT, "DOORS (Remake)");
	rlayers_init();
	struct rlayer layer = rlayer_create(WIDTH, HEIGHT, REN_RGB);

	uint base_shader =
		shader_create("shaders/base.vert", "shaders/base.frag");
	int proj_loc =      shader_get_loc(base_shader, "u_proj");
	int view_loc =      shader_get_loc(base_shader, "u_view");
	int model_loc =     shader_get_loc(base_shader, "u_model");

	mat4 proj;
	mat4_perspective(70.0f, ASPECT_RATIO, 0.1f, 10.0f, proj);
	struct camera cam = {{0, 0, 0}, 0, 0};
	struct model room = model_load("models/room.glb");
	struct input input = {0};

	/*
	float time_last = context_get_time();
	float delta_time = 0.0f;
	bool just_hit_right_click = false;
	*/

	context_set_cursor_locked(true);

	while(context_is_running()) {
		rlayer_bind_and_clear(layer, 0.05f, 0.1f, 0.2f, 1.0f);

		mat4 view;
		mat4 model;
		mat4_identity(view);
		mat4_trans(view, (vec3){0, 0, -6});

		model_get_mat4(room, model);

		camera_update_input(&cam, input);
		camera_get_mat4(cam, view);

		shader_use(base_shader);
		shader_uni_mat4(model_loc, model);
		shader_uni_mat4(view_loc, view);
		shader_uni_mat4(proj_loc, proj);
		model_draw(room, 0);
		rlayer_unbind_all();
		rlayer_draw(layer);

		context_poll_and_swap_buffers();
		input = context_get_input(input);

		/*
		float time_now = context_get_time();
		delta_time = time_now - time_last;
		time_last = time_now;
		*/
	}

	rlayer_destroy(&layer);
	rlayers_terminate();
	shader_destroy(base_shader);
	model_unload(&room);
	context_terminate();

	return 0;
}
