#include "e_context.h"
#include "e_rlayer.h"
#include "e_shader.h"
#include "e_model.h"
#include "e_camera.h"
#include "e_texture.h"

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

	mat4 proj;
	mat4_perspective(90.0f, ASPECT_RATIO, 0.1f, 128, proj);
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
		vec3 door_pos;
		mat4_get_pos(door_mesh->matrix, door_pos);
		vec3 a, b;
		vec3_copy(cam.pos_real, a);
		vec3_copy(door_pos, b);
		vec3 dist_vec;
		vec3_sub(b, a, dist_vec);
		float dist = vec3_len(dist_vec);

		static float door_timer = 0.0f;
		// if(dist <= 2.2f)
		if(input.run_last)
			door_timer += delta_time * 2;
		else
			door_timer -= delta_time * 2;

		door_timer = clampf(door_timer, 0, 1);
		door_mesh->rot[1] = lerpf(0, -PI_HALF, door_timer);

		model_recalc_mesh_matrices(room);
		model_draw(room, 0, base_shader, proj, view);
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
