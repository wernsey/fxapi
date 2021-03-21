/*
 * https://github.com/ssloy/tinyrenderer/wiki/Lesson-0:-getting-started
 * https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation
 *
 *
 * Future Ideas:
 * * MIP Maps?
 *   See https://github.com/thebeast33/cro_lib
 */

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>
#include <stdarg.h>
#include <assert.h>

#define GL_MATRIX_IMPLEMENTATION
#include "glmatrix.h"

#include "bmp.h"

#include "fx.h"

static Bitmap *Target = NULL;
static Bitmap *Pick = NULL;

static int V_Width = 0, V_Height = 0;

static double M_Model[16], M_View[16], M_Projection[16];
static double M_ModelView[16], M_Xform[16];
static double M_NormalXform[16];
static double M_View_Inv[16];

static int Xform_dirty = 1;

static int Backface = 0;

static fx_mode Mode;
static int Begun = 0;

#define VARRAY_SIZE	512
/* Vertex positions (transformed) */
static double VArray[VARRAY_SIZE][4];
static int NVerts = 0;
/* Vertex texture coordinates */
static double TArray[VARRAY_SIZE][2];
static int NTexs = 0;
/* Vertex normals */
static double NArray[VARRAY_SIZE][3];
static int NNorms = 0;
/* Vertex colors */
static double CArray[VARRAY_SIZE][3];
static int NCols = 0;

static Bitmap *Texture = NULL;

static int TextureDither = 0;

static int Transparent = 0;

static int Blend = 0;

static double *ZBuf = NULL;
#define ZBUF(X,Y) ZBuf[(Y) * V_Width + (X)]

static int Lighting = 0;
static double AmbientColor[3] = {0.5, 0.5, 0.5};

/* TODO: At the moment there's only the one directional light.
There ought to be more, and I ought to add support for point lights. */
static double DiffuseColor[3] = {0.5, 0.5, 0.5};
static double DiffuseDirection[3] = {0, 1, 0};

static int Material_Enabled = 0;
static numeric_t Material_Ambient[3] = {0.2, 0.2, 0.2};
static numeric_t Material_Diffuse[3] = {0.8, 0.8, 0.8};
static numeric_t Material_Emissive[3] = {0.0, 0.0, 0.0};

static fg_fog_type Fog_Type = FX_FOG_NONE;
static double Fog_Near = 0.5, Fog_Far = 1.0, Fog_Density = 0.05;
static double Fog_Color[] = {1.0, 1.0, 1.0};

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)

static vec3_t vec3_clamp01(vec3_t v) {
    v[0] = v[0] > 1 ? 1 : (v[0] < 0 ? 0 : v[0]);
    v[1] = v[1] > 1 ? 1 : (v[1] < 0 ? 0 : v[1]);
    v[2] = v[2] > 1 ? 1 : (v[2] < 0 ? 0 : v[2]);
    return v;
}

void fx_set_viewport(Bitmap *target) {

    Target = target;

    V_Width = bm_width(Target);
    V_Height = bm_height(Target);

    ZBuf = fx_calloc(V_Width * V_Height, sizeof *ZBuf);

	mat4_identity(M_Model);
	mat4_identity(M_View);

	double ratio = (double)V_Width / V_Height;

	mat4_perspective(60.0, ratio, 0.1, 10.0, M_Projection);
}

void fx_cleanup() {
    assert(ZBuf);
    free(ZBuf);
    ZBuf = NULL;

    Target = NULL;
    Pick = NULL;
    Texture = NULL;

    V_Width = V_Height = 0;

    Xform_dirty = 1;

    Backface = 0;
    Begun = 0;

    NVerts = NTexs = NCols = NNorms = 0;

    Transparent = 0;
    Lighting = 0;
    Blend = 0;
    Fog_Type = FX_FOG_NONE;
}

void fx_clear_zbuf() {
    int i;
    assert(ZBuf);
    for(i = 0; i < V_Width * V_Height; i++)
        ZBuf[i] = 1.0;
}

static void barycentric(vec3_t a, vec3_t b, vec3_t c, int P[2], vec3_t result) {
    double s[2][3], u[3];
    int i;
    for(i = 0; i < 2; i++){
        s[i][0] = c[i] - a[i];
        s[i][1] = b[i] - a[i];
        s[i][2] = a[i] - P[i];
    }
    vec3_cross(s[0], s[1], u);
    if(fabs(u[2]) > 1e-2) {
        result[0] = 1 - (u[0]+u[1])/u[2];
        result[1] = u[1]/u[2];
        result[2] = u[0]/u[2];
    } else {
        result[0] = -1; result[1] = 1; result[2] = 1;
    }
}

