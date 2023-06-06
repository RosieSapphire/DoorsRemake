#include "e_context.h"
#include "e_rlayer.h"
#include "e_shader.h"
#include "e_model.h"
#include "e_camera.h"
#include "e_texture.h"

#include <stdio.h>
#include <stdlib.h>

#define DOOR_OPEN_DISTANCE 2.2f

int main(void)
{
	uint win_width, win_height;
	context_init(&win_width, &win_height, "DOORS (Remake)");
	rlayers_init();
	struct rlayer layer = rlayer_create(win_width, win_height, REN_RGB);

	uint base_shader =
		shader_create("shaders/base.vert", "shaders/base.frag");

	mat4 proj;
	float aspect_ratio = (float)win_width / (float)win_height;
	mat4_perspective(90.0f, aspect_ratio, 0.1f, 128, proj);
	struct camera cam = {0};
	struct model room = model_load("models/room.glb");
	struct input input = {0};

	float time_last = context_get_time();
	float delta_time = 0.0f;
	context_set_cursor_locked(true);
	while(context_is_running()) {
		rlayer_bind_and_clear(layer, 0.05f, 0.1f, 0.2f, 1.0f);

		cam = camera_update_axis(cam, input);
		cam = camera_move(cam, input, delta_time);

		mat4 view;
		cam = camera_get_mat4(cam, view, delta_time);
		
		struct mesh *door_mesh = model_find_mesh_by_name(room, "Door");
		room.pos[0] = 0;

		static float door_timers[5] = {0.0f};
		static bool doors_opened[5] = {false};
		for(int i = 0; i < 5; i++) {
			if(model_mesh_dist_point(room, "Door", cam.pos_real)
					<= DOOR_OPEN_DISTANCE)
				doors_opened[i] = true;

			if(doors_opened[i]) {
				door_timers[i] += delta_time * 2;
				door_timers[i] = clampf(door_timers[i], 0, 1);
			}

			door_mesh->rot[1] = lerpf(0, -PI_HALF, door_timers[i]);
			model_recalc_mesh_matrices(room);
			model_draw(room, 0, base_shader, proj, view);
			room.pos[0] += 9.6f;
		}
		rlayer_unbind_all();
		rlayer_draw(layer);

		context_poll_and_swap_buffers();
		input = context_get_input(input);

		float time_now = context_get_time();
		delta_time = time_now - time_last;
		time_last = time_now;
	}

	rlayer_destroy(&layer);
	rlayers_terminate();
	shader_destroy(base_shader);
	model_unload(&room);
	context_terminate();

	return 0;
}
