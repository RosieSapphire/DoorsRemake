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

float clampf(float x, float min, float max)
{
	if(x > max)
		return max;

	if(x < min)
		return min;

	return x;
}

void vec2_copy(vec2 src, vec2 dst)
{
	bcopy(src, dst, sizeof(vec2));
}

void vec2_sub(vec2 a, vec2 b, vec2 o)
{
	o[0] = a[0] - b[0];
	o[1] = a[1] - b[1];
}

void vec3_copy(vec3 src, vec3 dst)
{
	bcopy(src, dst, sizeof(vec3));
}

void vec3_add(vec3 a, vec3 b, vec3 o)
{
	o[0] = a[0] + b[0];
	o[1] = a[1] + b[1];
	o[2] = a[2] + b[2];
}

void vec3_muladd(vec3 a, vec3 b, float bs, vec3 o)
{
	o[0] = a[0] + b[0] * bs;
	o[1] = a[1] + b[1] * bs;
	o[2] = a[2] + b[2] * bs;
}

void vec3_sub(vec3 a, vec3 b, vec3 o)
{
	o[0] = a[0] - b[0];
	o[1] = a[1] - b[1];
	o[2] = a[2] - b[2];
}

void vec3_scale(vec3 x, float s)
{
	x[0] *= s;
	x[1] *= s;
	x[2] *= s;
}

float vec3_dot(vec3 a, vec3 b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void vec3_cross(vec3 a, vec3 b, vec3 o)
{
	o[0] = a[1] * b[2] - a[2] * b[1];
	o[1] = a[2] * b[0] - a[0] * b[2];
	o[2] = a[0] * b[1] - a[1] * b[0];
}

float vec3_len(vec3 x)
{
	float mag = sqrtf(vec3_dot(x, x));
	if(!mag)
		return 0;

	return mag;
}

float vec3_norm(vec3 x)
{
	float mag = vec3_len(x);
	if(!mag)
		return 0;

	x[0] /= mag;
	x[1] /= mag;
	x[2] /= mag;

	return mag;
}

void mat4_copy(mat4 src, mat4 dst)
{
	bcopy(src, dst, sizeof(mat4));
}

void mat4_zero(mat4 m)
{
	bzero(m, sizeof(mat4));
}

void mat4_identity(mat4 m)
{
	mat4_zero(m);
	for(int i = 0; i < 4; i++)
		m[i][i] = 1;
}

void mat4_trans(mat4 m, vec3 v)
{
	vec3_add(m[3], v, m[3]);
}

void mat4_mul(mat4 a, mat4 b, mat4 o)
{
	mat4 a_tmp;
	mat4_copy(a, a_tmp);
	mat4_zero(o);

	for(int row = 0; row < 4; row++)
		for(int col = 0; col < 4; col++)
			for(int ind = 0; ind < 4; ind++)
				o[col][row] += a_tmp[ind][row] * b[col][ind];
}

void mat4_rot(mat4 m, vec3 axis, float angle_rad)
{
	mat4 rot;
	mat4_zero(rot);

	const float angle_sin = sinf(angle_rad);
	const float angle_cos = cosf(angle_rad);

	rot[0][0] = angle_cos + (axis[0] * axis[0]) * (1 - angle_cos);
	rot[1][0] = axis[0] * axis[1] * (1 - angle_cos) - axis[2] * angle_sin;
	rot[2][0] = axis[0] * axis[2] * (1 - angle_cos) + axis[1] * angle_sin;

	rot[0][1] = axis[1] * axis[0] * (1 - angle_cos) + axis[2] * angle_sin;
	rot[1][1] = angle_cos + (axis[1] * axis[1]) * (1 - angle_cos);
	rot[2][1] = axis[1] * axis[2] * (1 - angle_cos) - axis[0] * angle_sin;

	rot[0][2] = axis[2] * axis[0] * (1 - angle_cos) - axis[1] * angle_sin;
	rot[1][2] = axis[2] * axis[1] * (1 - angle_cos) + axis[0] * angle_sin;
	rot[2][2] = angle_cos + (axis[2] * axis[2]) * (1 - angle_cos);

	rot[3][3] = 1.0f;

	mat4_mul(m, rot, m);
}

void mat4_lookat(mat4 m, vec3 eye, vec3 focus, vec3 up)
{
	vec3 f;
	vec3_sub(focus, eye, f);
	vec3_norm(f);

	vec3 s;
	vec3_cross(up, f, s);
	vec3_norm(s);

	vec3 u;
	vec3_cross(f, s, u);

	m[0][0] = s[0];
	m[0][1] = u[0];
	m[0][2] = f[0];
	m[0][3] = 0.0f;

	m[1][0] = s[1];
	m[1][1] = u[1];
	m[1][2] = f[1];
	m[1][3] = 0.0f;

	m[2][0] = s[2];
	m[2][1] = u[2];
	m[2][2] = f[2];
	m[2][3] = 0.0f;

	m[3][0] = -vec3_dot(s, eye);
	m[3][1] = -vec3_dot(u, eye);
	m[3][2] = -vec3_dot(f, eye);
	m[3][3] = 1.0f;
}

void mat4_perspective(float fovd, float aspect,
		float near, float far, mat4 out)
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

void mat4_printf(mat4 m)
{
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++)
			printf("%f ", m[i][j]);

		printf("\n");
	}
	printf("\n");
}
