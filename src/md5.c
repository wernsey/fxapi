/*
 * 1. http://tfc.duke.free.fr/coding/md5-specs-en.html
 * 2. https://www.3dgep.com/loading-and-animating-md5-models-with-opengl/
 * 3. https://www.braynzarsoft.net/viewtutorial/q16390-27-loading-an-md5-model
 * 4. https://lwjglgamedev.gitbooks.io/3d-game-development-with-lwjgl/content/chapter19/chapter19.html
 *
 * $ gcc -DMD5_TEST -Wall -g -O0 md5.c -lm
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>

#include "glmatrix.h"
#include "md5.h"

#if defined(MD5_TEST)
#  define MD5_NODRAW
#  ifndef MD5_VERBOSE
#    define MD5_VERBOSE 1
#  endif
#  define GL_MATRIX_IMPLEMENTATION
#  include "glmatrix.h"
#endif

#ifndef MD5_VERBOSE
#  define MD5_VERBOSE 0
#endif

#define WARN_NO_TEXTURE 0

#ifndef MD5_NODRAW
#  include "bmph.h"
#  include "fx.h"
#  include "bm_cache.h"
#endif

#  define MAX_VERTS 2048
static numeric_t VertPositions[3 * MAX_VERTS];
static numeric_t VertNormals[3 * MAX_VERTS];

#ifndef MD5_NODRAW
/* Place where textures can be found... */
static BmCache *md5_cache = NULL;
#endif

/* =============================================================================
 *  Lexical Analyzer
 * ============================================================================= */

enum S_result {
 L_ERROR = -1,
 L_END = 0,
 L_IDENTIFIER = 1,
 L_STRING,
 L_NUMBER
};

#define APPEND_CHAR(c) do {                      \
            if(i == n) {                         \
                fx_error("MD5: token too long"); \
                return L_ERROR;                  \
            }                                    \
            tok[i++] = c; } while(0)

static int scan(const char *in, char tok[], size_t n, const char **rem) {
    size_t i = 0;
    if(!in)
        in = *rem;
restart:
    if(!in)
        return L_END;
    while(isspace(*in))
        in++;

    if (*in == '\0') {
        *rem = in;
        return L_END;
    } else if(in[0] == '/' && in[1] == '/') {
        while(*++in != '\n') {
            if(*in == '\0') {
                *rem = in;
                return L_END;
            }
        }
        goto restart;
    } else if (strchr("{}()", *in)) {
        tok[0] = *in;
        tok[1] ='\0';
        *rem =  ++in;
        return tok[0];
    } else if (*in == '\"' || *in == '\'') {
        char term = *in++;
        while(*in != term) {
            if(!*in) {
                fx_error("MD5: unterminated string constant");
                return L_ERROR;
            } else if(i == n) {
                fx_error("MD5: token too long");
                return L_ERROR;
            }
            if(*in == '\\') {
                ++in;
                if(isdigit(*in)) {
                    int v = *in - '0';
                    while(isdigit(*++in))
                        v = v * 10 + (*in - '0');
                    tok[i++] = v & 0x7F;
                } else {
                    switch(*in) {
                        case '\0': fx_error("MD5: unterminated string constant"); return L_ERROR;
                        case 'n' : tok[i++] = '\n'; break;
                        case 'r' : tok[i++] = '\r'; break;
                        case 't' : tok[i++] = '\t'; break;
                        case 'b' : tok[i++] = '\b'; break;
                        case 'a' : tok[i++] = '\a'; break;
                        default : tok[i++] = *in; break;
                    }
                    in++;
                }
            } else
                tok[i++] = *(in++);
        }
        tok[i] = '\0';
        *rem = ++in;
        return L_STRING;
    } else if(isalpha(*in)) {
        while(isalnum(*in) || *in == '_')
            APPEND_CHAR(*in++);
        tok[i] = '\0';
        *rem = in;
        return L_IDENTIFIER;
    } else if(isdigit(*in) || *in == '-') {
		APPEND_CHAR(*in++);
        while(isdigit(*in))
			APPEND_CHAR(*in++);
		if(*in == '.') {
			APPEND_CHAR(*in++);
			while(isdigit(*in))
				APPEND_CHAR(*in++);
		}
		if(tolower(*in) == 'e') {
			APPEND_CHAR(*in++);
			if(strchr("+-", *in))
				APPEND_CHAR(*in++);
			while(isdigit(*in))
				APPEND_CHAR(*in++);
		}
        tok[i] = '\0';
        *rem = in;
        return L_NUMBER;
	}

    snprintf(tok, n, "bad character");
    return L_ERROR;
}

