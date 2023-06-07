#include "e_context.h"
#include "e_rlayer.h"
#include "e_shader.h"
#include "e_model.h"
#include "e_camera.h"
#include "e_texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DOOR_OPEN_DISTANCE 2.2f
#define ROOM_COUNT 100
#define ROOM_VARIANT_COUNT 6

static const int room_chances[ROOM_VARIANT_COUNT] = {
	0, 40, 26, 14, 18, 2
};

static int room_randomize(void)
{
	int ran = (rand() % 100) + 1;
	int cur_weight = 0;
	for(int j = 0; j < ROOM_VARIANT_COUNT; j++) {
		cur_weight += room_chances[j];
		if(ran <= cur_weight) {
			return j;
		}
	}

	return -1;
}

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
	cam.pos_real[0] = 2.4f;
	struct model door = model_load("models/door.glb");
	struct input input = {0};

	float time_last = context_get_time();
	float delta_time = 0.0f;
	context_set_cursor_locked(true);

	struct model rooms[ROOM_VARIANT_COUNT] = {
		model_load("models/roomstart.glb"), /* starting room */
		model_load("models/room0.glb"),     /* box small */
		model_load("models/room1.glb"),     /* box big */
		model_load("models/room2.glb"),     /* turn left */
		model_load("models/room3.glb"),     /* turn right */
		model_load("models/room4.glb"),     /* big corridor */
	};

	int total_percentage = 0;
	for(int i = 0; i < ROOM_VARIANT_COUNT; i++)
		total_percentage += room_chances[i];

	if(total_percentage != 100) {
		fprintf(stderr, "Different room chances "
				"do not add up to 100%% (instead: %d%%).\n",
				total_percentage);
		exit(EXIT_FAILURE);
	}

	float room_x_offsets[ROOM_COUNT] = {0};
	float room_z_offsets[ROOM_COUNT] = {0};

	srand((uint)time(NULL));
	int which_room[ROOM_COUNT] = {0};
	float z_accum = 0, x_accum = 0;
	for(int i = 0; i < ROOM_COUNT; i++) {
		if(i > 0)
			which_room[i] = room_randomize();

		struct model *cur_room = rooms + which_room[i];
		struct mesh *door_exit_spawn =
			model_find_mesh_by_name(*cur_room, "DoorExit");
		door_exit_spawn->is_visible = false;
		vec3 door_exit_spawn_pos;
		mat4_get_pos(door_exit_spawn->matrix, door_exit_spawn_pos);

		vec3 room_size;
		model_get_aabb_size(*cur_room, room_size);
		z_accum += door_exit_spawn_pos[2];
		x_accum += room_size[0];
		room_z_offsets[i] = z_accum - door_exit_spawn_pos[2];
		room_x_offsets[i] = x_accum - room_size[0];
	}

	while(context_is_running()) {
		rlayer_bind_and_clear(layer, 0.05f, 0.1f, 0.2f, 1.0f);

		cam = camera_update_axis(cam, input);
		cam = camera_move(cam, input, delta_time);

		mat4 view;
		cam = camera_get_mat4(cam, view, delta_time);

		static float door_timers[ROOM_COUNT] = {0.0f};
		static bool doors_opened[ROOM_COUNT] = {false};
		static int num_doors_open = 0;
		struct mesh *door_mesh = model_find_mesh_by_name(door, "Door");
		vec3_zero(door.pos);
		for(int i = 0; i < ROOM_VARIANT_COUNT; i++)
			vec3_zero(rooms[i].pos);

		int iend = num_doors_open + 1;
		for(int i = 0; i < iend; i++) {
			/* Update door animation if they are moving */
			if(i - 4 >= 0)
				doors_opened[i - 4] = false;

			if(doors_opened[i])
				door_timers[i] += delta_time * 2;
			else
				door_timers[i] -= delta_time * 2;

			door_timers[i] = clampf(door_timers[i], 0, 1);
			door_mesh->rot[1] = lerpf(0, -PI_HALF, door_timers[i]);

			/* Offset the current room relative
			 * to the previous ones */
			struct model *cur_room = rooms + which_room[i];
			if(i > 0) {
				cur_room->pos[0] = room_x_offsets[i];
				cur_room->pos[2] = room_z_offsets[i];
			}

			model_recalc_mesh_matrices(*cur_room);
			model_recalc_mesh_matrices(door);

			/* Do the same thing for the doors */
			struct mesh *door_exit_obj =
				model_find_mesh_by_name(*cur_room, "DoorExit");
			door_exit_obj->is_visible = false;

			vec3 door_exit_pos;
			mat4_get_pos(door_exit_obj->matrix, door_exit_pos);
			vec3_add(door.pos, door_exit_pos, door.pos);

			if(i < num_doors_open - 4)
				continue;

			/* Check for interaction with a given door */
			if(model_mesh_dist(door, "Door", cam.pos_real)
					<= DOOR_OPEN_DISTANCE &&
					i == num_doors_open) {
				doors_opened[i] = true;
				num_doors_open++;
			}

			model_draw(*cur_room, 0, base_shader, proj, view);
			model_draw(door, 0, base_shader, proj, view);
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
	model_unload(&door);
	model_unload(&rooms[0]);
	model_unload(&rooms[1]);
	context_terminate();

	return 0;
}
