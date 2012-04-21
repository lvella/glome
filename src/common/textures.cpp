#include <cstdlib>
#include <cmath>

#include <fstream>
#include <iostream>

extern "C" {
#include "simplexnoise1234.h"
}

#include "textures.hpp"

void create_circle_texture(int w, float p, int a0, int a1, GLuint& tex, bool gen_mipmap)
{
	struct elem {
		unsigned char l;
		unsigned char a;
	};
	int i, j;
	float cx, cy, d, tex_r, tex_r_lim;
	int level = 0;

	elem* texture = (elem*)malloc(w * w * sizeof(elem));

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gen_mipmap ? GL_NEAREST_MIPMAP_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	w *= 2;
	do {
		w /= 2;
		cx = cy = (w - 1) / 2.;
		tex_r = w / 2.;
		tex_r_lim = tex_r * p;

		for(i = 0; i < w; ++i)
		{
			for(j = 0; j < w; ++j)
			{
				float x;
				d = sqrtf(((i - cx) * (i - cx)) + ((j - cy) * (j - cy)));
				texture[(i * w) + j].l = 255u;
				texture[(i * w) + j].a = (d > tex_r) ? a0 : ((d < tex_r_lim) ? a1 : ( (x = (tex_r - d) / (tex_r - tex_r_lim), x*x) * a1 ));
			}
		}

		glTexImage2D(GL_TEXTURE_2D, level++, GL_LUMINANCE_ALPHA, w, w, 0,
				GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)texture);

	} while(gen_mipmap && w > 1);

	free(texture);
}

void create_spherical_texture(int size, GLuint& tex)
{
	struct elem {
		unsigned char l;
		unsigned char a;
	};

	elem* buffer = (elem *) malloc(size * size * sizeof(elem));
	float r = (float)size / 2.0;

	for(int i = 0; i < size; ++i)
	{
		for(int j = 0; j < size; ++j)
		{
			float d = hypotf(i - r, j - r);
			buffer[(i * size) + j].l = 255u;
			buffer[(i * size) + j].a = d > r ? 0u : (unsigned char)nearbyint(sqrtf(r*r - d*d) / r * 255.0);
		}
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, size, size, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(buffer);
}

GLuint create_noise_texture(int w, int h, float scale, const Vector2& offset)
{
	GLuint tex;
	float max = -1000.0f;
	float min = 1000.0f;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	float* fbuffer = new float[w * h];
	for(int i = 0; i < h; ++i) {
		float y = offset.y + i * scale;
		for(int j = 0; j < w; ++j) {
			float x = offset.x + j * scale;
			float noise = snoise2(x, y);
			fbuffer[i*w + j] = noise;
			if(noise > max)
				max = noise;
			if(noise < min)
				min = noise;
		}
	}
	float byte_scale = 255.0f / (max - min);

	uint8_t* buffer = new uint8_t[w * h];

	for(int i = 0; i < h; ++i) {
		for(int j = 0; j < w; ++j) {
			int idx = i*w + j;
			buffer[idx] = uint8_t((fbuffer[idx] - min) * byte_scale);
		}
	}
	delete[] fbuffer;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, w, h, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, (GLvoid*)buffer);
	delete[] buffer;

	return tex;
}
