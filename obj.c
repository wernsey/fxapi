/* Simple reader for OBJ files.
 * It only supports triangle meshes.
 *
 * References:
 * * https://en.wikipedia.org/wiki/Wavefront_.obj_file
 * * http://www.martinreddy.net/gfx/3d/OBJ.spec
 * * http://paulbourke.net/dataformats/obj/
 *
 * Usage:
 *     # Compile:
 *     cc -o obj -DOBJ_TEST obj.c
 *     # Run:
 *     ./obj teapot.obj out.obj
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>
#include <errno.h>
#include <assert.h>

#ifdef OBJ_TEST
#  define OBJ_NODRAW
#endif

#include "obj.h"

#ifndef OBJ_NODRAW
#include "fx.h"
#endif

static char Obj_Err_Buf[256];
const char *obj_last_error() {
    return Obj_Err_Buf;
}

typedef struct OBJ_DArray {
    size_t esize;   /* sizeof the individual elements */
    unsigned int n; /* Number of elements in the listl allocated */
    unsigned int a; /* Allocated number of elements */
    char *els;      /* Actual elements */
    void (*dtor)(void* p);  /* Destructor */
} OBJ_DArray;

static OBJ_DArray *al_create(size_t esize) {
	OBJ_DArray *al = malloc(sizeof *al);
	al->esize = esize;
	al->a = 8;
	al->els = calloc(al->a, esize);
	al->n = 0;
    al->dtor = NULL;
	return al;
}

static void *al_get(OBJ_DArray *al, unsigned int i) {
	assert(i >= 0 && i < al->n);
	return al->els + (i * al->esize);
}

static void al_free(OBJ_DArray *al) {
    if(al->dtor) {
        int i;
        for(i = 0; i < al->n; i++)
            al->dtor(al_get(al, i));
    }
	free(al->els);
	free(al);
}

static void *al_add(OBJ_DArray *al) {
	if(al->n == al->a) {
		al->a <<= 1;
		al->els = realloc(al->els, al->a * al->esize);
	}
	assert(al->n < al->a);
	return al->els + (al->n++ * al->esize);
}

#define al_size(x) ((x)->n)

static void free_face(void *p) {
	OBJ_FACE *f = p;
	free(f->fv);
}

OBJ_MESH *obj_create() {
	OBJ_MESH *m = malloc(sizeof *m);
	m->verts = al_create(3 * sizeof(double));
	m->norms = al_create(3 * sizeof(double));
	m->texs = al_create(3 * sizeof(double));
	m->faces = al_create(sizeof(OBJ_FACE));
	m->faces->dtor = free_face;

	m->groups = al_create(sizeof(char *));
    m->groups->dtor = free;

	m->name = NULL;

	m->xmin = DBL_MAX; m->xmax = DBL_MIN;
	m->ymin = DBL_MAX; m->ymax = DBL_MIN;
	m->zmin = DBL_MAX; m->zmax = DBL_MIN;

	return m;
}

void obj_free(OBJ_MESH *m) {
	al_free(m->verts);
	al_free(m->norms);
	al_free(m->texs);
	al_free(m->faces);
	al_free(m->groups);
	if(m->name)
		free(m->name);
	free(m);
}

int obj_nfaces(OBJ_MESH *m) {
    return m->faces->n;
}

OBJ_FACE *obj_face(OBJ_MESH *m, int n) {
    assert(n < m->faces->n);
    return al_get(m->faces, n);
}

int obj_nverts(OBJ_MESH *m) {
    return m->verts->n;
}

double *obj_vert(OBJ_MESH *m, int n) {
    assert(n < m->verts->n);
    return al_get(m->verts, n);
}

int obj_norms(OBJ_MESH *m) {
    return m->norms->n;
}
double *obj_norm(OBJ_MESH *m, int n) {
    assert(n < m->norms->n);
    return al_get(m->norms, n);
}

int obj_ntexs(OBJ_MESH *m) {
    return m->texs->n;
}

double *obj_tex(OBJ_MESH *m, int n) {
    assert(n < m->texs->n);
    return al_get(m->texs, n);
}