/* =============================================================================
 *  Parser
 * ============================================================================= */

#define TOKEN_SIZE 4096

typedef struct {
    int sym, last_sym;
    const char *text;
    char tok[TOKEN_SIZE], next_tok[TOKEN_SIZE];
} Parser;

static void nextsym(Parser *p) {
    memcpy(p->tok, p->next_tok, sizeof p->tok);
    p->last_sym = p->sym;
    p->sym = scan(NULL, p->next_tok, sizeof p->next_tok, &p->text);
}

static int accept(Parser *p, int s) {
    if(p->sym == s) {
        nextsym(p);
        return 1;
    }
    return 0;
}

static int accept_id(Parser *p, const char *id) {
    if(p->sym == L_IDENTIFIER && !strcmp(p->next_tok, id)) {
        nextsym(p);
        return 1;
    }
    return 0;
}

static int expect(Parser *p, int s) {
    if(accept(p, s))
        return 1;
    if(p->sym != L_ERROR)
        fx_error("MD5: unexpected symbol");
    return 0;
}

static int expect_vec2(Parser *p, double v[]) {
    if(!expect(p, '(')) return 0;
    if(!expect(p, L_NUMBER)) return 0;
    v[0] = atof(p->tok);
    if(!expect(p, L_NUMBER)) return 0;
    v[1] = atof(p->tok);
    if(!expect(p, ')')) return 0;
    return 1;
}

static int expect_vec3(Parser *p, double v[]) {
    if(!expect(p, '(')) return 0;
    if(!expect(p, L_NUMBER)) return 0;
    v[0] = atof(p->tok);
    if(!expect(p, L_NUMBER)) return 0;
    v[1] = atof(p->tok);
    if(!expect(p, L_NUMBER)) return 0;
    v[2] = atof(p->tok);
    if(!expect(p, ')')) return 0;
    return 1;
}

