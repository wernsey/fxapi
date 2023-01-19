/*
 * http://tfc.duke.free.fr/coding/mdl-specs-en.html
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>

#include "bmp.h"
#include "mdl.h"

#ifndef MDL_NODRAW
#  include "fx.h"
#endif

static struct rgb {
	uint8_t r, g, b;
} palette[] = {
/* http://tfc.duke.free.fr/coding/src/colormap.h */
{  0,   0,   0}, { 15,  15,  15}, { 31,  31,  31}, { 47,  47,  47}, 
{ 63,  63,  63}, { 75,  75,  75}, { 91,  91,  91}, {107, 107, 107}, 
{123, 123, 123}, {139, 139, 139}, {155, 155, 155}, {171, 171, 171}, 
{187, 187, 187}, {203, 203, 203}, {219, 219, 219}, {235, 235, 235}, 
{ 15,  11,   7}, { 23,  15,  11}, { 31,  23,  11}, { 39,  27,  15}, 
{ 47,  35,  19}, { 55,  43,  23}, { 63,  47,  23}, { 75,  55,  27}, 
{ 83,  59,  27}, { 91,  67,  31}, { 99,  75,  31}, {107,  83,  31}, 
{115,  87,  31}, {123,  95,  35}, {131, 103,  35}, {143, 111,  35}, 
{ 11,  11,  15}, { 19,  19,  27}, { 27,  27,  39}, { 39,  39,  51}, 
{ 47,  47,  63}, { 55,  55,  75}, { 63,  63,  87}, { 71,  71, 103}, 
{ 79,  79, 115}, { 91,  91, 127}, { 99,  99, 139}, {107, 107, 151}, 
{115, 115, 163}, {123, 123, 175}, {131, 131, 187}, {139, 139, 203}, 
{  0,   0,   0}, {  7,   7,   0}, { 11,  11,   0}, { 19,  19,   0}, 
{ 27,  27,   0}, { 35,  35,   0}, { 43,  43,   7}, { 47,  47,   7}, 
{ 55,  55,   7}, { 63,  63,   7}, { 71,  71,   7}, { 75,  75,  11}, 
{ 83,  83,  11}, { 91,  91,  11}, { 99,  99,  11}, {107, 107,  15}, 
{  7,   0,   0}, { 15,   0,   0}, { 23,   0,   0}, { 31,   0,   0}, 
{ 39,   0,   0}, { 47,   0,   0}, { 55,   0,   0}, { 63,   0,   0}, 
{ 71,   0,   0}, { 79,   0,   0}, { 87,   0,   0}, { 95,   0,   0}, 
{103,   0,   0}, {111,   0,   0}, {119,   0,   0}, {127,   0,   0}, 
{ 19,  19,   0}, { 27,  27,   0}, { 35,  35,   0}, { 47,  43,   0}, 
{ 55,  47,   0}, { 67,  55,   0}, { 75,  59,   7}, { 87,  67,   7}, 
{ 95,  71,   7}, {107,  75,  11}, {119,  83,  15}, {131,  87,  19}, 
{139,  91,  19}, {151,  95,  27}, {163,  99,  31}, {175, 103,  35}, 
{ 35,  19,   7}, { 47,  23,  11}, { 59,  31,  15}, { 75,  35,  19}, 
{ 87,  43,  23}, { 99,  47,  31}, {115,  55,  35}, {127,  59,  43}, 
{143,  67,  51}, {159,  79,  51}, {175,  99,  47}, {191, 119,  47}, 
{207, 143,  43}, {223, 171,  39}, {239, 203,  31}, {255, 243,  27}, 
{ 11,   7,   0}, { 27,  19,   0}, { 43,  35,  15}, { 55,  43,  19}, 
{ 71,  51,  27}, { 83,  55,  35}, { 99,  63,  43}, {111,  71,  51}, 
{127,  83,  63}, {139,  95,  71}, {155, 107,  83}, {167, 123,  95}, 
{183, 135, 107}, {195, 147, 123}, {211, 163, 139}, {227, 179, 151}, 
{171, 139, 163}, {159, 127, 151}, {147, 115, 135}, {139, 103, 123}, 
{127,  91, 111}, {119,  83,  99}, {107,  75,  87}, { 95,  63,  75}, 
{ 87,  55,  67}, { 75,  47,  55}, { 67,  39,  47}, { 55,  31,  35}, 
{ 43,  23,  27}, { 35,  19,  19}, { 23,  11,  11}, { 15,   7,   7}, 
{187, 115, 159}, {175, 107, 143}, {163,  95, 131}, {151,  87, 119}, 
{139,  79, 107}, {127,  75,  95}, {115,  67,  83}, {107,  59,  75}, 
{ 95,  51,  63}, { 83,  43,  55}, { 71,  35,  43}, { 59,  31,  35}, 
{ 47,  23,  27}, { 35,  19,  19}, { 23,  11,  11}, { 15,   7,   7}, 
{219, 195, 187}, {203, 179, 167}, {191, 163, 155}, {175, 151, 139}, 
{163, 135, 123}, {151, 123, 111}, {135, 111,  95}, {123,  99,  83}, 
{107,  87,  71}, { 95,  75,  59}, { 83,  63,  51}, { 67,  51,  39}, 
{ 55,  43,  31}, { 39,  31,  23}, { 27,  19,  15}, { 15,  11,   7}, 
{111, 131, 123}, {103, 123, 111}, { 95, 115, 103}, { 87, 107,  95}, 
{ 79,  99,  87}, { 71,  91,  79}, { 63,  83,  71}, { 55,  75,  63}, 
{ 47,  67,  55}, { 43,  59,  47}, { 35,  51,  39}, { 31,  43,  31}, 
{ 23,  35,  23}, { 15,  27,  19}, { 11,  19,  11}, {  7,  11,   7}, 
{255, 243,  27}, {239, 223,  23}, {219, 203,  19}, {203, 183,  15}, 
{187, 167,  15}, {171, 151,  11}, {155, 131,   7}, {139, 115,   7}, 
{123,  99,   7}, {107,  83,   0}, { 91,  71,   0}, { 75,  55,   0}, 
{ 59,  43,   0}, { 43,  31,   0}, { 27,  15,   0}, { 11,   7,   0}, 
{  0,   0, 255}, { 11,  11, 239}, { 19,  19, 223}, { 27,  27, 207}, 
{ 35,  35, 191}, { 43,  43, 175}, { 47,  47, 159}, { 47,  47, 143}, 
{ 47,  47, 127}, { 47,  47, 111}, { 47,  47,  95}, { 43,  43,  79}, 
{ 35,  35,  63}, { 27,  27,  47}, { 19,  19,  31}, { 11,  11,  15}, 
{ 43,   0,   0}, { 59,   0,   0}, { 75,   7,   0}, { 95,   7,   0}, 
{111,  15,   0}, {127,  23,   7}, {147,  31,   7}, {163,  39,  11}, 
{183,  51,  15}, {195,  75,  27}, {207,  99,  43}, {219, 127,  59}, 
{227, 151,  79}, {231, 171,  95}, {239, 191, 119}, {247, 211, 139}, 
{167, 123,  59}, {183, 155,  55}, {199, 195,  55}, {231, 227,  87}, 
{127, 191, 255}, {171, 231, 255}, {215, 255, 255}, {103,   0,   0}, 
{139,   0,   0}, {179,   0,   0}, {215,   0,   0}, {255,   0,   0}, 
{255, 243, 147}, {255, 247, 199}, {255, 255, 255}, {159,  91,  83}, 
};

