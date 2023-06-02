#ifndef ENGINE_UTIL_H_
#define ENGINE_UTIL_H_

typedef unsigned int uint;
typedef float mat4[4][4];
typedef float vec3[3];
typedef float vec2[2];

#define EPSILON 0.0001f
#define PI      3.141592653589f
#define PI_INV  0.3183098861838710434f

#define RAD_TO_DEG  57.29577951309678781159f
#define DEG_TO_RAD  0.01745329251993888889f

char *file_read_data(const char *path);
void vec2_copy(vec2 src, vec2 dst);
void vec2_sub(vec2 a, vec2 b, vec2 o);
void mat4_zero(mat4 m);
void mat4_perspective(float fovd, float aspect,
		float near, float far, mat4 out);

#endif /* ENGINE_UTIL_H_ */