static char *tokenize(char *str, const char *delim, char **save) {
	if(!str)
		str = *save;
	if(!str[0])
		return NULL;
	char *p = strpbrk(str, delim);
	if(!p) {
		/* last token */
		p = str;
		while(str[0] && str[0] != '\n') str++;
		str[0] = '\0';
		*save = str;
		return p;
	}
	p[0] = '\0';
	*save = ++p;
	return str;
}

static char *read_line(char *o, size_t ol, FILE *f, OBJ_MESH *m, char **save) {
	unsigned i = 0;
	char *str = *save;
	while(str[0] && strchr(" \t\r", str[0])) str++;
	while(str[0] != '\n') {
		if(str[0] == '\0') {
			snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "unexpected end of file");
			goto error;
		} else if(str[0] == '\r')
			continue;
		if(i == ol - 1) {
			snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "can't handle object names that long");
			goto error;
		}
		o[i++] = *str++;
	}
	o[i++] = '\0';
    *save = str;
	return o;
error:
	o[0] = '\0';
	*save = str;
	return NULL;
}

OBJ_MESH *obj_load(const char *filename) {
	OBJ_MESH *m;
	FILE *f = fopen(filename, "r");
	if(!f) {
        snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "couldn't open '%s': %s", filename, strerror(errno));
		return NULL;
	}
	m = obj_create();
	char buffer[256], *str, *save, *word, *current_group = NULL;
    int smoothing_group = 0;

	while(fgets(buffer, sizeof buffer, f)) {
		str = buffer;
		int i;
		while(isspace(*str)) str++;
		if(str[0] == '\0' || str[0] == '#')
			continue;
		word = tokenize(str, " \n", &save);
		if(!strcmp(word, "v")) {
			double *vert = al_add(m->verts);
			for(i = 0; i < 3; i++) {
				/* It turns out the tokenize fails if
				you have more than one space */
				while(isspace(save[0])) save++;
				vert[i] = atof(tokenize(NULL, " ", &save));
			}
			/* Officially, the vertices can also have a w component
				that defaults to 1.0, but I don't support that. */
			if(vert[0] < m->xmin) m->xmin = vert[0];
			if(vert[0] > m->xmax) m->xmax = vert[0];
			if(vert[1] < m->ymin) m->ymin = vert[1];
			if(vert[1] > m->ymax) m->ymax = vert[1];
			if(vert[2] < m->zmin) m->zmin = vert[2];
			if(vert[2] > m->zmax) m->zmax = vert[2];

		} else if(!strcmp(word, "vn")) {
			double *norm = al_add(m->norms);
			for(i = 0; i < 3; i++) {
				while(isspace(save[0])) save++;
				norm[i] = atof(tokenize(NULL, " ", &save));
			}
		} else if(!strcmp(word, "vt")) {
			double *tex = al_add(m->texs);
			for(i = 0; i < 2; i++) {
				while(isspace(save[0])) save++;
				tex[i] = atof(tokenize(NULL, " ", &save));
			}
			/* w component is optional */
			char *tex2 = tokenize(NULL, " ", &save);
			tex[2] = tex2 ? atof(tex2) : 0.0;
            /* Textures are flipped vertically */
            tex[1] = 1.0 - tex[1];
		} else if(!strcmp(word, "f")) {
            OBJ_FACE *face = al_add(m->faces);
            face->g = current_group;
            face->s = smoothing_group;

			face->n = 0;
			face->a = 3;
			face->fv = malloc(face->a * sizeof *face->fv);

			for(;;) {
				while(isspace(save[0])) save++;
				if(save[0] == '\0' || save[0] == '\n')
					break;

				int v = -1, vn = -1, vt = -1, a;
				char *vertex = tokenize(NULL, " ", &save);
				char *save2;
				char *x = tokenize(vertex, "/", &save2);
				a = atoi(x);
				if(a < 0) /* negative indices counts from end of array */
					v = m->verts->n + a;
				else if (a == 0) {
					snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "vertex index is not allowed to be 0");
                	goto error;
				} else
					v = a - 1;

                /* It seems you don't _have_ to specify the normal index, and can still have normals */
				if(v < m->norms->n)
                	vn = v;

				x = tokenize(NULL, "/", &save2);
				if(x) {
					a = x[0] ? atoi(x) : 0;
					if(a < 0)
						vt = m->texs->n + a;
					else if (a == 0) {
						snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "texture index is not allowed to be 0");
						goto error;
					}
					else
						vt = a - 1;
					x = tokenize(NULL, "/", &save2);
					if(x) {
						a = atoi(x);
						if(a < 0)
							vn = m->norms->n + a;
						else if (a == 0) {
							snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "normal index is not allowed to be 0");
							goto error;
						} else
							vn = a - 1;
					}
				}

				if(face->n == face->a) {
					face->a <<= 1;
					face->fv = realloc(face->fv, face->a * sizeof *face->fv);
				}
				OBJ_FACE_VERTEX *fp = &face->fv[face->n++];
				fp->v = v;
				fp->vt = vt;
				fp->vn = vn;
			}
        } else if(!strcmp(word, "g")) {
            char group[64];
            if(!read_line(group, sizeof group, f, m, &save)) {
				goto error;
			}
            char **newgroupp = al_add(m->groups);
            *newgroupp = strdup(group);
            current_group = *newgroupp;
        } else if(!strcmp(word, "s")) {
            char * ss = tokenize(NULL, " ", &save);
            if(!ss) {
                snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "unexpected end of file");
                goto error;
            }
			/* `s off` will atoi() to 0 */
            smoothing_group = atoi(ss);
        } else if(!strcmp(word, "o")) {
            char o[64];
        	if(!read_line(o, sizeof o, f, m, &save))
				goto error;

			if(m->name)
				free(m->name);
            m->name = strdup(o);
		} else if(!strcmp(word, "mtllib")) {
			/* There can be more than one material file per mtllib line,
			which implies that you cannot have spaces in the filenames of
			the .mtl files, yet blender will happily export them with
			spaces for you, hence this `#if 0` */
# if 0
			for(;;) {
				while(save[0] && strchr(" \t\r", save[0])) save++;
				if(save[0] == '\0' || save[0] == '\n')
					break;
				char *mtllib = tokenize(NULL, " ", &save);
				if(!mtllib)
					break;
				/* FIXME: actually load the mtllib */
				//printf("mtllib: '%s'\n", mtllib);
				(void)mtllib;
			}
#else
			char mtllib[64];
        	if(!read_line(mtllib, sizeof mtllib, f, m, &save))
				goto error;
			//printf("mtllib: '%s'\n", mtllib);
			(void)mtllib;
#endif
		} else if(!strcmp(word, "usemtl")) {
			char mtlname[64];
        	if(!read_line(mtlname, sizeof mtlname, f, m, &save))
				goto error;

			if(mtlname[0]) {
				/* TODO: do something with the material */
			} else {
				/* TODO: Docs says use a White material */
			}
		} else {
			/* TODO: I don't intend to support the more advanced geometries
			 * listed in the specification.
			 */
			snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "'%s' command is not supported", word);
            goto error;
		}
	}
	fclose(f);
	return m;
