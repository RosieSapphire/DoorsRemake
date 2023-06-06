#include <GLFW/glfw3.h>
#include "e_context.h"
#include "e_gldef.h"
#include <stdio.h>

GLFWwindow *context_window = NULL;

void context_init(uint *width, uint *height, const char *title)
{
	/* setting up GLFW's funny goofy shit */
	glfwInit();
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	GLFWvidmode *vidmode = (GLFWvidmode *)glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, vidmode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, vidmode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, vidmode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, vidmode->refreshRate);
	glfwWindowHint(GLFW_DECORATED, false);
	/*
	vidmode->width >>= 1;
	vidmode->height >>= 1;
	*/
	*width = vidmode->width;
	*height = vidmode->height;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	context_window = glfwCreateWindow(vidmode->width,
			vidmode->height, title, NULL, NULL);
	glfwMakeContextCurrent(context_window);
	glfwSetInputMode(context_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/* Configure OpenGL rendering state */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

bool context_is_running(void)
{
	return !glfwGetKey(context_window, GLFW_KEY_ESCAPE) &&
		!glfwWindowShouldClose(context_window);
}

float context_get_time(void)
{
	return glfwGetTime();
}

void context_set_cursor_locked(bool toggle)
{
	uint setting = toggle ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
	glfwSetInputMode(context_window, GLFW_CURSOR, setting);
}

void context_get_mpos(vec2 pos)
{
	double x, y;
	glfwGetCursorPos(context_window, &x, &y);
	pos[0] = (float)x;
	pos[1] = (float)y;
}

void context_poll_and_swap_buffers(void)
{
	glfwSwapBuffers(context_window);
	glfwPollEvents();
}

void context_terminate(void)
{
	/* bye bye, window */
	glfwDestroyWindow(context_window);
	glfwTerminate();
}

struct input context_get_input(struct input i)
{
	uint w_now = glfwGetKey(context_window, GLFW_KEY_W);
	uint a_now = glfwGetKey(context_window, GLFW_KEY_A);
	uint s_now = glfwGetKey(context_window, GLFW_KEY_S);
	uint d_now = glfwGetKey(context_window, GLFW_KEY_D);
	uint jump_now = glfwGetKey(context_window, GLFW_KEY_SPACE);
	uint run_now = glfwGetKey(context_window, GLFW_KEY_LEFT_SHIFT);

	i.w_diff = w_now - i.w_last;
	i.a_diff = a_now - i.a_last;
	i.s_diff = s_now - i.s_last;
	i.d_diff = d_now - i.d_last;
	i.jump_diff =  jump_now - i.jump_last;
	i.run_diff = run_now - i.run_last;

	i.w_last = w_now;
	i.a_last = a_now;
	i.s_last = s_now;
	i.d_last = d_now;
	i.jump_last = jump_now;
	i.run_last = run_now;

	vec2 mpos;
	context_get_mpos(mpos);
	vec2_sub(mpos, i.mpos_last, i.mpos_diff);
	vec2_copy(mpos, i.mpos_last);
	return i;
}

void context_print_input(struct input i)
{
	printf("INPUT:\n");
	printf("\tW Press: %d\n", i.w_diff);
	printf("\tA Press: %d\n", i.a_diff);
	printf("\tS Press: %d\n", i.s_diff);
	printf("\tS Press: %d\n", i.s_diff);
	printf("\tSPC Press: %d\n", i.jump_diff);
	printf("\tSHFT Press: %d\n", i.run_diff);
	printf("\tMMove X: %f\n", i.mpos_diff[0]);
	printf("\tMMove Y: %f\n", i.mpos_diff[1]);
}
