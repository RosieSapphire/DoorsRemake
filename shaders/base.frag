#version 330 core

in vec2 o_uv;
in vec3 o_norm;
in vec3 o_frag_pos;

uniform sampler2D u_tex;
uniform bool u_is_using_tex;

out vec4 frag_color;

void main()
{
	vec3 light_pos = vec3(0, 2, 3);
	vec3 light_dir = normalize(light_pos - o_frag_pos);
	vec3 light_col = vec3(0.8, 0.6, 0.5);
	vec3 ambient = 0.1 * light_col;
	vec3 diffuse = max(dot(o_norm, light_dir), 0.0) * light_col;

	vec3 base = vec3(1);

	if(u_is_using_tex) {
 		base = texture(u_tex, o_uv).xyz;
	}

	frag_color = vec4((ambient + diffuse) * base, 1.0);
}
