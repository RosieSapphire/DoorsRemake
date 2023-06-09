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

float maxf(float a, float b)
{
	if(a >= b)
		return a;

	return b;
}

float minf(float a, float b)
{
	if(a <= b)
		return a;

	return b;
}

float clampf(float x, float min, float max)
{
	return minf(maxf(x, min), max);
}

float lerpf(float a, float b, float t)
{
	return a + (b - a) * clampf(t, 0, 1);
}

void vec2_copy(vec2 src, vec2 dst)
{
	bcopy(src, dst, sizeof(vec2));
}

void vec3_zero(vec3 x)
{
	bzero(x, sizeof(vec3));
}

void vec3_one(vec3 x)
{
	memset(x, 1, sizeof(vec3));
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

void vec3_negate_to(vec3 i, vec3 o)
{
	vec3_scale_to(i, -1, o);
}	

void vec3_negate(vec3 x)
{
	vec3_negate_to(x, x);
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

void vec3_scale_to(vec3 i, float s, vec3 o)
{
	o[0] = i[0] * s;
	o[1] = i[1] * s;
	o[2] = i[2] * s;
}

void vec3_scale(vec3 x, float s)
{
	vec3_scale_to(x, s, x);
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

void vec3_printf(vec3 x)
{
	printf("%f, %f, %f\n", x[0], x[1], x[2]);
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

void mat4_mul_vec3(mat4 m, vec3 o)
{
	o[0] = m[0][0] * o[0] + m[1][0] * o[1] + m[2][0] * o[2] + m[3][0] * 1.0f;
	o[1] = m[0][1] * o[0] + m[1][1] * o[1] + m[2][1] * o[2] + m[3][1] * 1.0f;
	o[2] = m[0][2] * o[0] + m[1][2] * o[1] + m[2][2] * o[2] + m[3][2] * 1.0f;
}

void mat4_invert(mat4 mat)
{
	float a = mat[0][0], b = mat[0][1], c = mat[0][2], d = mat[0][3],
	      e = mat[1][0], f = mat[1][1], g = mat[1][2], h = mat[1][3],
	      i = mat[2][0], j = mat[2][1], k = mat[2][2], l = mat[2][3],
	      m = mat[3][0], n = mat[3][1], o = mat[3][2], p = mat[3][3];
	
	float t[6];
	t[0] = k * p - o * l; t[1] = j * p - n * l; t[2] = j * o - n * k;
	t[3] = i * p - m * l; t[4] = i * o - m * k; t[5] = i * n - m * j;
	
	mat[0][0] =  f * t[0] - g * t[1] + h * t[2];
	mat[1][0] =-(e * t[0] - g * t[3] + h * t[4]);
	mat[2][0] =  e * t[1] - f * t[3] + h * t[5];
	mat[3][0] =-(e * t[2] - f * t[4] + g * t[5]);
	
	mat[0][1] =-(b * t[0] - c * t[1] + d * t[2]);
	mat[1][1] =  a * t[0] - c * t[3] + d * t[4];
	mat[2][1] =-(a * t[1] - b * t[3] + d * t[5]);
	mat[3][1] =  a * t[2] - b * t[4] + c * t[5];
	
	t[0] = g * p - o * h; t[1] = f * p - n * h; t[2] = f * o - n * g;
	t[3] = e * p - m * h; t[4] = e * o - m * g; t[5] = e * n - m * f;
	
	mat[0][2] =  b * t[0] - c * t[1] + d * t[2];
	mat[1][2] =-(a * t[0] - c * t[3] + d * t[4]);
	mat[2][2] =  a * t[1] - b * t[3] + d * t[5];
	mat[3][2] =-(a * t[2] - b * t[4] + c * t[5]);
	
	t[0] = g * l - k * h; t[1] = f * l - j * h; t[2] = f * k - j * g;
	t[3] = e * l - i * h; t[4] = e * k - i * g; t[5] = e * j - i * f;
	
	mat[0][3] =-(b * t[0] - c * t[1] + d * t[2]);
	mat[1][3] =  a * t[0] - c * t[3] + d * t[4];
	mat[2][3] =-(a * t[1] - b * t[3] + d * t[5]);
	mat[3][3] =  a * t[2] - b * t[4] + c * t[5];
	
	float det = 1.0f / (a * mat[0][0] + b * mat[1][0]
	            + c * mat[2][0] + d * mat[3][0]);
	
	mat4_scale(mat, det);
}

void mat4_scale(mat4 x, float s)
{
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
			x[i][j] *= s;
}

void mat4_rot(mat4 m, vec3 axis, float angle_rad)
{
	float sina = sinf(angle_rad);
	float cosa = cosf(angle_rad);
	
	mat4 rotation = MAT4_IDENTITY_INIT;
	float x = axis[0];
	float y = axis[1];
	float z = axis[2];
	
	rotation[0][0] = cosa + (1.0f - cosa) * x * x;
	rotation[1][0] = (1.0f - cosa) * x * y - sina * z;
	rotation[2][0] = (1.0f - cosa) * x * z + sina * y;
	
	rotation[0][1] = (1.0f - cosa) * y * x + sina * z;
	rotation[1][1] = cosa + (1.0f - cosa) * y * y;
	rotation[2][1] = (1.0f - cosa) * y * z - sina * x;
	
	rotation[0][2] = (1.0f - cosa) * z * x - sina * y;
	rotation[1][2] = (1.0f - cosa) * z * y + sina * x;
	rotation[2][2] = cosa + (1.0f - cosa) * z * z;

	mat4_mul(m, rotation, m);
}

void mat4_rot_euler(mat4 m, vec3 euler)
{
	mat4_rot(m, (vec3){1, 0, 0}, euler[0]);
	mat4_rot(m, (vec3){0, 1, 0}, euler[1]);
	mat4_rot(m, (vec3){0, 0, 1}, euler[2]);
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

	mat4_zero(m);
	m[0][0] = s[0];
	m[0][1] = u[0];
	m[0][2] = f[0];

	m[1][0] = s[1];
	m[1][1] = u[1];
	m[1][2] = f[1];

	m[2][0] = s[2];
	m[2][1] = u[2];
	m[2][2] = f[2];

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

void mat4_get_pos(mat4 m, vec3 o)
{
	vec3_copy(m[3], o);
}

void mat4_printf(mat4 m)
{
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++)
			printf("%f ", m[j][i]);

		printf("\n");
	}
	printf("\n");
}
