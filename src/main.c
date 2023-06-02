#include "e_context.h"
#include "e_rlayer.h"
#include "e_shader.h"
#include "e_model.h"

#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1920
#define HEIGHT 1080
#define ASPECT_RATIO ((float)WIDTH / (float)HEIGHT)

/*
 * TODO: Move this crap somewhere else
void axis_controls_draw(struct mesh *sphermesh,
		GLuint color_mul_loc, GLuint model_loc,
		GLuint shader);
*/

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
	mat4_perspective(90.0f, ASPECT_RATIO, 0.1f, 10.0f, proj);
	// struct camera cam = {{0, 0, 2}, RM_PI * 0.5f, 0};

	struct model cube = model_load("models/shape.glb");
	struct input input;

	/*
	float time_last = context_get_time();
	float delta_time = 0.0f;
	bool just_hit_right_click = false;
	*/

	context_set_cursor_locked(true);

	while(context_is_running()) {
		input = context_get_input(input);

		rlayer_bind_and_clear(layer, 0.05f, 0.1f, 0.2f, 1.0f);

		mat4 view;
		mat4 model;
		mat4_identity(view);
		mat4_trans(view, (vec3){0, 0, -6});
		cube.pos[0] = context_get_time();
		model_get_mat4(cube, model);
		// camera_get_view_mat4(cam, view);

		shader_use(base_shader);
		shader_uni_mat4(model_loc, model);
		shader_uni_mat4(view_loc, view);
		shader_uni_mat4(proj_loc, proj);
		model_draw(cube, 0);
		rlayer_unbind_all();
		rlayer_draw(layer);

		context_poll_and_swap_buffers();

		/*
		float time_now = context_get_time();
		delta_time = time_now - time_last;
		time_last = time_now;
		*/
	}

	rlayer_destroy(&layer);
	rlayers_terminate();
	shader_destroy(base_shader);
	model_unload(&cube);
	context_terminate();

	return 0;
}

/*
void axis_controls_draw(struct mesh *sphermesh,
		GLuint color_mul_loc, GLuint model_loc,
		GLuint shader)
{
	rm_mat4 model;
	rm_vec3f axis[3] = {
		RM_VEC3F_X_INIT,
		RM_VEC3F_Y_INIT,
		RM_VEC3F_Z_INIT,
	};

	float offset_values[2] = {
		0.28f, -0.28f
	};

	for(int k = 0; k < 2; k++) {
		for(int i = 0; i < 3; i++) {
			shader_uni_vec3f(color_mul_loc, axis[i]);

			for(int j = 0; j < 3; j++) {
				if(j == i) {
					sphermesh->pos[j] = offset_values[k];
					continue;
				}

				sphermesh->pos[j] = 0.0f;
			}

			mesh_get_model_mat4(*sphermesh, model);
			shader_uni_mat4(model_loc, model);
			mesh_draw(sphermesh, shader, 0);
		}
	}
}
*/