static Bitmap *read_texture(FILE *f, int w, int h) {
	int x, y, i = 0;
	Bitmap *bmp = bm_create(w, h);
	uint8_t *bytes;
	if(!bmp)
		return NULL;
	bytes = malloc(w * h);
	if(fread(bytes, w*h, 1, f) != 1) {
		fprintf(stderr, "couldn't read texture\n");
		goto error;
	}
	for(y = 0; y < h; y++) {
		for(x = 0; x < w; x++) {
			struct rgb *rgb = palette + bytes[i++];
			unsigned int color = bm_rgb(rgb->r, rgb->g, rgb->b);
			bm_set(bmp, x, y, color);
		}
	}
	free(bytes);
	return bmp;
error:
	free(bytes);
	bm_free(bmp);
	return NULL;
}

static void mdl_postprocess(MDL_MESH *m);

MDL_MESH *mdl_load(const char *filename) {
	int i;
	MDL_MESH *M = malloc(sizeof *M);
	if(!M) {
		fprintf(stderr, "error: out of memory\n");
		return NULL;
	}
	
	FILE *f = fopen(filename, "rb");
	if(!f) {
		fprintf(stderr, "error: Couldn't open %s: %s\n", filename, strerror(errno));
		return NULL;
	}
	
	if(fread(&M->header, sizeof M->header, 1, f) != 1) {
		fprintf(stderr, "couldn't read header: %s\n", strerror(errno));
		return NULL;
	}
	
	if(memcmp(&M->header.ident, "IDPO", 4)) {
		fprintf(stderr, "error: bad ident\n");
		return NULL;
	}
	
	int32_t group;
	if(fread(&group, sizeof group, 1, f) != 1) {
		fprintf(stderr, "error: couldn't read texture group: %s\n", strerror(errno));		
		return NULL;
	}
	
	if(group) {
		if(fread(&M->num_textures, sizeof M->num_textures, 1, f) != 1) {
			fprintf(stderr, "error: couldn't read texture count: %s\n", strerror(errno));		
			return NULL;
		}
	} else {
		M->num_textures = 1;
	}
		
	M->times = calloc(M->num_textures, sizeof *M->times);
	if(!M->times) {
		fprintf(stderr, "error: out of memory\n");
		return NULL;
	}
	M->textures = calloc(M->num_textures, sizeof *M->textures);
	if(!M->textures) {
		fprintf(stderr, "error: out of memory\n");
		return NULL;
	}
	
	if(group) {
		if(fread(M->times, sizeof *M->times, M->num_textures, f) != (unsigned)M->num_textures) {
			fprintf(stderr, "error: unable to read times: %s\n", strerror(errno));
			return NULL;
		}
		for(i = 0; i < M->num_textures; i++) {
			M->textures[i] = read_texture(f, M->header.skinwidth, M->header.skinheight);
			if(!M->textures[i])
				return NULL;
		}
	} else {
		M->times[0] = 0;
		M->textures[0] = read_texture(f, M->header.skinwidth, M->header.skinheight);
		if(!M->textures[0])
			return NULL;
	}
	
	M->tex_coords = malloc(M->header.num_verts * sizeof *M->tex_coords);
	if(!M->tex_coords) {
		fprintf(stderr, "error: out of memory\n");
		return NULL;
	}
	
	if(fread(M->tex_coords, sizeof *M->tex_coords, M->header.num_verts, f) != M->header.num_verts) {
		fprintf(stderr, "error: unable to read texture coordinates: %s\n", strerror(errno));
		return NULL;
	}

	M->triangles = malloc(M->header.num_tris * sizeof *M->triangles);
	if(!M->triangles) {
		fprintf(stderr, "error: out of memory\n");
		return NULL;
	}
	if(fread(M->triangles, sizeof *M->triangles, M->header.num_tris, f) != M->header.num_tris) {
		fprintf(stderr, "error: unable to read triangles: %s\n", strerror(errno));
		return NULL;
	}
	
	M->frames = malloc(M->header.num_frames * sizeof *M->frames);
	if(!M->frames) {
		fprintf(stderr, "error: out of memory\n");
		return NULL;
	}
	for(i = 0; i < M->header.num_frames; i++) {
		mdl_simpleframe *frame = &M->frames[i];
		frame->verts = malloc(M->header.num_verts * sizeof *frame->verts);
		if(!frame->verts) {
			fprintf(stderr, "error: out of memory\n");
			return NULL;
		}
		
		int32_t type;
		if(fread(&type, sizeof type, 1, f) != 1) {
			fprintf(stderr, "error: couldn't read frame type: %s\n", strerror(errno));
			return NULL;
		}
		if(type != 0) {
			/* The reference talks about groups of frames, but doesn't handle them */
			fprintf(stderr, "error: can't handle group of frames");
			return NULL;
		}
		if(fread(&frame->bboxmin, sizeof frame->bboxmin, 1, f) != 1 ||
			fread(&frame->bboxmin, sizeof frame->bboxmax, 1, f) != 1 ||
			fread(frame->name, 16, 1, f) != 1) {
			fprintf(stderr, "error: couldn't read frame data: %s", strerror(errno));
			return NULL;
		}
		if(fread(frame->verts, sizeof *frame->verts, M->header.num_verts, f) != M->header.num_verts) {
			fprintf(stderr, "error: couldn't read frame vertices: %s", strerror(errno));
			return NULL;
		}
	}	
	
	fclose(f);
	
	mdl_postprocess(M);
	
	return M;
}



