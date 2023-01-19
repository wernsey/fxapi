#ifndef FX_H
#define FX_H

#if defined(__cplusplus)
extern "C" {
#endif

#ifndef GL_MATRIX_H
/* #include "glmatrix.h" */
typedef double numeric_t;  /* You need to be really sure
                            * this is the same as in glmatrix.h
                            */
typedef numeric_t *vec2_t;
typedef numeric_t *vec3_t;
typedef numeric_t *mat4_t;
#endif

#ifndef BMP_H
/* #include "bmp.h" */
typedef struct Bitmap Bitmap;
#endif

typedef enum {FX_TRIANGLES, FX_TRIANGLE_STRIP, FX_TRIANGLE_FAN} fx_mode;

typedef enum {FX_FOG_NONE = 0, FX_FOG_LINEAR, FX_FOG_EXP, FX_FOG_EXP2} fg_fog_type;

void fx_set_viewport(Bitmap *target);

void fx_make_projection(numeric_t fovy, numeric_t near, numeric_t far);

void fx_cleanup();

void fx_clear_zbuf();

void fx_draw_zbuf(Bitmap *dest);

void fx_begin(fx_mode mode);
int fx_end();

int fx_vertex(double x, double y, double z);
int fx_texcoord(double u, double v);
int fx_normal(double x, double y, double z);
int fx_color(double r, double g, double b);

#define fx_vertex_v3(v3) fx_vertex(v3[0], v3[1], v3[2])
#define fx_normal_v3(v3) fx_normal(v3[0], v3[1], v3[2])
#define fx_color_v3(v3) fx_color(v3[0], v3[1], v3[2])

void fx_set_model(mat4_t m);
void fx_set_view(mat4_t m);
void fx_set_projection(mat4_t m);

void fx_save_model(mat4_t dest);
void fx_save_view(mat4_t dest);
void fx_save_projection(mat4_t dest);

void fx_set_texture(Bitmap *texture);
void fx_transparent(int enabled);

void fx_all_lighting(int enabled);

void fx_light_enable(unsigned int index);

void fx_light_disable(unsigned int index);

void fx_set_ambient(double r, double g, double b);

void fx_set_diffuse_color(unsigned int index, double r, double g, double b);
void fx_set_diffuse_direction(unsigned int index, double x, double y, double z);

void fx_set_material(vec3_t ambient, vec3_t diffuse, vec3_t emissive);
void fx_reset_material();

void fx_backface(int enabled);

void fx_blend(int enabled);

void fx_texture_dither(int enabled);

void fx_fog(fg_fog_type type);
void fx_fog_params(double r, double g, double b, double near, double far, double density);

void fx_set_pick(Bitmap *pick);

void fx_set_target_color(unsigned int color);

unsigned int fx_get_target_color();

void fx_ctorgb(unsigned int c, double *r, double *g, double *b);

#define BB_CYLINDRICAL      1
#define BB_SHADED           2
#define BB_ANCHOR_BOTTOM    4
#define BB_ANCHOR_TOP       8

void fx_billboard(vec3_t pos, double scale, int flags);
void fx_billboard_eye(vec3_t pos, vec3_t eye, double scale, int flags);

void fx_line(vec3_t p0, vec3_t p1);

void fx_line_d(numeric_t x0, numeric_t y0, numeric_t z0, numeric_t x1, numeric_t y1, numeric_t z1);

void fx_point(vec3_t p0);

void fx_point_d(numeric_t x0, numeric_t y0, numeric_t z0);

extern void (*fx_error)(const char *fmt, ...);
extern char *(*fx_readfile)(const char *fname);

void *fx_malloc(size_t size);
void *fx_realloc(void* p, size_t size);
void *fx_calloc(size_t nobj, size_t size);

#if defined(__cplusplus)
} /* extern "C" */
#endif

#endif