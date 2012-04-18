#include "textures.hpp"
#include <cstdlib>
#include <cmath>

void create_circle_texture(int w, float p, int a0, int a1, GLuint& tex)
{
	// TODO: create mipmaps down to every size.
	struct elem {
		unsigned char l;
		unsigned char a;
	};
	int i, j;
	float cx, cy, d, tex_r, tex_r_lim;

	elem* texture = (elem*)malloc(w * w * sizeof(elem));

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
			texture[(i * w) + j + 1].a = (d > tex_r) ? a0 : ((d < tex_r_lim) ? a1 : ( (x = (tex_r - d) / (tex_r - tex_r_lim), x*x) * a1 ));
		}
	}

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, w, w, 0,
	GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, (GLvoid*)texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
