#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "../csc_gft.h"

static void gtext2_gen_pos (v4f32 pos[6], float x, float y, float z, float w, float h)
{
	pos[0] = (v4f32){{x + 0, y + 0, z, 0}};
	pos[1] = (v4f32){{x + w, y + 0, z, 0}};
	pos[2] = (v4f32){{x + 0, y + h, z, 0}};
	pos[3] = (v4f32){{x + w, y + 0, z, 0}};
	pos[4] = (v4f32){{x + 0, y + h, z, 0}};
	pos[5] = (v4f32){{x + w, y + h, z, 0}};
}

static void gtext2_gen_uv (v2f32 uv[6], float x, float y, float w, float h)
{
	uv[0] = (v2f32){{x + 0, y + 0}};
	uv[1] = (v2f32){{x + w, y + 0}};
	uv[2] = (v2f32){{x + 0, y + h}};
	uv[3] = (v2f32){{x + w, y + 0}};
	uv[4] = (v2f32){{x + 0, y + h}};
	uv[5] = (v2f32){{x + w, y + h}};
}


static uint32_t gtext2_gen
(v4f32 pos[], v2f32 uv[], const char *text, struct gft_char c[], uint32_t n, float aw, float ah, float x, float y, float z, float sx, float sy)
{
	uint32_t i = 0;
	const uint8_t *p;
	for (p = (const uint8_t *)text; *p; p++)
	{
		if (i+6 > n) {return n;}
		// Calculate the vertex and texture coordinates
		float x2 = x + c[*p].bl * sx;
		float y2 = y + c[*p].bt * sy;
		float w = c[*p].bw * sx;
		float h = c[*p].bh * -sy;
		float tx = c[*p].tx;
		float ty = c[*p].ty;
		float tw = c[*p].bw / aw;
		float th = c[*p].bh / ah;
		// Advance the cursor to the start of the next character
		x += c[*p].ax * sx;
		y += c[*p].ay * sy;
		// Skip glyphs that have no pixels */
		if (!w || !h) {continue;}
		gtext2_gen_pos (pos + i, x2, y2, z, w, h);
		gtext2_gen_uv (uv + i, tx, ty, tw, th);
		i += 6;
	}
	return i;
}



/**
 * Render text using the currently loaded font and currently set font size.
 * Rendering starts at coordinates (x, y), z is always 0.
 * The pixel coordinates that the FreeType2 library uses are scaled by (sx, sy).
 */
static void gtext2_render_text
(
const char *text,
struct gft_char c[],
unsigned int aw, //Atlas width
unsigned int ah, //Atlas height
GLuint vbo_pos,
GLuint vbo_uv,
float x, //Coordinate x
float y, //Coordinate y
float z, //Coordinate z
float sx,
float sy
)
{

	v4f32 pos[6 * 100];
	v2f32 uv[6 * 100];
	uint32_t i = 0;
	i = gtext2_gen (pos, uv, text, c, 6*100, aw, ah, x, y, z, sx, sy);
	/* Draw all the character on the screen in one go */
	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
	glBufferData (GL_ARRAY_BUFFER, sizeof(v4f32)*i, pos, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
	glBufferData (GL_ARRAY_BUFFER, sizeof(v2f32)*i, uv, GL_DYNAMIC_DRAW);
	glDrawArrays (GL_TRIANGLES, 0, i);
}






struct gtext2_context
{
	GLuint tex;
	GLuint vao;
	GLuint vbo_pos;
	GLuint vbo_uv;
	GLint uniform_tex;
	GLint uniform_mvp;
	GLuint program;
	struct gft_char c[128];
	struct gft_atlas atlas;
	FT_Face face;
};


static void gtext2_context_init(struct gtext2_context * ctx, GLuint program)
{
	glUseProgram (program);
	ctx->uniform_tex = glGetUniformLocation (program, "tex0");
	ctx->uniform_mvp = glGetUniformLocation (program, "mvp");
	ctx->program = program;
	// Create a texture that will be used to hold all ASCII glyphs
	unsigned unit = 0;
	glActiveTexture (GL_TEXTURE0 + unit);
	glGenTextures (1, &ctx->tex);
	glBindTexture (GL_TEXTURE_2D, ctx->tex);
	glUniform1i (ctx->uniform_tex, unit);
	gft_init (ctx->face, ctx->c, &ctx->atlas);

	// Set up the VBO for our vertex data
	enum attr
	{
		ATTRIBUTE_POS = 0, // vertex shader: "layout (location = 0)"
		ATTRIBUTE_UV = 1,  // vertex shader: "layout (location = 1)"
	};
	glGenBuffers (1, &ctx->vbo_pos);
	glGenBuffers (1, &ctx->vbo_uv);
	glGenVertexArrays (1, &ctx->vao);
	glBindVertexArray (ctx->vao);
	glEnableVertexAttribArray (ATTRIBUTE_POS);
	glEnableVertexAttribArray (ATTRIBUTE_UV);
	glBindBuffer (GL_ARRAY_BUFFER, ctx->vbo_pos);
	glVertexAttribPointer (ATTRIBUTE_POS, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer (GL_ARRAY_BUFFER, ctx->vbo_uv);
	glVertexAttribPointer (ATTRIBUTE_UV, 2, GL_FLOAT, GL_FALSE, 0, 0);
}


static void gtext2_context_draw(struct gtext2_context * ctx, char const * text, float x, float y, float z, float sx, float sy, float mvp[4*4])
{
	glBindVertexArray (ctx->vao);
	glUseProgram (ctx->program);
	/* Use the texture containing the atlas */
	glBindTexture (GL_TEXTURE_2D, ctx->tex);
	glUniform1i (ctx->uniform_tex, 0);
	glUniformMatrix4fv (ctx->uniform_mvp, 1, GL_FALSE, (const GLfloat *) mvp);
	gtext2_render_text (text, ctx->c, ctx->atlas.w, ctx->atlas.h, ctx->vbo_pos, ctx->vbo_uv, x, y, z, sx, sy);
}