static int basic_triangle(vec4_t vp0, vec4_t vp1, vec4_t vp2, vec2_t t0, vec2_t t1, vec2_t t2, vec3_t c0, vec3_t c1, vec3_t c2) {
    double v0[3], v1[3], v2[3];
    v0[0] = (vp0[0]/vp0[3] + 1.0) * (double)V_Width/2.0;
    v0[1] = (-vp0[1]/vp0[3] + 1.0) * (double)V_Height/2.0;
    v0[2] = vp0[2]/vp0[3];

    v1[0] = (vp1[0]/vp1[3] + 1.0) * (double)V_Width/2.0;
    v1[1] = (-vp1[1]/vp1[3] + 1.0) * (double)V_Height/2.0;
    v1[2] = vp1[2]/vp1[3];

    v2[0] = (vp2[0]/vp2[3] + 1.0) * (double)V_Width/2.0;
    v2[1] = (-vp2[1]/vp2[3] + 1.0) * (double)V_Height/2.0;
    v2[2] = vp2[2]/vp2[3];

    /* Backface culling: */
    if(!Backface) {
        double P[3], Q[3], N[3];;
        vec3_subtract(v1, v0, P);
        vec3_subtract(v2, v0, Q);
        vec3_cross(P, Q, N);

        if(N[2] >= 0) /* This is effectively `N . <0,0,1>` */
            return 0;
    }

    int xmin = (int)MIN(v0[0], MIN(v1[0], v2[0]));
    int xmax = (int)MAX(v0[0], MAX(v1[0], v2[0]));
    int ymin = (int)MIN(v0[1], MIN(v1[1], v2[1]));
    int ymax = (int)MAX(v0[1], MAX(v1[1], v2[1]));

    BmRect clip = bm_get_clip(Target);

    if(xmin < clip.x0) xmin = clip.x0;
    if(xmax >= clip.x1) xmax = clip.x1 - 1;
    if(ymin < clip.y0) ymin = clip.y0;
    if(ymax >= clip.y1) ymax = clip.y1 - 1;

    int lighting = (Lighting && NNorms == NVerts) || (NCols == NVerts);
    int texture = NTexs == NVerts && Texture;

    double texel[3];
    unsigned int trans_color;
    int tex_x, tex_y, tex_w, tex_h;
    double sween[4][2];
    if(texture) {
        clip = bm_get_clip(Texture);
        trans_color = bm_get_color(Texture) & 0x00FFFFFF;
        tex_x = clip.x0;
        tex_y = clip.y0;
        tex_w = clip.x1 - clip.x0;
        tex_h = clip.y1 - clip.y0;
        assert(tex_x >= 0 && tex_y >= 0);
        assert(tex_w > 0 && tex_h > 0);

        /*
        Tim Sweeny described this technique for dithering textures in screen space
        Unreal's software renderer to make it look like bilinear filtering.
        https://www.flipcode.com/archives/Texturing_As_In_Unreal.shtml
        */
        double sween_f = 0.25;
        if(TextureDither) {
            sween[0][0] = (sween_f*1) / tex_w; sween[0][1] = (sween_f*0) / tex_h;
            sween[1][0] = (sween_f*3) / tex_w; sween[1][1] = (sween_f*2) / tex_h;
            sween[2][0] = (sween_f*2) / tex_w; sween[2][1] = (sween_f*3) / tex_h;
            sween[3][0] = (sween_f*0) / tex_w; sween[3][1] = (sween_f*1) / tex_h;
        }
    } else {
        fx_ctorgb(bm_get_color(Target), &texel[0], &texel[1], &texel[2]);
        vec3_scale(texel, 255, NULL);
    }
    double default_rgb[] = {1,1,1};

    int P[2]; // x, y;
    for(P[1] = ymin; P[1]<=ymax; P[1]++) {
        for(P[0] = xmin; P[0]<=xmax; P[0]++) {

            double bc_screen[3], bc_clip[3];
            barycentric(v0, v1, v2, P, bc_screen);

            if(bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0)
                continue;

            bc_clip[0] = bc_screen[0] / vp0[3];
            bc_clip[1] = bc_screen[1] / vp1[3];
            bc_clip[2] = bc_screen[2] / vp2[3];

            vec3_scale(bc_clip, 1.0/(bc_clip[0] + bc_clip[1] + bc_clip[2]), NULL);

            double z = v0[2] * bc_clip[0] + v1[2] * bc_clip[1] + v2[2] * bc_clip[2];

            if(ZBUF(P[0],P[1]) > z) {
                double rgb[3];
                unsigned int color;

                if(texture) {
                    double u = t0[0] * bc_clip[0] + t1[0] * bc_clip[1] + t2[0] * bc_clip[2];
                    double v = t0[1] * bc_clip[0] + t1[1] * bc_clip[1] + t2[1] * bc_clip[2];

                    if(TextureDither) {
                        int si = ((P[0] & 1) << 1) + (P[1] & 1);
                        u += sween[si][0]; v += sween[si][1];
                    }

                    while(u >= 1.0) u -= 1.0;
                    while(u < 0.0) u += 1.0;
                    while(v >= 1.0) v -= 1.0;
                    while(v < 0.0) v += 1.0;

                    color = bm_get(Texture, u * tex_w + tex_x, v * tex_h + tex_y);
                    if(Transparent && (color & 0x00FFFFFF) == trans_color)
                        continue;

                    texel[0] = (double)((color >> 16) & 0xFF)/ 255.0;
                    texel[1] = (double)((color >> 8) & 0xFF) / 255.0;
                    texel[2] = (double)((color >> 0) & 0xFF) / 255.0;
                } else {
                    vec3_set(default_rgb, texel);
                }

                if(lighting) {
                    rgb[0] = c0[0] * bc_clip[0] + c1[0] * bc_clip[1] + c2[0] * bc_clip[2];
                    rgb[1] = c0[1] * bc_clip[0] + c1[1] * bc_clip[1] + c2[1] * bc_clip[2];
                    rgb[2] = c0[2] * bc_clip[0] + c1[2] * bc_clip[1] + c2[2] * bc_clip[2];
                    vec3_clamp01(rgb);
                } else {
                    vec3_set(default_rgb, rgb);
                }

                vec3_multiply(rgb, texel, NULL);

                // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glFog.xml
                if(Fog_Type) {
                    double fac = 0;
                    if(Fog_Type == FX_FOG_LINEAR) {
                        fac = (z - Fog_Near)/(Fog_Far - Fog_Near);
                    } else if(Fog_Type == FX_FOG_EXP) {
                        fac = 1 - exp(-Fog_Density * z);
                    } else if(Fog_Type == FX_FOG_EXP2) {
                        fac = 1 - exp(-Fog_Density * Fog_Density * z * z);
                    }
                    if(fac > 0) {
                        if(fac > 1) {
                            vec3_set(Fog_Color, rgb);
                        } else {
                            vec3_lerp(rgb, Fog_Color, fac, NULL);
                        }
                    }
                }

                color = bm_rgb(rgb[0] * 255.0, rgb[1] * 255.0, rgb[2] * 255.0);

                if(Blend) {
                    unsigned int color2 = bm_get(Target, P[0], P[1]);
                    color = ((color >> 1) & 0x007F7F7F) + ((color2 >> 1) & 0x007F7F7F);
                }

                // color = 255 * (1.0 - z);
                // color = bm_rgb(color,color,color);

                bm_set(Target, P[0], P[1], color);

                ZBUF(P[0],P[1]) = z;

                if(Pick)
                    bm_putpixel(Pick, P[0], P[1]);
            }
        }
    }

#if 0
    unsigned int csave = bm_get_color(Target);
    bm_set_color(Target, 0xFFFFFF);
    bm_line(Target, v1[0], v1[1], v0[0], v0[1]);
    bm_line(Target, v1[0], v1[1], v2[0], v2[1]);
    bm_line(Target, v0[0], v0[1], v2[0], v2[1]);
    bm_set_color(Target, csave);
#endif

    return 1;
}

