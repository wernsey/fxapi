#ifndef FX_H
#define FX_H

#ifndef GL_MATRIX_H
/* #include "glmatrix.h" */
typedef double numeric_t;
typedef numeric_t *vec3_t;
typedef numeric_t *mat4_t;
#endif
#ifndef BMP_H
/* #include "bmp.h" */
typedef struct Bitmap Bitmap;
#endif

typedef enum {FX_TRIANGLES, FX_TRIANGLE_STRIP, FX_TRIANGLE_FAN} fx_mode;

void fx_set_viewport(Bitmap *target);

void fx_cleanup();

void fx_clear_zbuf();

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

void fx_lighting(int enabled);
void fx_set_ambient(double r, double g, double b);
void fx_set_diffuse_color(double r, double g, double b);
void fx_set_diffuse_direction(double x, double y, double z);

void fx_backface(int enabled);

void fx_blend(int enabled);

void fx_fog(int enabled);
void fx_fog_params(double r, double g, double b, double near, double far);

void fx_set_pick(Bitmap *pick);

void fx_ctorgb(unsigned int c, double *r, double *g, double *b);

#define BB_CYLINDRICAL      1
#define BB_SHADED           2
#define BB_ANCHOR_BOTTOM    4
#define BB_ANCHOR_TOP       8

void fx_billboard(vec3_t pos, double scale, int flags);
void fx_billboard_eye(vec3_t pos, vec3_t eye, double scale, int flags);

void fx_line(vec3_t p0, vec3_t p1);

#endif