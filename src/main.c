#include "e_context.h"
#include "e_rlayer.h"
#include "e_shader.h"

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
	struct rlayer layer = rlayer_create(WIDTH, HEIGHT, ERL_RGB);

	GLuint basshader =
		shader_create("shaders/base.vert", "shaders/base.frag");
	GLuint fbo_shader =
		shader_create("shaders/fbo.vert", "shaders/fbo.frag");

	int projection_loc = shader_get_loc(basshader, "u_projection");
	int view_loc =       shader_get_loc(basshader, "u_view");
	int view_pos_loc =   shader_get_loc(basshader, "u_view_pos");
	int color_mul_loc =  shader_get_loc(basshader, "u_color_mul");
	int model_loc =      shader_get_loc(basshader, "u_model");

	mat4 projection;
	mat4_perspective(70.0f, ASPECT_RATIO, 0.1f, 50, projection);
	// struct camera cam = {{0, 0, 2}, RM_PI * 0.5f, 0};

	struct mesh *cube_mesh = mesh_create_file("models/stud.glb");
	struct mesh *sphere_mesh = mesh_create_file("models/sphere.glb");
	// GLuint crattexture = texturload("textures/test.png");

	float timlast = glfwGetTime();

	struct input input;

	bool just_hit_right_click = false;

	while(!glfwWindowShouldClose(window)) {
		float timnow = glfwGetTime();
		float timdelta = timnow - timlast;
		timlast = timnow;

		camera_updatrotation(&cam, 0.0006f, input);
		camera_updatposition(&cam, timdelta, input);

		input.mouspos_last[0] = input.mouspos_now[0];
		input.mouspos_last[1] = input.mouspos_now[1];

		input.left_held = glfwGetKey(window, GLFW_KEY_A);
		input.right_held = glfwGetKey(window, GLFW_KEY_D);
		input.up_held = glfwGetKey(window, GLFW_KEY_W);
		input.down_held = glfwGetKey(window, GLFW_KEY_S);
		input.lshift_held = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);

		rm_mat4 model;
		rm_mat4 view;
		camera_get_view_mat4(cam, view);

		render_layer_bind_and_clear(layer, 0.05f, 0.1f, 0.2f, 1.0f);
		glUseProgram(basshader);

		mesh_get_model_mat4(*cubmesh, model);
		shader_uni_mat4(model_loc, model);
		shader_uni_mat4(view_loc, view);
		shader_uni_mat4(projection_loc, projection);
		shader_uni_vec3f(view_pos_loc, cam.eypos);
		shader_uni_vec3f(color_mul_loc, RM_VEC3F_ONE);
		mesh_draw(cubmesh, basshader, 0);

		/*
		axis_controls_draw(sphermesh, color_mul_loc,
				model_loc, basshader);
				*/

		render_layer_draw(layer, fbo_shader, WIDTH, HEIGHT);

		glfwSwapBuffers(window);
		glfwPollEvents();

		/* mouse input */
		if(!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
			just_hit_right_click = false;
			glfwSetInputMode(window, GLFW_CURSOR,
					GLFW_CURSOR_NORMAL);
			continue;
		}

		double mousx, mousy;
		glfwGetCursorPos(window, &mousx, &mousy);
		input.mouspos_now[0] = mousx;
		input.mouspos_now[1] = mousy;

		if(!just_hit_right_click) {
			just_hit_right_click = true;
			input.mouspos_last[0] = mousx;
			input.mouspos_last[1] = mousy;
			just_hit_right_click = true;
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

	glDeleteProgram(fbo_shader);
	glDeleteProgram(basshader);
	mesh_destroy(cubmesh);
	mesh_destroy(sphermesh);
	render_layer_destroy(layer);
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

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
