/*
 * 1. http://en.wikipedia.org/wiki/MD2_(file_format)
 * 2. http://tfc.duke.free.fr/coding/md2-specs-en.html
 * 3. http://tfc.duke.free.fr/old/models/md2.htm
 * 4. http://openglsamples.sourceforge.net/md2.html
 *
 * Here's a discussion of how the normals are calculated:
 * https://stackoverflow.com/q/1759397/115589
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "md2.h"

#ifdef MD2_TEST
#  define MD2_NODRAW
#endif

#ifndef MD2_VERBOSE
#define MD2_VERBOSE 0
#endif

#ifndef MD2_NODRAW
#include "fx.h"
#endif

#define POST_PROCESS /* see md2_postprocess() for details */

#ifdef POST_PROCESS
static void md2_postprocess(MD2_MESH *m);
#endif

/* defined in glmatrix.h */
vec3_t vec3_lerp(vec3_t vec, vec3_t vec2, numeric_t lerp, vec3_t dest);

#pragma pack(push, 1)
/*
 * Internal texture coordinates, before conversion is applied.
 * Texture coordinates are read into this as shorts, then
 * converted to floats in the final md2_file structure.
 */
struct intern_tex_coord {
	short s, t;
};
#pragma pack(pop)

static char MD2_Err_Buf[256];
const char *md2_last_error() {
    return MD2_Err_Buf;
}

