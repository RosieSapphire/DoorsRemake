#include <GLFW/glfw3.h>
#include "e_context.h"
#include "e_gldef.h"

GLFWwindow *context_window = NULL;

void context_init(uint width, uint height, const char *title)
{
	/* setting up GLFW's funny goofy shit */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	context_window = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwMakeContextCurrent(context_window);
	glfwSetInputMode(context_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glEnable(GL_DEPTH);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glfwMakeContextCurrent(context_window);

	/* center the monitor */
	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	int monitor_w, monitor_h;
	int window_x, window_y;
	glfwGetMonitorWorkarea(monitor, NULL, NULL, &monitor_w, &monitor_h);
	window_x = (monitor_w >> 1) - (width >> 1);
	window_y = (monitor_h >> 1) - (height >> 1);
	glfwSetWindowPos(context_window, window_x, window_y);
}

void context_terminate()
{
	/* bye bye, window */
	glfwDestroyWindow(context_window);
	glfwTerminate();
}
