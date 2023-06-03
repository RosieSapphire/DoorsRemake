#include "e_texture.h"
#include "e_gldef.h"

#define STB_IMAGE_IMPLEMENTATION
#include "e_stb_image.h"

uint texture_load(const char *path)
{
	uint id;
	int width, height, comp;

	stbi_set_flip_vertically_on_load(1);

	uint8_t *data = stbi_load(path, &width, &height, &comp, 0);

	if(!data) {
		printf("Failed to load texture from '%s'\n", path);
		return 0;
	}

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	/* TODO: Add ability to change wrapping mode as a param */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	return id;

}

uint texture_create_empty(int width, int height)
{
	uint tex;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
			GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return tex;
}

void texture_unload(uint *tex)
{
	glDeleteTextures(1, tex);
}