static int parse_mesh(Parser *p, MD5_MODEL *m) {
   int i, mi = 0;
   while(p->sym != L_END) {
       if(p->sym != L_IDENTIFIER) {
           fx_error("MD5: identifier expected");
           return 0;
        }
       if(accept_id(p, "MD5Version")) {
           if(!expect(p, L_NUMBER)) return 0;
           m->MD5Version = atoi(p->tok);
#if MD5_VERBOSE
           printf("MD5 version: %d\n", m->MD5Version);
#endif
       } else if(accept_id(p, "commandline")) {
           if(!expect(p, L_STRING)) return 0;
#if MD5_VERBOSE
           printf("command line: %s\n", p->tok);
#endif
           //m->MD5Version = atoi(p->tok);
       } else if(accept_id(p, "numJoints")) {
           if(!expect(p, L_NUMBER)) return 0;
           m->numJoints = atoi(p->tok);
#if MD5_VERBOSE
           printf("numJoints: %d\n", m->numJoints);
#endif
           m->joints = fx_calloc(m->numJoints, sizeof *m->joints);
       } else if(accept_id(p, "numMeshes")) {
           if(!expect(p, L_NUMBER)) return 0;
           m->numMeshes = atoi(p->tok);
#if MD5_VERBOSE
           printf("numMeshes: %d\n", m->numMeshes);
#endif
           m->meshes = fx_calloc(m->numMeshes, sizeof *m->meshes);
       } else if(accept_id(p, "joints")) {
          if(!expect(p, '{')) return 0;
          for(i = 0; i < m->numJoints; i++) {
              MD5_JOINT *j = &m->joints[i];
              if(!expect(p, L_STRING)) return 0;
              j->name = strdup(p->tok);
#if MD5_VERBOSE
              printf("joint: %s\n",j->name);
#endif
              if(!expect(p, L_NUMBER)) return 0;
              j->parent = atoi(p->tok);
              if(!expect_vec3(p, j->position)) return 0;
              if(!expect_vec3(p, j->orientation)) return 0;

              quat_calculateW(j->orientation, NULL);
          }
          if(!expect(p, '}')) return 0;
       } else if(accept_id(p, "mesh")) {
           if(mi >= m->numMeshes) {
               fx_error("MD5: too many meshes in model");
               return 0;
           }
           MD5_MESH *me = &m->meshes[mi];

           if(!expect(p, '{')) return 0;
            while(!accept(p, '}')) {
                if(p->sym != L_IDENTIFIER) {
                    fx_error("MD5: identifier expected");
                    return 0;
                }
                if(accept_id(p, "shader")) {
                    if(!expect(p, L_STRING)) return 0;
                    me->shader = strdup(p->tok);
                } else if(accept_id(p, "numverts")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    me->numverts = atoi(p->tok);
                    me->verts = fx_calloc(me->numverts, sizeof *me->verts);
                } else if(accept_id(p, "vert")) {

                    if(!expect(p, L_NUMBER)) return 0;
                    int vi = atoi(p->tok);
                    if(vi < 0 || vi >= me->numverts) {
                        fx_error("MD5: invalid vertex index");
                        return 0;
                    }

                    MD5_VERT *v = &me->verts[vi];
                    if(!expect_vec2(p, v->st)) return 0;
                    if(!expect(p, L_NUMBER)) return 0;
                    v->startWeight = atoi(p->tok);
                    if(!expect(p, L_NUMBER)) return 0;
                    v->countWeight = atoi(p->tok);
                } else if(accept_id(p, "numtris")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    me->numtris = atoi(p->tok);
                    me->tris = fx_calloc(me->numtris, sizeof *me->tris);
                } else if(accept_id(p, "tri")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    int ti = atoi(p->tok);
                    if(ti < 0 || ti >= me->numtris) {
                        fx_error("MD5: invalid tri index");
                        return 0;
                    }
                    MD5_TRI * t = &me->tris[ti];
                    if(!expect(p, L_NUMBER)) return 0;
                    t->vert[0] = atoi(p->tok);
                    if(!expect(p, L_NUMBER)) return 0;
                    t->vert[1] = atoi(p->tok);
                    if(!expect(p, L_NUMBER)) return 0;
                    t->vert[2] = atoi(p->tok);

                } else if(accept_id(p, "numweights")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    me->numweights = atoi(p->tok);
                    me->weights = fx_calloc(me->numweights, sizeof *me->weights);
                } else if(accept_id(p, "weight")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    int wi = atoi(p->tok);
                    if(wi < 0 || wi >= me->numweights) {
                        fx_error("MD5: invalid weight index");
                        return 0;
                    }
                    MD5_WEIGHT *w = &me->weights[wi];
                    if(!expect(p, L_NUMBER)) return 0;
                    w->joint = atoi(p->tok);
                    if(!expect(p, L_NUMBER)) return 0;
                    w->bias = atof(p->tok);
                    if(!expect_vec3(p, w->pos)) return 0;
                } else {
                    fx_error("MD5: unexpected identifier '%s'", p->tok);
                    return 0;
                }

            }
#if MD5_VERBOSE
            printf("mesh: %s %d %d %d\n", me->shader, me->numverts, me->numtris, me->numweights);
#endif
           mi++;

       } else {
           fx_error("MD5: unexpected identifier '%s'", p->next_tok);
           return 0;
       }
   }
   if(mi < m->numMeshes - 1) {
        fx_error("MD5: too few meshes in model");
        return 0;
    }
   return 1;
}

