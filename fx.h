#ifndef FX_H
#define FX_H

#ifndef GL_MATRIX_H
/* #include "gl-matrix/gl-matrix.h" */
typedef double *vec3_t;
typedef double *mat4_t;
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
void fx_end();

void fx_vertex(double x, double y, double z);
void fx_vertex_v3(vec3_t v);
void fx_texcoord(double u, double v);
void fx_normal(double x, double y, double z);
void fx_normal_v3(vec3_t v);
void fx_color(double r, double g, double b);

mat4_t fx_get_model();
void fx_set_model(mat4_t m);
mat4_t fx_get_view();
void fx_set_view(mat4_t m);
mat4_t fx_get_projection();
void fx_set_projection(mat4_t m);

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

#endif