static int inside_plane(vec4_t v, vec4_t P) {
    return v[0] * P[0] + v[1] * P[1] + v[2] * P[2] + v[3] * P[3] > 0;
}
static double intersect(const vec4_t v0, const vec4_t v1, const vec4_t P) {
    double d0 = v0[0] * P[0] + v0[1] * P[1] + v0[2] * P[2] + v0[3] * P[3];
    double d1 = v1[0] * P[0] + v1[1] * P[1] + v1[2] * P[2] + v1[3] * P[3];
    return d0 / (d0 - d1);
}

static double ClipPlanes[][4] = {
    {0, 0, 1, 0},   // NEAR
    {0, 0, -1, 1},  // FAR
    {1, 0, 0, 1},   // LEFT
    {-1, 0, 0, 1},  // RIGHT
    {0, 1, 0, 1},   // BOTTOM
    {0, -1, 0, 1},  // TOP
};

#define OUT_IN(N,M) vp[N] = v ## M; tp[N] = t ## M; cp[N] = c ## M;

static int clip_to_plane(vec4_t v0, vec4_t v1, vec4_t v2, vec2_t t0, vec2_t t1, vec2_t t2, vec3_t c0, vec3_t c1, vec3_t c2, int n) {
    vec4_t vp[3], P;
    double vq1[4], vq2[4], tq1[2], tq2[2], cq1[3], cq2[3];
    double i1, i2;
    vec2_t tp[3];
    vec3_t cp[3];

    if(n >= (sizeof ClipPlanes) / (sizeof ClipPlanes[0])) {
        return basic_triangle(v0, v1, v2, t0, t1, t2, c0, c1, c2);
    }
    P = ClipPlanes[n];

    int inside, in0=0, in1=0, in2=0;
    if(inside_plane(v0, P))
        in0 = 1;
    if(inside_plane(v1, P))
        in1 = 1;
    if(inside_plane(v2, P))
        in2 = 1;

    inside = in0 + in1 + in2;

    if(inside == 3) {
        return clip_to_plane(v0, v1, v2, t0, t1, t2, c0, c1, c2, n+1);
    } else if(inside == 2) {
        if(!in0) {
            OUT_IN(0, 0)
            OUT_IN(2, 1)
            OUT_IN(1, 2)
        } else if(!in1) {
            OUT_IN(0, 1)
            OUT_IN(2, 2)
            OUT_IN(1, 0)
        } else {
            OUT_IN(0, 2)
            OUT_IN(2, 0)
            OUT_IN(1, 1)
        }

        i1 = intersect(vp[1], vp[0], P);
        vec4_lerp(vp[1], vp[0], i1, vq1);
        vec2_lerp(tp[1], tp[0], i1, tq1);
        vec3_lerp(cp[1], cp[0], i1, cq1);

        i2 = intersect(vp[2], vp[0], P);
        vec4_lerp(vp[2], vp[0], i2, vq2);
        vec2_lerp(tp[2], tp[0], i2, tq2);
        vec3_lerp(cp[2], cp[0], i2, cq2);

        int tris = 0;
        tris += clip_to_plane(vp[1], vq1, vp[2], tp[1], tq1, tp[2], cp[1], cq1, cp[2], n+1);
        tris += clip_to_plane(vp[2], vq1, vq2, tp[2], tq1, tq2, cp[2], cq1, cq2, n+1);

        return tris;

    } else if(inside == 1) {
        if(in0) {
            OUT_IN(0, 0)
            OUT_IN(1, 1)
            OUT_IN(2, 2)
        } else if(in1) {
            OUT_IN(0, 1)
            OUT_IN(1, 2)
            OUT_IN(2, 0)
        } else {
            OUT_IN(0, 2)
            OUT_IN(1, 0)
            OUT_IN(2, 1)
        }

        i1 = intersect(vp[0], vp[1], P);
        vec4_lerp(vp[0], vp[1], i1, vq1);
        vec2_lerp(tp[0], tp[1], i1, tq1);
        vec3_lerp(cp[0], cp[1], i1, cq1);

        i2 = intersect(vp[0], vp[2], P);
        vec4_lerp(vp[0], vp[2], i2, vq2);
        vec2_lerp(tp[0], tp[2], i2, tq2);
        vec3_lerp(cp[0], cp[2], i2, cq2);

        return clip_to_plane(vp[0], vq1, vq2, tp[0], tq1, tq2, cp[0], cq1, cq2, n+1);

    } // else wholy outside plane
    return 0;
}