static int parse_anim(Parser *p, MD5_ANIM *a) {
   while(p->sym != L_END) {
       if(p->sym != L_IDENTIFIER) {
           fx_error("identifier expected");
            return 0;
        }
       if(accept_id(p, "MD5Version")) {
           if(!expect(p, L_NUMBER)) return 0;
           a->MD5Version = atoi(p->tok);
#if MD5_VERBOSE
           printf("MD5 version: %d\n", a->MD5Version);
#endif
       } else if(accept_id(p, "commandline")) {
           if(!expect(p, L_STRING)) return 0;
#if MD5_VERBOSE
           printf("command line: %s\n", p->tok);
#endif
           //a->MD5Version = strdup(p->tok);
       } else if(accept_id(p, "numFrames")) {
           if(!expect(p, L_NUMBER)) return 0;
           a->numFrames = atoi(p->tok);
#if MD5_VERBOSE
           printf("numFrames: %d\n", a->numFrames);
#endif
           a->bounds = fx_calloc(a->numFrames, sizeof *a->bounds);
           a->frames = fx_calloc(a->numFrames, sizeof *a->frames);
       } else if(accept_id(p, "numJoints")) {
           if(!expect(p, L_NUMBER)) return 0;
           a->numJoints = atoi(p->tok);
#if MD5_VERBOSE
           printf("numJoints: %d\n", a->numJoints);
#endif
           a->hierarachy = fx_calloc(a->numJoints, sizeof *a->hierarachy);
           a->baseframe = fx_calloc(a->numJoints, sizeof *a->baseframe);
       } else if(accept_id(p, "numAnimatedComponents")) {
           if(!expect(p, L_NUMBER)) return 0;
           a->numAnimatedComponents = atoi(p->tok);
#if MD5_VERBOSE
           printf("numAnimatedComponents: %d\n", a->numAnimatedComponents);
#endif
       } else if(accept_id(p, "frameRate")) {
           if(!expect(p, L_NUMBER)) return 0;
           a->frameRate = atoi(p->tok);
#if MD5_VERBOSE
           printf("frameRate: %d\n", a->frameRate);
#endif
       } else if(accept_id(p, "hierarchy")) {
           if(!expect(p, '{')) return 0;
           int i;
           for(i = 0; i < a->numJoints; i++) {
               MD5_HIERARCHY *h = &a->hierarachy[i];
               if(!expect(p, L_STRING)) return 0;
               h->name = strdup(p->tok);
               if(!expect(p, L_NUMBER)) return 0;
               h->parent = atoi(p->tok);
               if(!expect(p, L_NUMBER)) return 0;
               h->flags = atoi(p->tok);
               if(!expect(p, L_NUMBER)) return 0;
               h->startIndex = atoi(p->tok);
#if MD5_VERBOSE
               printf("%2d %14s %2d %04X %4d\n", i, h->name, h->parent, h->flags, h->startIndex);
#endif
           }
           if(!expect(p, '}')) return 0;
       } else if(accept_id(p, "bounds")) {
#if MD5_VERBOSE
           printf("bounds:\n");
#endif
           if(!expect(p, '{')) return 0;
           int i;
           for(i = 0; i < a->numFrames; i++) {
               MD5_BOUND *b = &a->bounds[i];
               if(!expect_vec3(p, b->min)) return 0;
               if(!expect_vec3(p, b->max)) return 0;
#if MD5_VERBOSE
               printf("   %-3d (%6g %6g %6g) (%6g %6g %6g)\n", i, b->min[0], b->min[1], b->min[2], b->max[0], b->max[1], b->max[2]);
#endif
           }
           if(!expect(p, '}')) return 0;
       } else if(accept_id(p, "baseframe")) {
#if MD5_VERBOSE
           printf("baseframe:\n");
#endif
           if(!expect(p, '{')) return 0;
           int i;
           for(i = 0; i < a->numJoints; i++) {
               MD5_FRAME_JOINT *b = &a->baseframe[i];
               if(!expect_vec3(p, b->position)) return 0;
               if(!expect_vec3(p, b->orientation)) return 0;
               quat_calculateW(b->orientation, NULL);
#if MD5_VERBOSE
               printf("   %-3d (%6g %6g %6g) (%6g %6g %6g)\n", i,
                b->position[0], b->position[1], b->position[2],
                b->orientation[0], b->orientation[1], b->orientation[2]);
#endif
           }
           if(!expect(p, '}')) return 0;
       } else if(accept_id(p, "frame")) {
           int index;
           if(!expect(p, L_NUMBER)) return 0;
           index = atoi(p->tok);
#if MD5_VERBOSE
           printf("frame %d:\n", index);
#endif
           MD5_FRAME *f = &a->frames[index];
           f->components = fx_calloc(a->numAnimatedComponents, sizeof *f->components);
           if(!expect(p, '{')) return 0;
           int i;
           for(i = 0; i < a->numAnimatedComponents; i++) {
                if(!expect(p, L_NUMBER)) return 0;
                f->components[i] = atof(p->tok);
#if MD5_VERBOSE
                printf("%g ", f->components[i]);
                if((i + 1) % 6 == 0) printf("\n");
#endif
           }
           if(!expect(p, '}')) return 0;
#if MD5_VERBOSE
           printf("\n");
#endif
       } else {
           fx_error("unexpected identifier '%s'", p->next_tok);
           return 0;
       }
   }

   return 1;
}