MD2_MESH *md2_load(const char *filename) {
	FILE *f;
	MD2_MESH *m;
	int i;
	char id_str[5];

	struct intern_tex_coord *tex_coords; /* temp place to read in texture coords */
	if(!(f = fopen(filename, "rb")))
		return NULL;

	m = calloc(1, sizeof *m);
	if(fread(&m->header, sizeof m->header, 1, f) != 1) {
		snprintf(MD2_Err_Buf, sizeof MD2_Err_Buf, "Unable to read header of %s", filename);
		free(m);
		fclose(f);
		return NULL;
	}

	assert(sizeof m->header == 68);
	sprintf(id_str, "%c%c%c%c", ((char*)&m->header.ident)[0], ((char*)&m->header.ident)[1], ((char*)&m->header.ident)[2], ((char*)&m->header.ident)[3]);

#if MD2_VERBOSE
	printf("IDENT.........: %s\n", id_str);
	printf("VERSION.......: %d\n", m->header.version);
	printf("SKIN DIMS.....: %dx%d\n", m->header.skin_w, m->header.skin_h);
	printf("FRAMESIZE.....: %d\n", m->header.framesize);
	printf("# SKINS.......: %d\n", m->header.n_skins);
	printf("# XYZ.........: %d\n", m->header.n_xyz);
	printf("# ST..........: %d\n", m->header.n_st);
	printf("# TRI.........: %d\n", m->header.n_tri);
	printf("# GLCMDS......: %d\n", m->header.n_glcmds);
	printf("# FRAMES......: %d\n", m->header.n_frames);
	printf("OFS SKINS.....: %d\n", m->header.ofs_skins);
	printf("OFS ST........: %d\n", m->header.ofs_st);
	printf("OFS TRIS......: %d\n", m->header.ofs_tris);
	printf("OFS FRAMES....: %d\n", m->header.ofs_frames);
	printf("OFS GLCMDS....: %d\n", m->header.ofs_glcmds);
	printf("OFS END.......: %d\n", m->header.ofs_end);
#endif

	if(strcmp(id_str, "IDP2")) {
		snprintf(MD2_Err_Buf, sizeof MD2_Err_Buf, "%s is not MD2 (ID %s mismatch)", filename, id_str);
		free(m);
		fclose(f);
		return NULL;
	}

	m->tex_coords = calloc(m->header.n_st, sizeof *m->tex_coords);
	m->triangles = calloc(m->header.n_tri, sizeof *m->triangles);
	m->glcmds = calloc(m->header.n_glcmds, sizeof *m->glcmds);
	m->frames = calloc(m->header.n_frames, sizeof *m->frames);
	m->skin_names = (m->header.n_skins > 0)?calloc(m->header.n_skins, sizeof *m->skin_names):NULL;

	if(m->header.n_skins > 0) {
		/* Apparently, this is possible */
		fseek(f, m->header.ofs_skins, SEEK_SET);
		if(fread(m->skin_names, sizeof *m->skin_names, m->header.n_skins, f) != m->header.n_skins)
		{
			snprintf(MD2_Err_Buf, sizeof MD2_Err_Buf, "unable to read skin names from %s", filename);
			md2_free(m);
			fclose(f);
			return NULL;
		}
	} else
		m->skin_names = NULL;

	/* Read the texture coordinates */
	tex_coords = calloc(m->header.n_st, sizeof *tex_coords);
	fseek(f, m->header.ofs_st, SEEK_SET);
	if(fread(tex_coords, sizeof *tex_coords, m->header.n_st, f) != m->header.n_st) {
		snprintf(MD2_Err_Buf, sizeof MD2_Err_Buf, "unable to read texture coordinates from %s", filename);
		md2_free(m);
		fclose(f);
		return NULL;
	}
	/* Now apply the conversion */
	for(i = 0; i < m->header.n_st; i++) {
		m->tex_coords[i].s = (float)tex_coords[i].s/m->header.skin_w;
		m->tex_coords[i].t = (float)tex_coords[i].t/m->header.skin_h;
	}
	free(tex_coords);

	/* Triangles */
	fseek(f, m->header.ofs_tris, SEEK_SET);
	if(fread(m->triangles, sizeof *m->triangles, m->header.n_tri, f) != m->header.n_tri) {
		snprintf(MD2_Err_Buf, sizeof MD2_Err_Buf, "unable to read triangles from %s", filename);
		md2_free(m);
		fclose(f);
		return NULL;
	}

	/* Frames */
	fseek(f, m->header.ofs_frames, SEEK_SET);
	/* You technically only need to read m->header.n_frames * m->header.framesize
	 * bytes, but it would complicate things later */
	for(i = 0; i < m->header.n_frames; i++) {
		if(fread(&m->frames[i].head, sizeof m->frames[i].head, 1, f) != 1) {
			snprintf(MD2_Err_Buf, sizeof MD2_Err_Buf, "unable to read frame %d head from %s", i, filename);
			md2_free(m);
			fclose(f);
			return NULL;
		}
		m->frames[i].tris = malloc(m->header.n_xyz * sizeof *m->frames[i].tris);
		if(fread(m->frames[i].tris, sizeof *m->frames[i].tris, m->header.n_xyz, f) != m->header.n_xyz) {
			snprintf(MD2_Err_Buf, sizeof MD2_Err_Buf, "unable to read frame %d tris from %s", i, filename);
			md2_free(m);
			fclose(f);
			return NULL;
		}
#if MD2_VERBOSE
		printf("FRAME %d.......: %s\n", i, m->frames[i].head.name);
#endif
	}

	/* GL Commands */
	fseek(f, m->header.ofs_glcmds, SEEK_SET);
	if(fread(m->glcmds, sizeof *m->glcmds, m->header.n_glcmds, f) != m->header.n_glcmds) {
		snprintf(MD2_Err_Buf, sizeof MD2_Err_Buf, "unable to read GL commands from %s", filename);
		md2_free(m);
		return NULL;
	}

	/* If the last command is not 0, we have a problem */
	assert(m->glcmds[m->header.n_glcmds-1] == 0);

	fclose(f);
#ifdef POST_PROCESS
	md2_postprocess(m);
#endif

	return m;
}

#ifdef POST_PROCESS
/*
 * This function takes the first frame of the model, and then scales the
 * model in such a way that the model has a height of 1 and a y-offset of
 * 0 at frame[0], and every other frame is scaled relative to frame[0]
 */