static void print_vec(const char *name, mdl_vec3 v) {
	if(name) 
		printf("%s: <%g, %g, %g>\n", name, v[0], v[1], v[2]);
	else
		printf("<%g, %g, %g>\n", v[0], v[1], v[2]);
}

static void print_vert(struct mdl_vertex *vert) {
	printf(" * <%u %u %u>  [%u]\n", vert->v[0], vert->v[0], vert->v[0], vert->normalIndex);
}

void mdl_print(MDL_MESH *M) {
	int i;
	
	printf("ident: %08X; version: %d\n", M->header.ident, M->header.version);
	
	print_vec("scale", M->header.scale);
	print_vec("translate", M->header.translate);
	printf("boundingradius: %g\n", M->header.boundingradius);
	print_vec("eye_pos", M->header.eye_pos);
	
	printf("num_skins: %d; dimensions: %dx%d\n", M->header.num_skins, M->header.skinwidth, M->header.skinheight);
	
	printf("verts: %d; tris: %d; frames: %d\n", M->header.num_verts, M->header.num_tris, M->header.num_frames);
	
	printf("sync: %d; flags: %08X; size: %g\n", M->header.synctype, M->header.flags, M->header.size);
	
	printf("Number of textures: %d\n", M->num_textures);

	for(i = 0; i < M->num_textures; i++) {
		bm_savef(M->textures[i], "texture%d.gif", i);
		printf("Texture %d. Time: %g; saved\n", i, M->times[i]);		
	}
	
	printf("Texture coordinates:\n");
	for(i = 0; i < M->header.num_verts; i++) {
		printf(" * %3d (%d, %d) %c\n", i, M->tex_coords[i].s, M->tex_coords[i].t, M->tex_coords[i].onseam ? '*' : ' ');
	}
	
	printf("Triangles:\n");
	for(i = 0; i < M->header.num_tris; i++) {
		printf(" * %3d (%d, %d, %d) %c\n", i, M->triangles[i].vertex[0], M->triangles[i].vertex[1], M->triangles[i].vertex[2], M->triangles[i].facesfront ? '*' : ' ');
	}
	
	for(i = 0; i < M->header.num_frames; i++) {
		mdl_simpleframe *frame = &M->frames[i];
		printf("FRAME %d: %s\n", i, frame->name);
		print_vert(&frame->bboxmin);
		print_vert(&frame->bboxmax);
		/*
		int j;
		for(j = 0; j < M->header.num_verts; j++) {
			print_vert(&frame->verts[j]);
		}
		*/
	}	
	
}