/* =============================================================================
 *  API
 * ============================================================================= */

static void precompute_normals(MD5_MODEL *m) {
    int mi, vi, ti, wi;
    for(mi = 0; mi < m->numMeshes; mi++) {
        MD5_MESH *me = &m->meshes[mi];

        memset(VertPositions, 0, me->numverts * 3 * sizeof VertPositions[0]);
        for(vi = 0; vi < me->numverts; vi++) {

            assert(vi < MAX_VERTS && "You need to increase MAX_VERTS");

            MD5_VERT *v = &me->verts[vi];

            numeric_t *finalPos = VertPositions + vi * 3;

            for(wi = 0; wi < v->countWeight; wi++) {
                MD5_WEIGHT *w = &me->weights[v->startWeight + wi];
                MD5_JOINT *j = &m->joints[w->joint];

                numeric_t wv[4];
                quat_rotate(j->orientation, w->pos, wv);

                finalPos[0] += (j->position[0] + wv[0] ) * w->bias;
                finalPos[1] += (j->position[1] + wv[1] ) * w->bias;
                finalPos[2] += (j->position[2] + wv[2] ) * w->bias;
            }
        }

        for(ti = 0; ti < me->numtris; ti++) {
            MD5_TRI *t = &me->tris[ti];
            numeric_t *v0 = VertPositions + t->vert[0] * 3;
            numeric_t *v1 = VertPositions + t->vert[1] * 3;
            numeric_t *v2 = VertPositions + t->vert[2] * 3;

            numeric_t t1[3], t2[3];

            vec3_cross(vec3_subtract(v2, v0, t1), vec3_subtract(v1, v0, t2), NULL);

            vec3_normalize(t1, NULL);

            vec3_add(me->verts[ t->vert[0] ].normal, t1, NULL);
            vec3_add(me->verts[ t->vert[1] ].normal, t1, NULL);
            vec3_add(me->verts[ t->vert[2] ].normal, t1, NULL);
        }

        for(vi = 0; vi < me->numverts; vi++) {
            vec3_normalize(me->verts[vi].normal, NULL);

            // This part undoes the rotation around the joint
            // so that it can be rotated around the joint again
            // when animating
            numeric_t normal[3];
            vec3_set(me->verts[vi].normal, normal);
            MD5_VERT *v = &me->verts[vi];
            for(wi = 0; wi < v->countWeight; wi++) {
                MD5_WEIGHT *w = &me->weights[v->startWeight + wi];
                MD5_JOINT *j = &m->joints[w->joint];

                numeric_t wv[4], qinv[4];

                quat_inverse(j->orientation, qinv);
                quat_rotate(qinv, normal, wv);

                vec3_add(w->normal, wv, NULL);
            }
        }

        for(wi = 0; wi < me->numweights; wi++) {
            vec3_normalize(me->weights[wi].normal, NULL);
        }
    }
}

MD5_MODEL *md5_load_mesh(const char *filename) {
    char * text = fx_readfile(filename);
    Parser p;

    p.text = text;
    p.next_tok[0] = '\0';

    nextsym(&p);

    MD5_MODEL *m = fx_malloc(sizeof *m);
    memset(m, 0, sizeof *m);

    if(!parse_mesh(&p, m)) goto error;

    precompute_normals(m);

    free(text);
    return m;

error:
    md5_free(m);
    free(text);
    return NULL;
}

