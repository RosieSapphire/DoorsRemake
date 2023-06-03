#include "e_context.h"
#include "e_rlayer.h"
#include "e_shader.h"
#include "e_model.h"
#include "e_camera.h"
#include "e_texture.h"

#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1280
#define HEIGHT 720
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
	int tex_loc =       shader_get_loc(base_shader, "u_tex");
	int using_tex_loc = shader_get_loc(base_shader, "u_is_using_tex");

	mat4 proj;
	mat4_perspective(90.0f, ASPECT_RATIO, 0.1f, 128, proj);
	struct camera cam = {0};
	struct model room = model_load("models/room.glb");
	uint room_tex = texture_load("textures/checker.png");
	struct input input = {0};

	float time_last = context_get_time();
	float delta_time = 0.0f;
	context_set_cursor_locked(true);
	while(context_is_running()) {
		rlayer_bind_and_clear(layer, 0.05f, 0.1f, 0.2f, 1.0f);

		cam = camera_update_axis(cam, input);
		cam = camera_move(cam, input, delta_time);

		shader_use(base_shader);

		mat4 model;
		model_get_mat4(room, model);
		shader_uni_mat4(model_loc, model);

		mat4 view;
		cam = camera_get_mat4(cam, view, delta_time);
		shader_uni_mat4(view_loc, view);
		shader_uni_mat4(proj_loc, proj);
		shader_uni_int(tex_loc, 0);
		shader_uni_int(using_tex_loc, true);
		model_draw(room, room_tex);
		rlayer_unbind_all();
		rlayer_draw(layer);

		context_poll_and_swap_buffers();
		input = context_get_input(input);

		float time_now = context_get_time();
		delta_time = time_now - time_last;
		time_last = time_now;
	}

	texture_unload(&room_tex);
	rlayer_destroy(&layer);
	rlayers_terminate();
	shader_destroy(base_shader);
	model_unload(&room);
	context_terminate();

	return 0;
}