error:
	obj_free(m);
	fclose(f);
	return NULL;
}

int obj_save(OBJ_MESH *m, const char *filename) {
	int i;
	FILE *f = fopen(filename, "w");
	if(!f) {
		snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "Unable to create %s: %s", filename, strerror(errno));
		return 0;
	}
	fprintf(f, "# %s\n", filename);

	if(m->name)
		fprintf(f, "o %s\n", m->name);

	fprintf(f, "\n# %d Vertices: x y z\n", al_size(m->verts));
	for(i = 0; i < al_size(m->verts); i++) {
		double *vert = al_get(m->verts, i);
		fprintf(f, "v %g %g %g\n", vert[0], vert[1], vert[2]);
	}

	if(al_size(m->texs) > 0) {
		fprintf(f, "\n# %d Texture Coordinates: u v [w]\n", al_size(m->texs));
		for(i = 0; i < al_size(m->texs); i++) {
			double *tex = al_get(m->texs, i);
			if(tex[2]) {
				/* u,v and w components */
				fprintf(f, "vt %g %g %g\n", tex[0], 1.0 - tex[1], tex[2]);
			} else {
				/* only u,v components */
				fprintf(f, "vt %g %g\n", tex[0], 1.0 - tex[1]);
			}
		}
	}

	if(al_size(m->norms) > 0) {
		fprintf(f, "\n# %d Normals: x y z\n", al_size(m->norms));
		for(i = 0; i < al_size(m->norms); i++) {
			double *norm = al_get(m->norms, i);
			fprintf(f, "vn %g %g %g\n", norm[0], norm[1], norm[2]);
		}
	}

    char *current_group = NULL;
    int smoothing_group = 0;

	fprintf(f, "\n# %d Faces: v1/[vt1]/[vn1] ...\n", al_size(m->faces));
	for(i = 0; i < al_size(m->faces); i++) {
		int j;
        OBJ_FACE *face = al_get(m->faces, i);
        if(face->g != current_group) {
            current_group = face->g;
            fprintf(f, "g %s\n", current_group);
        }
        if(face->s != smoothing_group) {
            smoothing_group = face->s;
            fprintf(f, "s %d\n", smoothing_group);
        }
		fprintf(f, "f");
		for(j = 0; j < face->n; j++) {
			OBJ_FACE_VERTEX *fp = &face->fv[j];
            int v = fp->v, vt = fp->vt, vn = fp->vn;
			if(vt >= 0) {
				if(vn >= 0) {
					fprintf(f, " %d/%d/%d", v+1, vt+1, vn+1);
				} else {
					fprintf(f, " %d/%d", v+1, vt+1);
				}
			} else {
				if(vn > 0) {
					fprintf(f, " %d//%d", v+1, vn+1);
				} else {
					fprintf(f, " %d", v+1);
				}
			}
		}
		fprintf(f, "\n");
	}

	fclose(f);
	return 1;
}