static void md2_postprocess(MD2_MESH *m)
{
	md2_frame *fr = &m->frames[0];
	int *cmdp, cmd, i, nv, index = 0;

	m->maxx = fr->tris[index].v[0] * fr->head.scale[2] + fr->head.translate[0];
	m->minx = fr->tris[index].v[0] * fr->head.scale[2] + fr->head.translate[0];
	m->maxy = -(fr->tris[index].v[2] * fr->head.scale[2] + fr->head.translate[2]);
	m->miny = -(fr->tris[index].v[2] * fr->head.scale[2] + fr->head.translate[2]);
	m->maxz = fr->tris[index].v[1] * fr->head.scale[1] + fr->head.translate[1];
	m->minz = fr->tris[index].v[1] * fr->head.scale[1] + fr->head.translate[1];

	cmdp = m->glcmds;
	while((cmd = *cmdp++) != 0)
	{
		if(cmd < 0)
			nv = -cmd;
		else
			nv = cmd;
		for(i = 0; i < nv; i++)
		{
			float x, y, z;
			cmdp += 2; /* dont care about texture coords here */
			index = *cmdp++;
			x = fr->tris[index].v[0] * fr->head.scale[0] + fr->head.translate[0];
			z = -(fr->tris[index].v[1] * fr->head.scale[1] + fr->head.translate[1]);
			y = fr->tris[index].v[2] * fr->head.scale[2] + fr->head.translate[2];

			if(x > m->maxx) m->maxx = x;
			if(x < m->minx) m->minx = x;
			if(y > m->maxy) m->maxy = y;
			if(y < m->miny) m->miny = y;
			if(z > m->maxz) m->maxz = z;
			if(z < m->minz) m->minz = z;
		}
	}

	for(i = 0; i < m->header.n_frames; i++)
	{
		fr = &m->frames[i];
		fr->head.scale[0] /= (m->maxy - m->miny);
		fr->head.translate[0] /= (m->maxy - m->miny);
		fr->head.scale[1] /= (m->maxy - m->miny);
		fr->head.translate[1] /= (m->maxy - m->miny);
		fr->head.scale[2] /= (m->maxy - m->miny);
		/*fr->head.translate[2] /= (m->maxy - m->miny);*/
		fr->head.translate[2] = (fr->head.translate[2] - m->miny)/(m->maxy - m->miny);
	}
}
#endif

void md2_free(MD2_MESH *m) {
	int i;
	if(!m)
		return;
	for(i = 0; i < m->header.n_frames; i++)
		free(m->frames[i].tris);
	free(m->frames);
	free(m->triangles);
	free(m->tex_coords);
	free(m->skin_names);
	free(m->glcmds);
	free(m);
}

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

double *md2_get_normal(int i) {
    assert(i >= 0 && i < sizeof(anorms)/sizeof(anorms[0]));
    return anorms[i];
}

#ifndef MD2_NODRAW
void md2_draw(MD2_MESH *m, double frame) {
	assert(frame >= 0 && frame < m->header.n_frames);
	int f0 = (int)frame;
	int f1 = (int)frame + 1;
	if(f1 >= m->header.n_frames) f1 = 0;
    md2_draw_interpolate(m, f0, f1, frame - f0);
}