static void mdl_postprocess(MDL_MESH *m) {
	int i, j, n;
	
	double mins[3] = {
		m->header.scale[0] * 255 + m->header.translate[0], 
		m->header.scale[1] * 255 + m->header.translate[1], 
		m->header.scale[2] * 255 + m->header.translate[2]};
	double maxs[3] = {
		m->header.translate[0], 
		m->header.translate[1], 
		m->header.translate[2]};
	
	for(n = 0; n < m->header.num_frames; n++) {
		mdl_simpleframe *frame = &m->frames[n];
		for(i = 0; i < m->header.num_tris; i++) {
			for(j = 0; j < 3; j++) {
				int index = m->triangles[i].vertex[j];
				mdl_vertex *vert = &frame->verts[index];
				
				/*
				float s = (float)m->tex_coords[index].s;
				float t = (float)m->tex_coords[index].t;
				
				if(!m->triangles[i].facesfront && m->tex_coords[index].onseam) {
					s += m->header.skinwidth * 0.5;
				}				
				s = (s + 0.5)/ m->header.skinwidth;
				t = (t + 0.5)/ m->header.skinheight;
				*/
				
				double v[3];
				v[0] = m->header.scale[0] * vert->v[0] + m->header.translate[0];
				v[1] = m->header.scale[1] * vert->v[1] + m->header.translate[1];
				v[2] = m->header.scale[2] * vert->v[2] + m->header.translate[2];
				
				if(v[0] < mins[0]) mins[0] = v[0];
				if(v[0] > maxs[0]) maxs[0] = v[0];
				if(v[1] < mins[1]) mins[1] = v[1];
				if(v[1] > maxs[1]) maxs[1] = v[1];
				if(v[2] < mins[2]) mins[2] = v[2];
				if(v[2] > maxs[2]) maxs[2] = v[2];
				
			}
		}
	}
	
#if 0
	double d[3] = {maxs[0] - mins[0], maxs[1] - mins[1], maxs[2] - mins[2]}, max, min;
	
	if(d[0] > d[1]) {
		if(d[0] > d[2]) {
			max = maxs[0];
			min = mins[0];			
		} else {			
			max = maxs[2];
			min = mins[2];
		}
	} else { 
		if (d[1] > d[2]) {
			max = maxs[1];
			min = mins[1];
		} else {	
			max = maxs[2];
			min = mins[2];			
		}
	}
	
	printf("MINs: <%g %g %g>\n", mins[0], mins[1], mins[2]);
	printf("MAXs: <%g %g %g>\n", maxs[0], maxs[1], maxs[2]);
	
	printf("D: %g -> %g: %g\n", min, max, max-min);
#else
	double max = maxs[2], min = mins[2];
#endif

	
	m->header.scale[0] /= (max - min);
	m->header.translate[0] /= (max - min);
	m->header.scale[1] /= (max - min);
	m->header.translate[1] /= (max - min);
	m->header.scale[2] /= (max - min);
	m->header.translate[2] =  (m->header.translate[2] - mins[2]) / (max - min);
	
}