void md5_free(MD5_MODEL *m) {
    int i;
    if(!m)
        return;
    if(m->joints) {
        for(i = 0; i < m->numJoints; i++) {
            free(m->joints[i].name);
        }
        free(m->joints);
    }
    if(m->meshes) {
        for(i = 0; i < m->numMeshes; i++) {
            MD5_MESH *me = &m->meshes[i];
            free(me->shader);
            free(me->tris);
            free(me->verts);
            free(me->weights);
        }
        free(m->meshes);
    }
    free(m);
}

MD5_ANIM *md5_load_anim(const char *filename) {
    char * text = fx_readfile(filename);
    Parser p;

    p.text = text;
    p.next_tok[0] = '\0';

    nextsym(&p);

    MD5_ANIM *a = fx_malloc(sizeof *a);
    memset(a, 0, sizeof *a);

    if(!parse_anim(&p, a)) goto error;

    free(text);
    return a;

error:
    md5_free_anim(a);
    free(text);
    return NULL;
}

void md5_free_anim(MD5_ANIM *a) {
    int i;
    if(!a)
        return;
    if(a->hierarachy) {
        for(i = 0; i < a->numJoints; i++) {
            free(a->hierarachy[i].name);
        }
        free(a->hierarachy);
    }
    if(a->bounds) {
        free(a->bounds);
    }
    if(a->baseframe) {
        free(a->baseframe);
    }
    if(a->frames) {
        for(i = 0; i < a->numFrames; i++) {
            free(a->frames[i].components);
        }
        free(a->frames);
    }
    free(a);
}

#ifndef MD5_NODRAW

static void free_cache() {
    if(md5_cache)
        bc_destroy(md5_cache);
}

void md5_set_shader(const char *shader_name, Bitmap *texture) {
    if(!md5_cache) {
        md5_cache = bc_create();
        atexit(free_cache);
    }
    bc_put(md5_cache, shader_name, texture);
}

void md5_draw(MD5_MODEL *m) {
    int i, mi, wi;

    for(mi = 0; mi < m->numMeshes; mi++) {
        MD5_MESH *me = &m->meshes[mi];

        if(md5_cache) {
            Bitmap *tex = bc_get(md5_cache, me->shader);
            if(!tex) {
#if WARN_NO_TEXTURE
                fx_error("no texture for %s", me->shader);
#endif
                continue;
            }
            fx_set_texture(tex);
        }

        memset(VertPositions, 0, me->numverts * 3 * sizeof VertPositions[0]);
        for(i = 0; i < me->numverts; i++) {

            assert(i < MAX_VERTS && "You need to increase MAX_VERTS");

            MD5_VERT *v = &me->verts[i];

            numeric_t *finalPos = VertPositions + i * 3;

            for(wi = 0; wi < v->countWeight; wi++) {
                MD5_WEIGHT *w = &me->weights[v->startWeight + wi];
                MD5_JOINT *j = &m->joints[w->joint];

                numeric_t wv[4];
                quat_rotate(j->orientation, w->pos, wv);

                finalPos[0] += (j->position[0] + wv[0] ) * w->bias;
                finalPos[1] += (j->position[1] + wv[1] ) * w->bias;
                finalPos[2] += (j->position[2] + wv[2] ) * w->bias;
            }
        }

       int c = 0;
       fx_begin(FX_TRIANGLES);
       for(i = 0; i < me->numtris; i++) {
           MD5_TRI *t = &me->tris[i];

            if(c >= 512 - 3) {
                fx_end();
                fx_begin(FX_TRIANGLES);
                c = 0;
            }

            int k;
            for(k = 0; k < 3; k++) {
                MD5_VERT *v = &me->verts[t->vert[k]];
                numeric_t *vp = VertPositions + t->vert[k] * 3;
                fx_vertex(vp[0], vp[2], vp[1]);
                fx_texcoord(v->st[0], v->st[1]);
                fx_normal(v->normal[0], v->normal[2], v->normal[1]);
                c++;
            }
       }
       fx_end();
#if 0
        // Drawing the normals
        fx_set_target_color(0x00FF00);
        for(i = 0; i < me->numverts; i++) {

            MD5_VERT *v = &me->verts[i];
            numeric_t *finalPos = VertPositions + i * 3;

            numeric_t normal[3];
            vec3_scale(v->normal, 1.0, normal);
            vec3_add(normal, finalPos, NULL);

            fx_line_d(finalPos[0], finalPos[2], finalPos[1], normal[0], normal[2], normal[1]);
        }
#endif
    }

}