static void compute_lighting(const vec3_t n0, vec3_t out);

static int triangle(int v0i, int v1i, int v2i) {
    assert(v0i >= 0 && v0i < NVerts);
    assert(v1i >= 0 && v1i < NVerts);
    assert(v2i >= 0 && v2i < NVerts);

    double vcolors[3][3];
    vec3_t color[] = {vcolors[0], vcolors[1], vcolors[2]};
    if(Lighting) {

        compute_lighting(NArray[v0i], color[0]);
        compute_lighting(NArray[v1i], color[1]);
        compute_lighting(NArray[v2i], color[2]);

        if(NCols == NVerts) {
            vec3_add(color[0], CArray[v0i], NULL);
            vec3_add(color[1], CArray[v1i], NULL);
            vec3_add(color[2], CArray[v2i], NULL);
            vec3_clamp01(color[0]);
            vec3_clamp01(color[1]);
            vec3_clamp01(color[2]);
        }

    } else {
        /* I don't care at this point if NCols == NVerts;
        I just need color[] to point to valid addresses */
        color[0] = CArray[v0i];
        color[1] = CArray[v1i];
        color[2] = CArray[v2i];
    }

    return clip_to_plane(  VArray[v0i], VArray[v1i], VArray[v2i],
                    TArray[v0i], TArray[v1i], TArray[v2i],
                    color[0], color[1], color[2], 0);
}

/*
Here's a nice reference:
http://what-when-how.com/opengl-programming-guide/the-mathematics-of-lighting-opengl-programming/
*/
static void compute_lighting(const vec3_t n0, vec3_t out) {
    numeric_t ambient[3], diffuse[3];
    numeric_t n[3], m[3];

    mat4_multiplyVec3(M_NormalXform, n0, n);

    vec3_normalize(n, NULL);

    double intensity = vec3_dot(n, vec3_negate(DiffuseDirection, m));
    if(intensity < 0) intensity = 0;
    vec3_scale(DiffuseColor, intensity, diffuse);

    if(Material_Enabled) {
        vec3_multiply(Material_Ambient, AmbientColor, ambient);
        vec3_multiply(Material_Diffuse, diffuse, diffuse);
        vec3_add(ambient, diffuse, out);
        vec3_add(out, Material_Emissive, out);
    } else {
        vec3_add(diffuse, AmbientColor, out);
    }

    vec3_clamp01(out);
}