void mdl_free(MDL_MESH *m) {
	int i;
	for(i = 0; i < m->num_textures; i++)
		bm_free(m->textures[i]);
	free(m->times);		
	for(i = 0; i < m->header.num_frames; i++)
		free(m->frames[i].verts);
	free(m->frames);	
	free(m->triangles);
	free(m->tex_coords);	
	free(m);
}

#ifndef MDL_NODRAW

/* I got the normals from here:
 * http://tfc.duke.free.fr/coding/src/anorms.h
 */
static double anorms[][3] = {
{ -0.525731f,  0.000000f,  0.850651f },
{ -0.442863f,  0.238856f,  0.864188f },
{ -0.295242f,  0.000000f,  0.955423f },
{ -0.309017f,  0.500000f,  0.809017f },
{ -0.162460f,  0.262866f,  0.951056f },
{  0.000000f,  0.000000f,  1.000000f },
{  0.000000f,  0.850651f,  0.525731f },
{ -0.147621f,  0.716567f,  0.681718f },
{  0.147621f,  0.716567f,  0.681718f },
{  0.000000f,  0.525731f,  0.850651f },
{  0.309017f,  0.500000f,  0.809017f },
{  0.525731f,  0.000000f,  0.850651f },
{  0.295242f,  0.000000f,  0.955423f },
{  0.442863f,  0.238856f,  0.864188f },
{  0.162460f,  0.262866f,  0.951056f },
{ -0.681718f,  0.147621f,  0.716567f },
{ -0.809017f,  0.309017f,  0.500000f },
{ -0.587785f,  0.425325f,  0.688191f },
{ -0.850651f,  0.525731f,  0.000000f },
{ -0.864188f,  0.442863f,  0.238856f },
{ -0.716567f,  0.681718f,  0.147621f },
{ -0.688191f,  0.587785f,  0.425325f },
{ -0.500000f,  0.809017f,  0.309017f },
{ -0.238856f,  0.864188f,  0.442863f },
{ -0.425325f,  0.688191f,  0.587785f },
{ -0.716567f,  0.681718f, -0.147621f },
{ -0.500000f,  0.809017f, -0.309017f },
{ -0.525731f,  0.850651f,  0.000000f },
{  0.000000f,  0.850651f, -0.525731f },
{ -0.238856f,  0.864188f, -0.442863f },
{  0.000000f,  0.955423f, -0.295242f },
{ -0.262866f,  0.951056f, -0.162460f },
{  0.000000f,  1.000000f,  0.000000f },
{  0.000000f,  0.955423f,  0.295242f },
{ -0.262866f,  0.951056f,  0.162460f },
{  0.238856f,  0.864188f,  0.442863f },
{  0.262866f,  0.951056f,  0.162460f },
{  0.500000f,  0.809017f,  0.309017f },
{  0.238856f,  0.864188f, -0.442863f },
{  0.262866f,  0.951056f, -0.162460f },
{  0.500000f,  0.809017f, -0.309017f },
{  0.850651f,  0.525731f,  0.000000f },
{  0.716567f,  0.681718f,  0.147621f },
{  0.716567f,  0.681718f, -0.147621f },
{  0.525731f,  0.850651f,  0.000000f },
{  0.425325f,  0.688191f,  0.587785f },
{  0.864188f,  0.442863f,  0.238856f },
{  0.688191f,  0.587785f,  0.425325f },
{  0.809017f,  0.309017f,  0.500000f },
{  0.681718f,  0.147621f,  0.716567f },
{  0.587785f,  0.425325f,  0.688191f },
{  0.955423f,  0.295242f,  0.000000f },
{  1.000000f,  0.000000f,  0.000000f },
{  0.951056f,  0.162460f,  0.262866f },
{  0.850651f, -0.525731f,  0.000000f },
{  0.955423f, -0.295242f,  0.000000f },
{  0.864188f, -0.442863f,  0.238856f },
{  0.951056f, -0.162460f,  0.262866f },
{  0.809017f, -0.309017f,  0.500000f },
{  0.681718f, -0.147621f,  0.716567f },
{  0.850651f,  0.000000f,  0.525731f },
{  0.864188f,  0.442863f, -0.238856f },
{  0.809017f,  0.309017f, -0.500000f },
{  0.951056f,  0.162460f, -0.262866f },
{  0.525731f,  0.000000f, -0.850651f },
{  0.681718f,  0.147621f, -0.716567f },
{  0.681718f, -0.147621f, -0.716567f },
{  0.850651f,  0.000000f, -0.525731f },
{  0.809017f, -0.309017f, -0.500000f },
{  0.864188f, -0.442863f, -0.238856f },
{  0.951056f, -0.162460f, -0.262866f },
{  0.147621f,  0.716567f, -0.681718f },
{  0.309017f,  0.500000f, -0.809017f },
{  0.425325f,  0.688191f, -0.587785f },
{  0.442863f,  0.238856f, -0.864188f },
{  0.587785f,  0.425325f, -0.688191f },
{  0.688191f,  0.587785f, -0.425325f },
{ -0.147621f,  0.716567f, -0.681718f },
{ -0.309017f,  0.500000f, -0.809017f },
{  0.000000f,  0.525731f, -0.850651f },
{ -0.525731f,  0.000000f, -0.850651f },
{ -0.442863f,  0.238856f, -0.864188f },
{ -0.295242f,  0.000000f, -0.955423f },
{ -0.162460f,  0.262866f, -0.951056f },
{  0.000000f,  0.000000f, -1.000000f },
{  0.295242f,  0.000000f, -0.955423f },
{  0.162460f,  0.262866f, -0.951056f },
{ -0.442863f, -0.238856f, -0.864188f },
{ -0.309017f, -0.500000f, -0.809017f },
{ -0.162460f, -0.262866f, -0.951056f },
{  0.000000f, -0.850651f, -0.525731f },
{ -0.147621f, -0.716567f, -0.681718f },
{  0.147621f, -0.716567f, -0.681718f },
{  0.000000f, -0.525731f, -0.850651f },
{  0.309017f, -0.500000f, -0.809017f },
{  0.442863f, -0.238856f, -0.864188f },
{  0.162460f, -0.262866f, -0.951056f },
{  0.238856f, -0.864188f, -0.442863f },
{  0.500000f, -0.809017f, -0.309017f },
{  0.425325f, -0.688191f, -0.587785f },
{  0.716567f, -0.681718f, -0.147621f },
{  0.688191f, -0.587785f, -0.425325f },
{  0.587785f, -0.425325f, -0.688191f },
{  0.000000f, -0.955423f, -0.295242f },
{  0.000000f, -1.000000f,  0.000000f },
{  0.262866f, -0.951056f, -0.162460f },
{  0.000000f, -0.850651f,  0.525731f },
{  0.000000f, -0.955423f,  0.295242f },
{  0.238856f, -0.864188f,  0.442863f },
{  0.262866f, -0.951056f,  0.162460f },
{  0.500000f, -0.809017f,  0.309017f },
{  0.716567f, -0.681718f,  0.147621f },
{  0.525731f, -0.850651f,  0.000000f },
{ -0.238856f, -0.864188f, -0.442863f },
{ -0.500000f, -0.809017f, -0.309017f },
{ -0.262866f, -0.951056f, -0.162460f },
{ -0.850651f, -0.525731f,  0.000000f },
{ -0.716567f, -0.681718f, -0.147621f },
{ -0.716567f, -0.681718f,  0.147621f },
{ -0.525731f, -0.850651f,  0.000000f },
{ -0.500000f, -0.809017f,  0.309017f },
{ -0.238856f, -0.864188f,  0.442863f },
{ -0.262866f, -0.951056f,  0.162460f },
{ -0.864188f, -0.442863f,  0.238856f },
{ -0.809017f, -0.309017f,  0.500000f },
{ -0.688191f, -0.587785f,  0.425325f },
{ -0.681718f, -0.147621f,  0.716567f },
{ -0.442863f, -0.238856f,  0.864188f },
{ -0.587785f, -0.425325f,  0.688191f },
{ -0.309017f, -0.500000f,  0.809017f },
{ -0.147621f, -0.716567f,  0.681718f },
{ -0.425325f, -0.688191f,  0.587785f },
{ -0.162460f, -0.262866f,  0.951056f },
{  0.442863f, -0.238856f,  0.864188f },
{  0.162460f, -0.262866f,  0.951056f },
{  0.309017f, -0.500000f,  0.809017f },
{  0.147621f, -0.716567f,  0.681718f },
{  0.000000f, -0.525731f,  0.850651f },
{  0.425325f, -0.688191f,  0.587785f },
{  0.587785f, -0.425325f,  0.688191f },
{  0.688191f, -0.587785f,  0.425325f },
{ -0.955423f,  0.295242f,  0.000000f },
{ -0.951056f,  0.162460f,  0.262866f },
{ -1.000000f,  0.000000f,  0.000000f },
{ -0.850651f,  0.000000f,  0.525731f },
{ -0.955423f, -0.295242f,  0.000000f },
{ -0.951056f, -0.162460f,  0.262866f },
{ -0.864188f,  0.442863f, -0.238856f },
{ -0.951056f,  0.162460f, -0.262866f },
{ -0.809017f,  0.309017f, -0.500000f },
{ -0.864188f, -0.442863f, -0.238856f },
{ -0.951056f, -0.162460f, -0.262866f },
{ -0.809017f, -0.309017f, -0.500000f },
{ -0.681718f,  0.147621f, -0.716567f },
{ -0.681718f, -0.147621f, -0.716567f },
{ -0.850651f,  0.000000f, -0.525731f },
{ -0.688191f,  0.587785f, -0.425325f },
{ -0.587785f,  0.425325f, -0.688191f },
{ -0.425325f,  0.688191f, -0.587785f },
{ -0.425325f, -0.688191f, -0.587785f },
{ -0.587785f, -0.425325f, -0.688191f },
{ -0.688191f, -0.587785f, -0.425325f }
};