void obj_normalize_size(OBJ_MESH *obj) {
	double xdim = obj->xmax - obj->xmin;
	double ydim = obj->ymax - obj->ymin;
	double zdim = obj->zmax - obj->zmin;

	double ratio;

	if(xdim > ydim) {
		if(xdim > zdim)
			ratio = 1.0/xdim;
		else
			ratio = 1.0/zdim;
	} else {
		if(ydim > zdim)
			ratio = 1.0/ydim;
		else
			ratio = 1.0/zdim;
	}

	int i;
	for(i = 0; i < obj_nverts(obj); i++) {
		double *v = obj_vert(obj, i);
		v[0] *= ratio;
		v[1] *= ratio;
		v[2] *= ratio;
	}

	obj->xmax *= ratio; obj->xmin *= ratio;
	obj->ymax *= ratio; obj->ymin *= ratio;
	obj->zmax *= ratio; obj->zmin *= ratio;
}

#ifndef OBJ_NODRAW
void obj_draw(OBJ_MESH *obj) {
    int i;

    for(i = 0; i < obj_nfaces(obj); i++) {
		int j;
		OBJ_FACE *face = obj_face(obj, i);

		fx_begin(FX_TRIANGLE_FAN);
		for(j = 0; j < face->n; j++) {
			OBJ_FACE_VERTEX *fp = &face->fv[j];
			vec3_t v = obj_vert(obj, fp->v);
			fx_vertex_v3(v);
			int ti = fp->vt;
			if(ti >= 0) {
				vec2_t t = obj_tex(obj, ti);
				fx_texcoord(t[0], t[1]);
			}
			int ni = fp->vn;
			if(ni >= 0) {
				vec3_t n0 = obj_norm(obj, ni);
				fx_normal_v3(n0);
			}
		}
		fx_end();
	}
}
#endif

#ifdef OBJ_TEST
int main(int argc, char *argv[]) {
    OBJ_MESH *obj;
    if(argc > 1) {
        obj = obj_load(argv[1]);
        if(!obj) {
            fprintf(stderr, "error: unable to load %s: %s\n", argv[1], obj_last_error());
            return 1;
        }
        printf("OBJ loaded\n");
        if(obj->name)
			printf("%s\n", obj->name);
        if(argc > 2) {
            if(!obj_save(obj, argv[2])) {
                fprintf(stderr, "error: unable to save %s: %s\n", argv[2], obj_last_error());
                return 1;
            }
            printf("OBJ saved\n");
        }
        obj_free(obj);
    }
    return 0;
}
#endif