#define MAX_FRAME_JOINTS    128
static MD5_FRAME_JOINT FrameJoints[MAX_FRAME_JOINTS];

static void make_frame_joint(MD5_FRAME_JOINT *in, MD5_FRAME *frame, MD5_HIERARCHY *h , MD5_FRAME_JOINT *out) {
    int flags = h->flags, d = h->startIndex;
    out->position[0] = (flags & 0x01) ? frame->components[d++] : in->position[0];
    out->position[1] = (flags & 0x02) ? frame->components[d++] : in->position[1];
    out->position[2] = (flags & 0x04) ? frame->components[d++] : in->position[2];
    out->orientation[0] = (flags & 0x08) ? frame->components[d++] : in->orientation[0];
    out->orientation[1] = (flags & 0x10) ? frame->components[d++] : in->orientation[1];
    out->orientation[2] = (flags & 0x20) ? frame->components[d++] : in->orientation[2];
    quat_calculateW(out->orientation, NULL);
}

#define LERP 1

static void build_skeleton(MD5_MODEL *m, MD5_ANIM *a, MD5_FRAME *f0, MD5_FRAME *f1, numeric_t interp, MD5_FRAME_JOINT *out) {
    int i;

    assert(m->numJoints == a->numJoints);
    assert(a->numJoints <= MAX_FRAME_JOINTS);

    for(i = 0; i < a->numJoints; i++) {
        MD5_FRAME_JOINT *j = &out[i];
        MD5_HIERARCHY *h = &a->hierarachy[i];

#if LERP
        MD5_FRAME_JOINT t0, t1;
        make_frame_joint(&a->baseframe[i], f0, h, &t0);
        make_frame_joint(&a->baseframe[i], f1, h, &t1);
#else
        make_frame_joint(&a->baseframe[i], f0, h, j);
#endif
        int p = h->parent;
        if(p >= 0) {
            numeric_t rpos[4];
            MD5_FRAME_JOINT *jp = &out[p];

#if LERP
            vec3_lerp(t0.position, t1.position, interp, j->position);
            // There was some glitching when using slerp rather than lerp. Why?
            // quat_slerp(t0.orientation, t1.orientation, interp, j->orientation);
            vec4_lerp(t0.orientation, t1.orientation, interp, j->orientation);
#endif
            quat_rotate(jp->orientation, j->position, rpos);
            vec3_add(jp->position, rpos, j->position);

            quat_multiply(jp->orientation, j->orientation, j->orientation);
            quat_normalize(j->orientation, NULL);
        } else {
#if LERP
            vec3_lerp(t0.position, t1.position, interp, j->position);
            // quat_slerp(t0.orientation, t1.orientation, interp, j->orientation);
            vec4_lerp(t0.orientation, t1.orientation, interp, j->orientation);
#endif

        }
    }
}

