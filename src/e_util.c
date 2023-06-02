#include "e_util.h"

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

char *file_read_data(const char *path)
{
	FILE *file = fopen(path, "r");
	size_t file_size = 0L;
	char *buffer = NULL;
	fseek(file, 0, SEEK_END);
	file_size = ftell(file);
	rewind(file);
	buffer = malloc(file_size);
	fread(buffer, sizeof(char), file_size, file);
	fclose(file);
	buffer[--file_size] = 0;
	return buffer;
}

void mat4_zero(mat4 m)
{
	bzero(m, sizeof(mat4));
}

void mat4_perspective(float fovd, float aspect, float near, float far, mat4 out)
{
	float fovitan = 1.0f / tanf(fovd * DEG_TO_RAD * 0.5f);
	float z_range = 1.0f / (near - far);

	mat4_zero(out);
	out[0][0] = fovitan / aspect;
	out[1][1] = fovitan;
	out[2][2] = (near + far) * z_range;
	out[2][3] = -1.0f;
	out[3][2] = 2.0f * near * far * z_range;
}
