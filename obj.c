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

OBJ_MESH *obj_create() {
	OBJ_MESH *m = malloc(sizeof *m);
	m->verts = al_create(3 * sizeof(double));
	m->norms = al_create(3 * sizeof(double));
	m->texs = al_create(3 * sizeof(double));
	m->faces = al_create(sizeof(OBJ_FACE));

	m->groups = al_create(sizeof(char *));
    m->groups->dtor = free;

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
		while(str[0]) str++;
		*save = str;
		return p;
	}
	p[0] = '\0';
	*save = ++p;
	return str;
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
			int i;
            OBJ_FACE *face = al_add(m->faces);
            face->g = current_group;
            face->s = smoothing_group;
			/* Officially, faces can have more than 3
				vertices, but I don't support that */
			for(i = 0; i < 3; i++) {
				while(isspace(save[0])) save++;
				int v = -1, vn = -1, vt = -1;
				char *vertex = tokenize(NULL, " ", &save);
				char *save2;
				char *x = tokenize(vertex, "/", &save2);
				v = atoi(x) - 1;

                /* The Teapot.obj sample I've been testing has normals,
                 * but does not specify them explicitly in the faces,
                 * hence the need for the following.
                 * Is this official?
                 * Would I need to do something similar for vt?
                 */
                //vn = v;

				x = tokenize(NULL, "/", &save2);
				if(x) {
					vt = x[0] ? atoi(x) - 1 : -1;
					x = tokenize(NULL, "/", &save2);
					if(x) {
						vn = atoi(x) - 1;
					}
				}
                face->v[i] = v;
                face->vt[i] = vt;
                face->vn[i] = vn;
				/* FIXME: Negative vertex indices are allowed by indexing from the end of the array. */
			}
        } else if(!strcmp(word, "g")) {
            char group[64];
            unsigned i = 0;
            while(isspace(save[0])) save++;
            while(save[0] != '\n') {
                if(save[0] == '\0') {
                    snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "unexpected end of file");
                    obj_free(m);
					fclose(f);
                    return NULL;
                } else if(save[0] == '\r')
                    continue;
                if(i == sizeof group - 1) {
                    snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "can't handle group names that long");
                    obj_free(m);
					fclose(f);
                    return NULL;
                }
                group[i++] = *save++;
            }
            group[i++] = '\0';
            char **newgroupp = al_add(m->groups);
            *newgroupp = strdup(group);
            current_group = *newgroupp;

        } else if(!strcmp(word, "s")) {
            char * ss = tokenize(NULL, " ", &save);
            if(!ss) {
                snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "unexpected end of file");
                obj_free(m);
				fclose(f);
                return NULL;
            }
            smoothing_group = atoi(ss);
		} else {
			/* TODO: Add support for
			 * - 'mtllib' - Materials
			 * I don't intend to support the more advanced geometries
			 * listed in the specification.
			 */
			snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "'%s' command is not supported", word);
            obj_free(m);
			fclose(f);
            return NULL;
		}
	}
	fclose(f);
	return m;
}

int obj_save(OBJ_MESH *m, const char *filename) {
	int i;
	FILE *f = fopen(filename, "w");
	if(!f) {
		snprintf(Obj_Err_Buf, sizeof Obj_Err_Buf, "Unable to create %s: %s", filename, strerror(errno));
		return 0;
	}
	fprintf(f, "# %s\n", filename);
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
		for(j = 0; j < 3; j++) {
            int v = face->v[j], vt = face->vt[j], vn = face->vn[j];
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

#ifndef OBJ_NODRAW
void obj_draw(OBJ_MESH *obj) {
    int i;

    for(i = 0; i < obj_nfaces(obj); i++) {
		int j;
		OBJ_FACE *face = obj_face(obj, i);

		fx_begin(FX_TRIANGLES);
		for(j = 0; j < 3; j++) {
			vec3_t v = obj_vert(obj, face->v[j]);
			fx_vertex_v3(v);
			int ti = face->vt[j];
			if(ti >= 0) {
				vec2_t t = obj_tex(obj, ti);
				fx_texcoord(t[0], t[1]);
			}
			int ni = face->vn[j];
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
