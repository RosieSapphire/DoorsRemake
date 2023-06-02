#version 330 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_uv;
layout(location = 2) in vec3 a_norm;
layout(location = 3) in vec3 a_tan;
layout(location = 4) in vec3 a_bitan;

uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 o_uv;
out vec3 o_norm;
out vec3 o_frag_pos;

void main()
{
	mat4 mvp = u_proj * u_view * u_model;
	vec4 vertex_pos = vec4(a_pos, 1.0);

	gl_Position = mvp * vertex_pos;
	o_uv = a_uv;
	o_norm = mat3(transpose(inverse(u_model))) * normalize(a_norm);
	o_frag_pos = vec3(u_model * vertex_pos);
}
