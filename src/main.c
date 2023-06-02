#include "e_context.h"
#include "e_rlayer.h"
#include "e_shader.h"
#include "e_model.h"

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
	struct rlayer layer = rlayer_create(WIDTH, HEIGHT, REN_RGB);

	/*
	GLuint base_shader =
		shader_create("shaders/base.vert", "shaders/base.frag");
	GLuint fbo_shader =
		shader_create("shaders/fbo.vert", "shaders/fbo.frag");
		*/

	/*
	int projection_loc = shader_get_loc(base_shader, "u_projection");
	int view_loc =       shader_get_loc(base_shader, "u_view");
	int view_pos_loc =   shader_get_loc(base_shader, "u_view_pos");
	int color_mul_loc =  shader_get_loc(base_shader, "u_color_mul");
	int model_loc =      shader_get_loc(base_shader, "u_model");
	*/

	mat4 projection;
	mat4_perspective(70.0f, ASPECT_RATIO, 0.1f, 50, projection);
	// struct camera cam = {{0, 0, 2}, RM_PI * 0.5f, 0};

	/*
	struct model cube_model = model_load("models/stud.glb");
	struct model sphere_model = model_load("models/sphere.glb");
	*/
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
		/*
		rm_mat4 model;
		rm_mat4 view;
		camera_get_view_mat4(cam, view);

		glUseProgram(basshader);

		mesh_get_model_mat4(*cubmesh, model);
		shader_uni_mat4(model_loc, model);
		shader_uni_mat4(view_loc, view);
		shader_uni_mat4(projection_loc, projection);
		shader_uni_vec3f(view_pos_loc, cam.eypos);
		shader_uni_vec3f(color_mul_loc, RM_VEC3F_ONE);
		mesh_draw(cubmesh, basshader, 0);
		render_layer_draw(layer, fbo_shader, WIDTH, HEIGHT);
		*/

		context_poll_and_swap_buffers();

		/*
		float time_now = context_get_time();
		delta_time = time_now - time_last;
		time_last = time_now;
		*/
	}

	rlayer_destroy(&layer);
	/*
	glDeleteProgram(fbo_shader);
	glDeleteProgram(basshader);
	mesh_destroy(cubmesh);
	mesh_destroy(sphermesh);
	*/
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