static void compute_transforms() {
    if(Xform_dirty) {
        /* Multiply the Model, View and Projection matrices to get the transformation matrix */

        mat4_multiply(M_View, M_Model, M_ModelView);
        mat4_multiply(M_Projection, M_ModelView, M_Xform);

        /* The matrix for transforming normals is the inverse transpose of the Model matrix
        http://www.songho.ca/opengl/gl_normaltransform.html
        https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html */
        mat4_transpose(mat4_inverse(M_Model, M_NormalXform), NULL);
        Xform_dirty = 0;
    }
}

void fx_begin(fx_mode mode) {
    assert(Target);
    compute_transforms();

    Mode = mode;
    NVerts = 0;
    NTexs = 0;
    NNorms = 0;
    NCols = 0;
    Begun = 1;
}

int fx_end() {
    int i, tris = 0;
    if(!Target)
        return 0;
    assert(Begun);
    switch(Mode) {
        case FX_TRIANGLES:
        for(i = 2; i < NVerts; i+= 3) {
            tris += triangle(i - 2, i - 1, i);
        }
        break;
        case FX_TRIANGLE_STRIP:
        for(i = 2; i < NVerts; i++) {
            if(i & 0x1)
                tris += triangle(i-2, i, i-1);
            else
                tris += triangle(i-2, i-1, i);
        }
        break;
        case FX_TRIANGLE_FAN:
        for(i = 2; i < NVerts; i++) {
            tris += triangle(i-1, i, 0);
        }
        break;
    }
    Begun = 0;
    return tris;
}

int fx_vertex(double x, double y, double z) {
    assert(NVerts < VARRAY_SIZE && "You need to increase VARRAY_SIZE");
    assert(Begun && "`fx_vertex()` must be called between `fx_begin()` and `fx_end()`");
    if(NVerts > VARRAY_SIZE || !Begun)
        return 0;
    vec4_t V = VArray[NVerts++];
    V[0] = x;
    V[1] = y;
    V[2] = z;
    V[3] = 1.0;
    mat4_multiplyVec4(M_Xform, V, V);
    return NVerts;
}

int fx_texcoord(double u, double v) {
    assert(NTexs < VARRAY_SIZE && "You need to increase VARRAY_SIZE");
    assert(Begun && "`fx_texcoord()` must be called between `fx_begin()` and `fx_end()`");
    if(NTexs > VARRAY_SIZE || !Begun)
        return 0;
    vec2_t T = TArray[NTexs++];
    T[0] = u;
    T[1] = v;
    return NTexs;
}

int fx_normal(double x, double y, double z) {
    assert(NNorms < VARRAY_SIZE && "You need to increase VARRAY_SIZE");
    assert(Begun && "`fx_normal()` must be called between `fx_begin()` and `fx_end()`");
    if(NNorms > VARRAY_SIZE || !Begun)
        return 0;
    vec3_t V = NArray[NNorms++];
    V[0] = x;
    V[1] = y;
    V[2] = z;
    return NNorms;
}

int fx_color(double r, double g, double b) {
    assert(NCols < VARRAY_SIZE && "You need to increase VARRAY_SIZE");
    assert(Begun && "`fx_color()` must be called between `fx_begin()` and `fx_end()`");
    if(NCols > VARRAY_SIZE || !Begun)
        return 0;
    vec3_t C = CArray[NCols++];
    C[0] = r;
    C[1] = g;
    C[2] = b;
    return NCols;
}

void fx_set_model(mat4_t m) {
    assert(!Begun && "Don't change the matrices between `fx_begin()` and `fx_end()`");
    if(!Begun) {
        mat4_set(m, M_Model);
        Xform_dirty = 1;
    }
}
void fx_save_model(mat4_t dest) {
    mat4_set(M_Model, dest);
}

void fx_set_view(mat4_t m) {
    assert(!Begun && "Don't change the matrices between `fx_begin()` and `fx_end()`");
    if(!Begun) {
        mat4_set(m, M_View);
        mat4_inverse(M_View, M_View_Inv);
        Xform_dirty = 1;
    }
}
void fx_save_view(mat4_t dest) {
    mat4_set(M_View, dest);
}

void fx_set_projection(mat4_t m) {
    assert(!Begun && "Don't change the matrices between `fx_begin()` and `fx_end()`");
    if(!Begun) {
        mat4_set(m, M_Projection);
        Xform_dirty = 1;
    }
}
void fx_save_projection(mat4_t dest) {
    mat4_set(M_Projection, dest);
}