void md5_draw_frame(MD5_MODEL *m, MD5_ANIM *a, double frame) {
    int i;

    int f0 = (int)frame;
    int f1 = f0 + 1;
    if(f1 >= a->numFrames)
        f1 = 0;

    numeric_t interp = frame - f0;

    MD5_FRAME *frame0 = &a->frames[f0];
    MD5_FRAME *frame1 = &a->frames[f1];

    build_skeleton(m, a, frame0, frame1, interp, FrameJoints);

#if 0
    for(i = 0; i < a->numJoints; i++) {
        MD5_FRAME_JOINT *j = &FrameJoints[i];
        MD5_HIERARCHY *h = &a->hierarachy[i];

        int p = h->parent;
        if(p >= 0) {
            MD5_FRAME_JOINT *jp = &FrameJoints[p];
            numeric_t *pos = j->position, *par = jp->position;
            fx_line_d(pos[0], pos[2], pos[1], par[0], par[2], par[1]);
        }
    }
#endif

#if 1
    int mi, wi;
    for(mi = 0; mi < m->numMeshes; mi++) {
        MD5_MESH *me = &m->meshes[mi];

        if(md5_cache) {
            Bitmap *tex = bc_get(md5_cache, me->shader);
            if(!tex) {
#if WARN_NO_TEXTURE
                fx_error("no texture for %s", me->shader);
#endif
                continue;
            }
            fx_set_texture(tex);
        }

        memset(VertPositions, 0, me->numverts * 3 * sizeof VertPositions[0]);
        memset(VertNormals, 0, me->numverts * 3 * sizeof VertNormals[0]);
        for(i = 0; i < me->numverts; i++) {

            assert(i < MAX_VERTS && "You need to increase MAX_VERTS");

            MD5_VERT *v = &me->verts[i];

            vec3_t finalPos = VertPositions + i * 3;
            vec3_t finalNorm = VertNormals + i * 3;

            for(wi = 0; wi < v->countWeight; wi++) {
                MD5_WEIGHT *w = &me->weights[v->startWeight + wi];
                MD5_FRAME_JOINT *j = &FrameJoints[w->joint];

                numeric_t wv[4];
                quat_rotate(j->orientation, w->pos, wv);

                finalPos[0] += (j->position[0] + wv[0] ) * w->bias;
                finalPos[1] += (j->position[1] + wv[1] ) * w->bias;
                finalPos[2] += (j->position[2] + wv[2] ) * w->bias;

                quat_rotate(j->orientation, w->normal, wv);
                vec3_add(finalNorm, vec3_scale(wv, w->bias, NULL), NULL);
            }
            vec3_normalize(finalNorm, NULL);
        }

       int c = 0;
       fx_begin(FX_TRIANGLES);
       for(i = 0; i < me->numtris; i++) {
           MD5_TRI *t = &me->tris[i];

            if(c >= 512 - 3) {
                fx_end();
                fx_begin(FX_TRIANGLES);
                c = 0;
            }

            int k;
            for(k = 0; k < 3; k++) {
                MD5_VERT *v = &me->verts[t->vert[k]];
                vec3_t vp = VertPositions + t->vert[k] * 3;
                vec3_t np = VertNormals + t->vert[k] * 3;
                fx_vertex(vp[0], vp[2], vp[1]);
                fx_texcoord(v->st[0], v->st[1]);
                fx_normal(np[0], np[2], np[1]);
                c++;
            }
       }
       fx_end();

#if 0
        // Drawing the normals
        fx_set_target_color(0x00FF00);
        for(i = 0; i < me->numverts; i++) {

            MD5_VERT *v = &me->verts[i];
            vec3_t finalPos = VertPositions + i * 3;
            vec3_t finalNorm = VertNormals + i * 3;

            numeric_t normal[3];
            vec3_scale(finalNorm, 3, normal);
            vec3_add(normal, finalPos, NULL);

            fx_line_d(finalPos[0], finalPos[2], finalPos[1], normal[0], normal[2], normal[1]);
        }
#endif

    }
#endif
}

#endif // ifndef MD5_NO_DRAW


#ifdef MD5_TEST
int main(int argc, char *argv[]) {
    MD5_MODEL *md5 = md5_load_mesh("bob_lamp_update/bob_lamp_update.md5mesh");
    if(!md5) {
        fprintf(stderr, "error: Couldn't load file\n");
        return 1;
    }

    MD5_ANIM *anim = md5_load_anim("bob_lamp_update/bob_lamp_update.md5anim");
    if(!anim) {
        fprintf(stderr, "error: Couldn't load anim file\n");
        md5_free(md5);
        return 1;
    }

    md5_free_anim(anim);
    md5_free(md5);
    return 0;
}
#endif