void md2_draw_interpolate(MD2_MESH *m, int frame0, int frame1, double frac) {
#if 1
	assert(frame0 >= 0 && frame0 < m->header.n_frames);
	assert(frame1 >= 0 && frame1 < m->header.n_frames);
	md2_frame *fr0 = &m->frames[frame0];
	md2_frame *fr1 = &m->frames[frame1];

	int i, j;
	for(i = 0; i < m->header.n_tri; i++) {

        md2_tri *tri = &m->triangles[i];

		fx_begin(FX_TRIANGLES);
		for(j = 0; j < 3; j++) {
			int index = tri->vertexindex[j];
			double v0[3];
			v0[0] = fr0->tris[index].v[0] * fr0->head.scale[0] + fr0->head.translate[0];
			v0[1] = fr0->tris[index].v[1] * fr0->head.scale[1] + fr0->head.translate[1];
			v0[2] = fr0->tris[index].v[2] * fr0->head.scale[2] + fr0->head.translate[2];

			double v1[3];
			v1[0] = fr1->tris[index].v[0] * fr1->head.scale[0] + fr1->head.translate[0];
			v1[1] = fr1->tris[index].v[1] * fr1->head.scale[1] + fr1->head.translate[1];
			v1[2] = fr1->tris[index].v[2] * fr1->head.scale[2] + fr1->head.translate[2];

			vec3_lerp(v0, v1, frac, NULL);
			fx_vertex(v0[0], v0[2], v0[1]);

			double t[2];
			t[0] = m->tex_coords[tri->textureindex[j]].s;
			t[1] = m->tex_coords[tri->textureindex[j]].t;
			fx_texcoord(t[0], t[1]);

			double n[3];
            double *n0 = md2_get_normal(fr0->tris[index].normal_i);
            double *n1 = md2_get_normal(fr1->tris[index].normal_i);
			vec3_lerp(n0, n1, frac, n);
			fx_normal(n[0], n[2], n[1]);
		}
		fx_end();
	}
#else
	assert(frame0 >= 0 && frame0 < m->header.n_frames);
	assert(frame1 >= 0 && frame1 < m->header.n_frames);
	md2_frame *fr0 = &m->frames[frame0];
	md2_frame *fr1 = &m->frames[frame1];

	int *cmdp, cmd, i, nv, index;

	cmdp = m->glcmds;
	while((cmd = *cmdp++) != 0) {
		if(cmd < 0) {
			fx_begin(FX_TRIANGLE_FAN);
			nv = -cmd;
		} else {
			fx_begin(FX_TRIANGLE_STRIP);
			nv = cmd;
		}

		for(i = 0; i < nv; i++) {

			float u = *(float*)cmdp++;
			float v = *(float*)cmdp++;
			fx_texcoord(u,v);

			index = *cmdp++;

			/* Swap y and z (MD2 does not use the same coordinate system as OpenGL) */
			double v0[3];
			v0[0] = fr0->tris[index].v[0] * fr0->head.scale[0] + fr0->head.translate[0];
			v0[1] = fr0->tris[index].v[1] * fr0->head.scale[1] + fr0->head.translate[1];
			v0[2] = fr0->tris[index].v[2] * fr0->head.scale[2] + fr0->head.translate[2];
			double v1[3];
			v1[0] = fr1->tris[index].v[0] * fr1->head.scale[0] + fr1->head.translate[0];
			v1[1] = fr1->tris[index].v[1] * fr1->head.scale[1] + fr1->head.translate[1];
			v1[2] = fr1->tris[index].v[2] * fr1->head.scale[2] + fr1->head.translate[2];

			vec3_lerp(v0, v1, frac, NULL);
			fx_vertex(v0[0], v0[2], -v0[1]);

            double *n0 = md2_get_normal(fr0->tris[index].normal_i);
            double *n1 = md2_get_normal(fr1->tris[index].normal_i);
            double n[3];
			vec3_lerp(n0, n1, frac, n);
			fx_normal(n[0], n[2], -n[1]);
		}
		fx_end();
	}
#endif
}
#endif

#ifdef MD2_TEST
int main(int argc, char *argv[]) {
    MD2_MESH *md2;
    if(argc > 1) {
        md2 = md2_load(argv[1]);
        if(!md2) {
            fprintf(stderr, "error: unable to load %s: %s\n", argv[1], md2_last_error());
            return 1;
        }
        printf("OBJ loaded\n");
        md2_free(md2);
    }
    return 0;
}
#endif