void fx_set_texture(Bitmap *texture) {
    Texture = texture;
}

void fx_transparent(int enabled) {
    Transparent = enabled;
}

void fx_lighting(int enabled) {
    Lighting = enabled;
}

void fx_set_ambient(double r, double g, double b) {
    AmbientColor[0] = r;
    AmbientColor[1] = g;
    AmbientColor[2] = b;
    vec3_clamp01(AmbientColor);
}

void fx_set_diffuse_color(double r, double g, double b) {
    DiffuseColor[0] = r;
    DiffuseColor[1] = g;
    DiffuseColor[2] = b;
    vec3_clamp01(DiffuseColor);
}

void fx_set_diffuse_direction(double x, double y, double z) {
    DiffuseDirection[0] = x;
    DiffuseDirection[1] = y;
    DiffuseDirection[2] = z;
    vec3_normalize(DiffuseDirection, NULL);
}

/* Materials are inspired by OpenGL's.
Here are some links that may be useful
http://math.hws.edu/graphicsbook/c4/s2.html
http://what-when-how.com/opengl-programming-guide/defining-material-properties-lighting-opengl-programming/
*/
void fx_set_material(vec3_t ambient, vec3_t diffuse, vec3_t emissive) {
    vec3_set(ambient, Material_Ambient);
    vec3_set(diffuse, Material_Diffuse);
    vec3_set(emissive, Material_Emissive);
    Material_Enabled = 1;
}

void fx_reset_material() {
    Material_Enabled = 0;
}

void fx_backface(int enabled) {
    Backface = enabled;
}

void fx_fog(fg_fog_type type) {
    Fog_Type = type;
}

void fx_fog_params(double r, double g, double b, double near, double far, double density) {
    Fog_Color[0] = r;
    Fog_Color[1] = g;
    Fog_Color[2] = b;
    vec3_clamp01(Fog_Color);
    Fog_Near = near;
    Fog_Far = far;
    Fog_Density = density;
}

void fx_blend(int enabled) {
    Blend = enabled;
}

void fx_texture_dither(int enabled) {
    TextureDither = enabled;
}

void fx_set_pick(Bitmap *pick) {
    if(!pick) {
        Pick = NULL;
    } else {
        assert(Target);
        assert(bm_width(pick) >= bm_width(Target) && bm_height(pick) >= bm_height(Target));
        if(bm_width(pick) >= bm_width(Target) && bm_height(pick) >= bm_height(Target))
            Pick = pick;
    }
}

void fx_ctorgb(unsigned int c, double *r, double *g, double *b) {
    int ir = (c >> 16) & 0xFF;
    int ig = (c >>  8) & 0xFF;
    int ib = (c >>  0) & 0xFF;
    *r = (double)ir / 255.0;
    *g = (double)ig / 255.0;
    *b = (double)ib / 255.0;
}

void fx_set_target_color(unsigned int color) {
    bm_set_color(Target, color);
}

unsigned int fx_get_target_color() {
    return bm_get_color(Target);
}

/*
To compute lighting on the billboard we subtract the position
of the billboard from the camera position and normalize.
If you don't supply the camera position to the function, it will
use translation part of the inverse of the view matrix to get
the camera position, but this assumes that the view matrix is
not scaled or skewed (which is normally the case).
https://community.khronos.org/t/extracting-camera-position-from-a-modelview-matrix/68031
https://gamedev.stackexchange.com/questions/22283/how-to-get-translation-from-view-matrix
https://gamedev.stackexchange.com/a/138209/25926
*/

void fx_billboard(vec3_t pos, double scale, int flags) {
    double eye_pos[3];
    vec3_negate(vec3_set(&M_View_Inv[12], eye_pos), NULL);
    fx_billboard_eye(pos, eye_pos, scale, flags);
}

