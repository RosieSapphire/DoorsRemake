#include "e_shader.h"
#include "e_util.h"
#include "e_gldef.h"
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

uint shader_compile(const char *path, int type)
{
	uint shader = glCreateShader(type);
	char *source = file_read_data(path);
	GLint status;

	glShaderSource(shader, 1, (const char **)&source, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if(status)
		return shader;

	char log[512];

	glGetShaderInfoLog(shader, 512, NULL, log);
	printf("Failed to create shader from '%s': %s\n", path, log);
	free(source);

	return 0;
}

uint shader_create(const char *vert_path, const char *frag_path)
{
	uint vert_shader = shader_compile(vert_path, GL_VERTEX_SHADER);
	uint frag_shader = shader_compile(frag_path, GL_FRAGMENT_SHADER);
	uint program = glCreateProgram();

	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);
	glLinkProgram(program);

	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	return program;
}

uint shader_get_loc(uint s, const char *name)
{
	int loc = glGetUniformLocation(s, name);
	if(loc == -1) {
		fprintf(stderr, "Failed to find loc '%s' in shader %d.\n",
				name, s);
		exit(EXIT_FAILURE);
	}

	return loc;
}

void shader_use(uint s)
{
	glUseProgram(s);
}

void shader_uni_mat4(uint loc, mat4 m4)
{
	glUniformMatrix4fv(loc, 1, GL_FALSE, m4);
}

void shader_uni_vec3(uint loc, vec3 v3)
{
	glUniform3fv(loc, 1, v3);
}

void shader_uni_int(uint loc, int i)
{
	glUniform1i(loc, i);
}

void shader_destroy(uint shader)
{
	glDeleteProgram(shader);
}
