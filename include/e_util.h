#ifndef ENGINE_UTIL_H_
#define ENGINE_UTIL_H_

typedef unsigned int uint;
typedef float mat4[4][4];
typedef float vec3[3];
typedef float vec2[2];

#define EPSILON 0.0001f
#define PI      3.141592653589f
#define PI_HALF (PI * 0.5f)
#define PI_INV  0.3183098861838710434f

#define RAD_TO_DEG  57.29577951309678781159f
#define DEG_TO_RAD  0.01745329251993888889f

char *file_read_data(const char *path);
float clampf(float x, float min, float max);
void vec2_copy(vec2 src, vec2 dst);
void vec2_sub(vec2 a, vec2 b, vec2 o);
void vec3_copy(vec3 src, vec3 dst);
void vec3_add(vec3 a, vec3 b, vec3 o);
void vec3_sub(vec3 a, vec3 b, vec3 o);
float vec3_dot(vec3 a, vec3 b);
void vec3_cross(vec3 a, vec3 b, vec3 o);
float vec3_len(vec3 x);
float vec3_norm(vec3 x);
void mat4_copy(mat4 src, mat4 dst);
void mat4_zero(mat4 m);
void mat4_identity(mat4 m);
void mat4_trans(mat4 m, vec3 v);
void mat4_mul(mat4 a, mat4 b, mat4 o);
void mat4_rot(mat4 m, vec3 axis, float angle_rad);
void mat4_lookat(mat4 m, vec3 eye, vec3 focus, vec3 up);
void mat4_perspective(float fovd, float aspect,
		float near, float far, mat4 out);

#endif /* ENGINE_UTIL_H_ */