void fx_billboard_eye(vec3_t pos, vec3_t eye, double scale, int flags) {

    double model[16], modelview[16], save_model[16], save_view[16];

    fx_save_model(save_model);
    fx_save_view(save_view);

    mat4_identity(model);
    mat4_translate(model, pos, NULL);
    //double sv[] = {scale, scale, scale};
    //mat4_scale(model, sv, NULL);
    mat4_multiply(M_View, model, modelview);

    BmRect tclip = bm_get_clip(Texture);
    int tw = tclip.x1 - tclip.x0, th = tclip.y1 - tclip.y0;
    double scale_x = scale * tw / th, scale_y = scale;

    if(flags & BB_CYLINDRICAL) {
        modelview[0] = scale_x; modelview[1] = 0; modelview[2] = 0;
        modelview[4] *= scale_y;modelview[5] *= scale_y;modelview[6] *= scale_y;
        modelview[8] = 0; modelview[9] = 0; modelview[10] = scale;
    } else {
        modelview[0] = scale_x; modelview[1] = 0; modelview[2] = 0;
        modelview[4] = 0; modelview[5] = scale_y; modelview[6] = 0;
        modelview[8] = 0; modelview[9] = 0; modelview[10] = scale;
    }

    mat4_multiply(M_Projection, modelview, M_Xform);

    static double vc[][3] = { { -0.5,  0.5, 0 },
                                { -0.5, -0.5, 0 },
                                {  0.5,  0.5, 0 },
                                {  0.5, -0.5, 0 }};
    static double vb[][3] = { { -0.5,  1.0, 0 },
                                { -0.5,  0.0, 0 },
                                {  0.5,  1.0, 0 },
                                {  0.5,  0.0, 0 }};
    static double vt[][3] = { { -0.5,  0.0, 0 },
                                { -0.5, -1.0, 0 },
                                {  0.5,  0.0, 0 },
                                {  0.5,  -1.0, 0 }};

    double (*v)[3] = (flags & BB_ANCHOR_BOTTOM) ? vb : (flags & BB_ANCHOR_TOP) ? vt : vc;

    static double t[][2] = {{0,0},{0,1},{1,0},{1,1}};

    int save_backface = Backface;
    Backface = 0;

    double color[3] = {1.0, 1.0, 1.0};

    /* Disable lighting temporarily.
    We will calculate the lighting here, rather than
    defer it to `triangle()` because the normal vector
    works differently */
    int save_light = Lighting;
    Lighting = 0;
    if(save_light) {
        double n0[3], e[3];
        assert(eye);
        vec3_negate(eye, e);
        vec3_subtract(pos, e, n0);
        vec3_normalize(n0, NULL);

        double intensity = vec3_dot(n0, DiffuseDirection);
        if(intensity < 0) intensity = 0;
        vec3_scale(DiffuseColor, intensity, color);
        vec3_add(color, AmbientColor, NULL);
        vec3_clamp01(color);
    }

    Xform_dirty = 0; // Hack to prevent us from recomputing the transform
    fx_begin(FX_TRIANGLE_STRIP);
    fx_vertex_v3(v[0]); fx_texcoord(t[0][0], t[0][1]); fx_color_v3(color);
    fx_vertex_v3(v[1]); fx_texcoord(t[1][0], t[1][1]); fx_color_v3(color);
    fx_vertex_v3(v[2]); fx_texcoord(t[2][0], t[2][1]); fx_color_v3(color);
    fx_vertex_v3(v[3]); fx_texcoord(t[3][0], t[3][1]); fx_color_v3(color);
    fx_end();

    fx_set_model(save_model);
    fx_set_view(save_view);
    Backface = save_backface;
    Lighting = save_light;
}

static void transform_vertex(vec3_t in, vec4_t out) {
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = 1.0;
    mat4_multiplyVec4(M_Xform, out, out);
}

static void line_3d(vec4_t p0, vec4_t p1) {

    p0[0] = (p0[0]/p0[3] + 1) * bm_width(Target) / 2.0;
    p0[1] = (-p0[1]/p0[3] + 1) * bm_height(Target) / 2.0;
    p0[2] =  p0[2] / p0[3];
    p1[0] = (p1[0]/p1[3] + 1) * bm_width(Target) / 2.0;
    p1[1] = (-p1[1]/p1[3] + 1) * bm_height(Target) / 2.0;
    p1[2] =  p1[2] / p1[3];

    int x0 = p0[0];
    int y0 = p0[1];
    double z0 =  p0[2];
    int x1 = p1[0];
    int y1 = p1[1];
    double z1 =  p1[2];

    int dx = x1 - x0;
    int dy = y1 - y0;
    int sx, sy;
    int err, e2;

    if(dx < 0) dx = -dx;
    if(dy < 0) dy = -dy;

    if(x0 < x1)
        sx = 1;
    else
        sx = -1;
    if(y0 < y1)
        sy = 1;
    else
        sy = -1;

    err = dx - dy;

    double dzx, dzy;
    dzx = dx != 0 ? (z1 - z0)/dx : 0.0;
    dzy = dy != 0 ? (z1 - z0)/dy : 0.0;

    unsigned int color = bm_get_color(Target);
    BmRect clip = bm_get_clip(Target);

    int w = bm_width(Target);
    for(;;) {
        if(x0 >= clip.x0 && x0 < clip.x1 && y0 >= clip.y0 && y0 < clip.y1 && z0 >= 0 && z0 < 1.0) {
            /* DBL_EPSILON is there to give lines preference.
            Useful when using lines to draw triangle edges. */
            if(z0 < ZBuf[y0 * w + x0] + DBL_EPSILON) {
                ZBuf[y0 * w + x0] = z0;
                bm_set(Target, x0, y0, color);
            }
        }

        if(x0 == x1 && y0 == y1) break;

        e2 = 2 * err;

        if(e2 > -dy) {
            err -= dy;
            x0 += sx;
            z0 += dzx;
        }
        if(e2 < dx) {
            err += dx;
            y0 += sy;
            z0 += dzy;
        }
    }
}

