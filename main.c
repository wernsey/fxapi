#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "bmp.h"
#include "fx.h"

double vertices[][3] = {
	{-1,-1,-1},
	{-1,-1,+1},
	{-1,+1,-1},
	{-1,+1,+1},
	{+1,-1,-1},
	{+1,-1,+1},
	{+1,+1,-1},
	{+1,+1,+1},
};

static Bitmap *tex = NULL;
static Bitmap *pick_buffer = NULL;

int main(int argc, char *argv[]) {
	
	Bitmap *viewport = bm_create(320, 240);
	fx_set_viewport(viewport);

	/* The pick buffer is just a second bitmap that will be colored
	in as the renderer draws pixels. It uses
	The intention is that you 
	You need to manage its memory yourself. */
	pick_buffer = bm_create(viewport->w, viewport->h);
	if(!pick_buffer)
	if(!tex) { 
		fprintf(stderr, "couldn't create pick buffer\n");
		return 1;
	}
	fx_set_pick(pick_buffer);

	
	bm_set_color(viewport, 0xFF0000);
	bm_printf(viewport, 10, 10, "Hello World");

	/* The renderer respects the target bitmap's clipping rectangle */
	bm_clip(viewport, 20, 20, viewport->w - 20, viewport->h - 20);

	/* The fog effect is a simple linear fog with a color 
		and near/far parameters */
	fx_fog(1);
	fx_fog_params(0, 0, 0, 0.5, 1.0);

	/* You can enable/disable backface drawing */
	fx_backface(1);

	/* The blend mode uses a bit shifting trick to blend */	
	fx_blend(1);

	tex = bm_load("tile.gif");
	if(!tex) { 
		fprintf(stderr, "couldn't load tile.gif\n");
		return 1;
	}
	fx_set_texture(tex);

	/* To use transparent pixels on the texture map, set
		the color of the texture, and enable transparency */	
	fx_transparent(1);
	bm_set_color(tex, 0x404040);
	
	/* The clipping rectangle of the texture bitmap is
	respected. 
	A UV at (0,0) is the top left corner of the texture's clipping rectangle,
	and (1,1) is the bottom right of the clipping rectangle. 
	This feature is intended to make it easier to work with spritesheet texture maps */
	bm_clip(tex,0,0,11,11);
	

	bm_set_color(viewport, 0x0000055);
	bm_clear(viewport);

	bm_set_color(pick_buffer, 0x000000);
	bm_clear(pick_buffer);
	bm_set_color(pick_buffer, 0x00FF00);

	fx_clear_zbuf();

	mat4_t view = fx_get_view();
	double eyePos[] = {0,0,5},
		center[] = {0,0,0},
		up[] = {0, 1, 0};
	mat4_lookAt(eyePos, center, up, view);
	//fx_set_view(view);

	double model[16];
	static double theta = M_PI/4.0;
	double pos[] = {0,0,0};

	mat4_identity(model);
	mat4_translate(model, pos, NULL);
	mat4_rotateY(model, theta, NULL);

	fx_set_model(model);

#if 0
	fx_begin(FX_TRIANGLES);
	fx_vertex(-2, 1.5, 0); fx_texcoord(0, 0); fx_vcolor(1, 0, 0); 
	fx_vertex(2, -1, 0);   fx_texcoord(1, 1); fx_vcolor(0, 1, 0);
	fx_vertex(-2, -1, 0);  fx_texcoord(0, 1); fx_vcolor(0, 0, 1);

	fx_vertex(-2, 1.5, 0); fx_texcoord(0, 0); fx_vcolor(1, 0, 0);
	fx_vertex(2, 1.5, 0);  fx_texcoord(1, 0); fx_vcolor(1, 1, 1);
	fx_vertex(2, -1, 0);   fx_texcoord(1, 1); fx_vcolor(0, 1, 0);
	fx_end();
#elif 0
	fx_begin(FX_TRIANGLE_STRIP);
	fx_vertex(-1.5, 1.5, 0); fx_texcoord(0, 0);
	fx_vertex(-1.5, -1.5, 0); fx_texcoord(0, 1);
	fx_vertex(-0.5, 1.5, 0); fx_texcoord(1, 0);
	fx_vertex(-0.5, -1.5, 0); fx_texcoord(1, 1);
	fx_vertex(0.5, 1.5, 0); fx_texcoord(2, 0);
	fx_vertex(0.5, -1.5, 0); fx_texcoord(2, 1);
	fx_vertex(1.5, 1.5, 0); fx_texcoord(3, 0);
	fx_vertex(1.5, -1.5, 0); fx_texcoord(3, 1);
	fx_end();
#elif 0
	fx_begin(FX_TRIANGLE_FAN);
	fx_vertex(0, 0, 0); fx_texcoord(0, 1);
	fx_vertex(0, 1, 0); fx_texcoord(0, 0);
	fx_vertex(1, 1, 0); fx_texcoord(1, 0);
	fx_vertex(1, 0, 0); fx_texcoord(1, 1);
	fx_vertex(1, -1, 0); fx_texcoord(1, 2);
	fx_vertex(0, -1, 0); fx_texcoord(0, 2);
	fx_end();
#else
	
	fx_lighting(1);
	fx_set_diffuse_direction(-0.5, -1.0, -0.5);
	fx_set_diffuse_color(1.0, 1, 1);
	fx_set_ambient(0.5, 0.5, 0.5);

	double normals[8][3];
	int i;
	for(i = 0; i < 8; i++) {
		vec3_normalize(vertices[i], normals[i]);
	}
	
	// bottom	
	fx_begin(FX_TRIANGLES);
		fx_vertex_v3(vertices[1]); fx_texcoord(0, 0); fx_normal_v3(normals[1]);
		fx_vertex_v3(vertices[0]); fx_texcoord(1, 0); fx_normal_v3(normals[0]);
		fx_vertex_v3(vertices[4]); fx_texcoord(1, 1); fx_normal_v3(normals[4]);
		fx_vertex_v3(vertices[5]); fx_texcoord(0, 1); fx_normal_v3(normals[5]);
		fx_vertex_v3(vertices[1]); fx_texcoord(0, 0); fx_normal_v3(normals[1]);
		fx_vertex_v3(vertices[4]); fx_texcoord(1, 1); fx_normal_v3(normals[4]);
	fx_end();

	// top
	fx_begin(FX_TRIANGLES);
		fx_vertex_v3(vertices[2]); fx_texcoord(0, 0); fx_normal_v3(normals[2]);
		fx_vertex_v3(vertices[3]); fx_texcoord(0, 1); fx_normal_v3(normals[3]);
		fx_vertex_v3(vertices[6]); fx_texcoord(1, 0); fx_normal_v3(normals[6]);
		fx_vertex_v3(vertices[7]); fx_texcoord(1, 1); fx_normal_v3(normals[7]);
		fx_vertex_v3(vertices[6]); fx_texcoord(1, 0); fx_normal_v3(normals[6]);
		fx_vertex_v3(vertices[3]); fx_texcoord(0, 1); fx_normal_v3(normals[3]);
	fx_end();

	// back 
	fx_begin(FX_TRIANGLES);
		fx_vertex_v3(vertices[0]); fx_texcoord(1, 1); fx_normal_v3(normals[0]);
		fx_vertex_v3(vertices[2]); fx_texcoord(1, 0); fx_normal_v3(normals[2]);
		fx_vertex_v3(vertices[4]); fx_texcoord(0, 1); fx_normal_v3(normals[4]);
		fx_vertex_v3(vertices[4]); fx_texcoord(0, 1); fx_normal_v3(normals[4]);
		fx_vertex_v3(vertices[2]); fx_texcoord(1, 0); fx_normal_v3(normals[2]);
		fx_vertex_v3(vertices[6]); fx_texcoord(0, 0); fx_normal_v3(normals[6]);
	fx_end();

	// front
	fx_begin(FX_TRIANGLES);
		fx_vertex_v3(vertices[7]); fx_texcoord(1, 0); fx_normal_v3(normals[7]);
		fx_vertex_v3(vertices[3]); fx_texcoord(0, 0); fx_normal_v3(normals[3]);
		fx_vertex_v3(vertices[5]); fx_texcoord(1, 1); fx_normal_v3(normals[5]);
		fx_vertex_v3(vertices[5]); fx_texcoord(1, 1); fx_normal_v3(normals[5]);
		fx_vertex_v3(vertices[3]); fx_texcoord(0, 0); fx_normal_v3(normals[3]);
		fx_vertex_v3(vertices[1]); fx_texcoord(0, 1); fx_normal_v3(normals[1]);
	fx_end();

	// left
	fx_begin(FX_TRIANGLES);
		fx_vertex_v3(vertices[0]); fx_texcoord(0, 1); fx_normal_v3(normals[0]);
		fx_vertex_v3(vertices[1]); fx_texcoord(1, 1); fx_normal_v3(normals[1]);
		fx_vertex_v3(vertices[3]); fx_texcoord(1, 0); fx_normal_v3(normals[3]);
		fx_vertex_v3(vertices[2]); fx_texcoord(0, 0); fx_normal_v3(normals[2]);
		fx_vertex_v3(vertices[0]); fx_texcoord(0, 1); fx_normal_v3(normals[0]);
		fx_vertex_v3(vertices[3]); fx_texcoord(1, 0); fx_normal_v3(normals[3]);
	fx_end();

	// right
	fx_begin(FX_TRIANGLES);
		fx_vertex_v3(vertices[6]); fx_texcoord(1, 0); fx_normal_v3(normals[6]);
		fx_vertex_v3(vertices[7]); fx_texcoord(0, 0); fx_normal_v3(normals[7]);
		fx_vertex_v3(vertices[4]); fx_texcoord(1, 1); fx_normal_v3(normals[4]);
		fx_vertex_v3(vertices[5]); fx_texcoord(0, 1); fx_normal_v3(normals[5]);
		fx_vertex_v3(vertices[4]); fx_texcoord(1, 1); fx_normal_v3(normals[4]);
		fx_vertex_v3(vertices[7]); fx_texcoord(0, 0); fx_normal_v3(normals[7]);
	fx_end();
#endif

	bm_save(viewport, "out.gif");
	bm_save(pick_buffer, "pick.gif");

	fx_cleanup();
	bm_free(viewport);
	bm_free(tex);
	bm_free(pick_buffer);

	return 0;
}