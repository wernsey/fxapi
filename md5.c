/*
 * 1. http://tfc.duke.free.fr/coding/md5-specs-en.html
 * 2. https://www.3dgep.com/loading-and-animating-md5-models-with-opengl/
 * 3. https://www.braynzarsoft.net/viewtutorial/q16390-27-loading-an-md5-model
 * 4. https://lwjglgamedev.gitbooks.io/3d-game-development-with-lwjgl/content/chapter19/chapter19.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>

#include "glmatrix.h"
#include "md5.h"

//#define MD5_TEST

#if defined(MD5_TEST)
#  define MD5_NODRAW
#  ifndef MD5_VERBOSE
#    define MD5_VERBOSE 1
#  endif
#endif

#ifndef MD5_VERBOSE
#  define MD5_VERBOSE 0
#endif

#ifndef MD5_NODRAW
#  include "bmp.h"
#  include "fx.h"
#  include "bm_cache.h"
#endif

#ifndef MD5_NODRAW
#  define MAX_VERTS 2048
static numeric_t VertPositions[3 * MAX_VERTS];

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

    MD5_MODEL *m;
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

static int toplevel(Parser *p) {
   int i, mi = 0;
   while(p->sym != L_END) {
       if(p->sym != L_IDENTIFIER) {
           fx_error("MD5: identifier expected");
           return 0;
        }
       if(accept_id(p, "MD5Version")) {
           if(!expect(p, L_NUMBER)) return 0;
           p->m->MD5Version = atoi(p->tok);
#if MD5_VERBOSE
           printf("MD5 version: %d\n", p->m->MD5Version);
#endif
       } else if(accept_id(p, "commandline")) {
           if(!expect(p, L_STRING)) return 0;
#if MD5_VERBOSE
           printf("command line: %s\n", p->tok);
#endif
           //p->m->MD5Version = atoi(p->tok);
       } else if(accept_id(p, "numJoints")) {
           if(!expect(p, L_NUMBER)) return 0;
           p->m->numJoints = atoi(p->tok);
#if MD5_VERBOSE
           printf("numJoints: %d\n", p->m->numJoints);
#endif
           p->m->joints = fx_calloc(p->m->numJoints, sizeof *p->m->joints);
       } else if(accept_id(p, "numMeshes")) {
           if(!expect(p, L_NUMBER)) return 0;
           p->m->numMeshes = atoi(p->tok);
#if MD5_VERBOSE
           printf("numMeshes: %d\n", p->m->numMeshes);
#endif
           p->m->meshes = fx_calloc(p->m->numMeshes, sizeof *p->m->meshes);
       } else if(accept_id(p, "joints")) {
          if(!expect(p, '{')) return 0;
          for(i = 0; i < p->m->numJoints; i++) {
              MD5_JOINT *j = &p->m->joints[i];
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
           if(mi >= p->m->numMeshes) {
               fx_error("MD5: too many meshes in model");
               return 0;
           }
           MD5_MESH *m = &p->m->meshes[mi];

           if(!expect(p, '{')) return 0;
            while(!accept(p, '}')) {
                if(p->sym != L_IDENTIFIER) {
                    fx_error("MD5: identifier expected");
                    return 0;
                }
                if(accept_id(p, "shader")) {
                    if(!expect(p, L_STRING)) return 0;
                    m->shader = strdup(p->tok);
                } else if(accept_id(p, "numverts")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    m->numverts = atoi(p->tok);
                    m->verts = fx_calloc(m->numverts, sizeof *m->verts);
                } else if(accept_id(p, "vert")) {

                    if(!expect(p, L_NUMBER)) return 0;
                    int vi = atoi(p->tok);
                    if(vi < 0 || vi >= m->numverts) {
                        fx_error("MD5: invalid vertex index");
                        return 0;
                    }

                    MD5_VERT *v = &m->verts[vi];
                    if(!expect_vec2(p, v->st)) return 0;
                    if(!expect(p, L_NUMBER)) return 0;
                    v->startWeight = atoi(p->tok);
                    if(!expect(p, L_NUMBER)) return 0;
                    v->countWeight = atoi(p->tok);
                } else if(accept_id(p, "numtris")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    m->numtris = atoi(p->tok);
                    m->tris = fx_calloc(m->numtris, sizeof *m->tris);
                } else if(accept_id(p, "tri")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    int ti = atoi(p->tok);
                    if(ti < 0 || ti >= m->numtris) {
                        fx_error("MD5: invalid tri index");
                        return 0;
                    }
                    MD5_TRI * t = &m->tris[ti];
                    if(!expect(p, L_NUMBER)) return 0;
                    t->vert[0] = atoi(p->tok);
                    if(!expect(p, L_NUMBER)) return 0;
                    t->vert[1] = atoi(p->tok);
                    if(!expect(p, L_NUMBER)) return 0;
                    t->vert[2] = atoi(p->tok);

                } else if(accept_id(p, "numweights")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    m->numweights = atoi(p->tok);
                    m->weights = fx_calloc(m->numweights, sizeof *m->weights);
                } else if(accept_id(p, "weight")) {
                    if(!expect(p, L_NUMBER)) return 0;
                    int wi = atoi(p->tok);
                    if(wi < 0 || wi >= m->numweights) {
                        fx_error("MD5: invalid weight index");
                        return 0;
                    }
                    MD5_WEIGHT *w = &m->weights[wi];
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
            printf("mesh: %s %d %d %d\n", m->shader, m->numverts, m->numtris, m->numweights);
#endif
           mi++;

       } else {
           fx_error("MD5: unexpected identifier '%s'", p->next_tok);
           return 0;
       }
   }
   if(mi < p->m->numMeshes - 1) {
        fx_error("MD5: too few meshes in model");
        return 0;
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

MD5_MODEL *md5_load(const char *filename) {
    char * text = fx_readfile(filename);
    Parser p;

    p.text = text;
    p.next_tok[0] = '\0';

    nextsym(&p);

    p.m = fx_malloc(sizeof *p.m);
    p.m->joints = NULL;
    p.m->meshes = NULL;

    if(!toplevel(&p)) goto error;

    precompute_normals(p.m);

    free(text);
    return p.m;

error:
    md5_free(p.m);
    free(text);
    return NULL;
}

void md5_free(MD5_MODEL *m) {
    int i;
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

void md5_draw(MD5_MODEL *m, double frame) {
    int i, mi, wi;

#if 0
	fx_set_target_color(0x999999);
    for(i = 0; i < m->numJoints; i++) {
        MD5_JOINT *j = &m->joints[i];
        int p = j->parent;
        if(p >= 0) {
            MD5_JOINT *j2 = &m->joints[p];
            fx_line_d(j->position[0], j->position[2], j->position[1], j2->position[0], j2->position[2], j2->position[1]);
        } else
            fx_point_d(j->position[0], j->position[2], j->position[1]);
    }
#endif

    fx_set_target_color(0x00FF00);
    for(mi = 0; mi < m->numMeshes; mi++) {
        MD5_MESH *me = &m->meshes[mi];

        if(md5_cache) {
            Bitmap *tex = bc_get(md5_cache, me->shader);
            if(!tex) {
#ifdef WARN_NO_TEXTURE
                fx_error("no texture for %s", me->shader);
#else
                continue;
#endif
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
#endif


#ifdef MD5_TEST
int main(int argc, char *argv[]) {
    MD5_MODEL *md5 = md5_load("bob_lamp_update/bob_lamp_update.md5mesh");
    if(!md5) {
        fprintf(stderr, "error: Couldn't load file: %s\n");
        return 1;
    }
    md5_free(md5);
    return 0;
}
#endif