static void clip_line_3d(vec4_t p0, vec4_t p1, int n) {
    if(n == (sizeof ClipPlanes/sizeof ClipPlanes[0])) {
       line_3d(p0, p1);
    } else {
        double *P = ClipPlanes[n];

        int i0 = inside_plane(p0, P);
        int i1 = inside_plane(p1, P);
        if(!i0) {
            if(!i1) {
                return;
            }
            double t = intersect(p0, p1, P);
            vec4_lerp(p0, p1, t, p0);
        } else if(!i1) {
            double t = intersect(p0, p1, P);
            vec4_lerp(p0, p1, t, p1);
        }
        clip_line_3d(p0, p1, n + 1);
    }
}

void fx_line(vec3_t p0, vec3_t p1) {
    double q0[4], q1[4];

    compute_transforms();
    transform_vertex(p0, q0);
    transform_vertex(p1, q1);

    clip_line_3d(q0, q1, 0);
}

void fx_line_d(numeric_t x0, numeric_t y0, numeric_t z0, numeric_t x1, numeric_t y1, numeric_t z1) {
	numeric_t p0[] = {x0, y0, z0};
	numeric_t p1[] = {x1, y1, z1};
    fx_line(p0, p1);
}

static void point_3d(vec4_t p0) {

    p0[0] = (p0[0]/p0[3] + 1) * bm_width(Target) / 2.0;
    p0[1] = (-p0[1]/p0[3] + 1) * bm_height(Target) / 2.0;
    p0[2] =  p0[2] / p0[3];

    int x0 = p0[0];
    int y0 = p0[1];
    double z0 =  p0[2];

    unsigned int color = bm_get_color(Target);
    BmRect clip = bm_get_clip(Target);

    if(x0 >= clip.x0 && x0 < clip.x1 && y0 >= clip.y0 && y0 < clip.y1 && z0 >= 0 && z0 < 1.0) {
        int w = bm_width(Target);
        /* DBL_EPSILON is there to give points preference. */
        if(z0 < ZBuf[y0 * w + x0] + DBL_EPSILON) {
            ZBuf[y0 * w + x0] = z0;
            bm_set(Target, x0, y0, color);
        }
    }
}

static void clip_point_3d(vec4_t p0, int n) {
    if(n == (sizeof ClipPlanes/sizeof ClipPlanes[0])) {
       point_3d(p0);
    } else {
        if(!inside_plane(p0, ClipPlanes[n]))
            return;
        clip_point_3d(p0, n + 1);
    }
}

void fx_point(vec3_t p0) {
    numeric_t q0[4];

    compute_transforms();
    transform_vertex(p0, q0);

    clip_point_3d(q0, 0);
}

void fx_point_d(numeric_t x0, numeric_t y0, numeric_t z0) {
    numeric_t p0[] = {x0, y0, z0};
    fx_point(p0);
}

void *fx_malloc(size_t size) {
    void *p = malloc(size);
    if(!p) {
        fx_error("FX: out of memory");
        abort();
    }
    return p;
}

void *fx_realloc(void* p, size_t size) {
    p = realloc(p, size);
    if(!p) {
        fx_error("FX: out of memory");
        abort();
    }
    return p;
}

void *fx_calloc(size_t nobj, size_t size) {
    void *p = calloc(nobj, size);
    if(!p) {
        fx_error("FX: out of memory");
        abort();
    }
    return p;
}

/* Default error handler just prints to `stderr` */
static void _fx_error(const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	fputs("error: ", stderr);
	vfprintf(stderr, fmt, arg);
	fputc('\n', stderr);
	va_end(arg);
    fflush(stderr);
}

/* Reads an entire file into a dynamically allocated memory buffer.
 * The returned buffer needs to be free()d afterwards
 */
static char *_fx_readfile(const char *fname) {
	FILE *f;
	long len,r;
	char *str;

	if(!(f = fopen(fname, "rb")))
		return NULL;

	fseek(f, 0, SEEK_END);
	len = ftell(f);
	rewind(f);

	str = fx_malloc(len+2);
	r = fread(str, 1, len, f);

	if(r != len) {
		free(str);
		return NULL;
	}

	fclose(f);
	str[len] = '\0';
	return str;
}

void (*fx_error)(const char *fmt, ...) = _fx_error;
char *(*fx_readfile)(const char *fname) = _fx_readfile;