void mdl_draw_interpolate(MDL_MESH *m, int frame0, int frame1, double frac) {
	int i, j;
	mdl_simpleframe *fr0 = &m->frames[frame0];
	mdl_simpleframe *fr1 = &m->frames[frame1];
	
	fx_set_texture(m->textures[0]);
	fx_begin(FX_TRIANGLES);
	for(i = 0; i < m->header.num_tris; i++) {
		for(j = 0; j < 3; j++) {
			int index = m->triangles[i].vertex[j];
			mdl_vertex *vert0 = &fr0->verts[index];
			mdl_vertex *vert1 = &fr1->verts[index];
			float s = (float)m->tex_coords[index].s;
			float t = (float)m->tex_coords[index].t;
			
			if(!m->triangles[i].facesfront && m->tex_coords[index].onseam) {
				s += m->header.skinwidth * 0.5;
			}
			
			s = (s + 0.5)/ m->header.skinwidth;
			t = (t + 0.5)/ m->header.skinheight;
			fx_texcoord(s,t);
			
			double *no0 = anorms[vert0->normalIndex];
			double *no1 = anorms[vert1->normalIndex];
			fx_normal(no0[0] + frac * (no1[0] - no0[0]), no0[2] + frac * (no1[2] - no0[2]), no0[1] + frac * (no1[1] - no0[1]));			 
			 
			double v[3];
			v[0] = m->header.scale[0] * (vert0->v[0] + frac * (vert1->v[0] - vert0->v[0])) + m->header.translate[0];
			v[1] = m->header.scale[1] * (vert0->v[1] + frac * (vert1->v[1] - vert0->v[1])) + m->header.translate[1];
			v[2] = m->header.scale[2] * (vert0->v[2] + frac * (vert1->v[2] - vert0->v[2])) + m->header.translate[2];
			fx_vertex(v[0], v[2], v[1]);
		}
	}
	fx_end();
}

void mdl_draw(MDL_MESH *m, double frame) {
	assert(frame >= 0 && frame < m->header.num_frames);
	int f0 = (int)frame;
	int f1 = (int)frame + 1;
	if(f1 >= m->header.num_frames) f1 = 0;
    mdl_draw_interpolate(m, f0, f1, frame - f0);
}

#endif

#if 0
int main(int argc, char *argv[]) {	
	const char *filename = "SHAMBLER.MDL";
	if(argc > 1)
		filename = argv[1];
	
	MDL_MESH *m = mdl_load(filename);
	if(!m) {
		
		return 1;
	}
	
	mdl_print(m);
	
	mdl_free(m);
	
	printf("Done.\n");
	return 0;
}
#endif
