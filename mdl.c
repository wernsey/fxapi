/*
 * http://tfc.duke.free.fr/coding/mdl-specs-en.html
 * https://six-of-one.github.io/quake-specifications/qkspec_5.htm#CMDLF
 *
 * TODO: It doesn't do much with the timing information in skin groups
 * TODO: It doesn't do much with the timing information in frame groups
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

#ifdef MDL_TEST
#  define MDL_NODRAW
#  ifndef MDL_VERBOSE
#    define MDL_VERBOSE 1
#  endif
#endif

#ifndef MDL_VERBOSE
#  define MDL_VERBOSE 0
#endif

#include "fx.h"

uint8_t mdl_quake_palette[] = {
    /* http://tfc.duke.free.fr/coding/src/colormap.h */
      0,   0,   0,  15,  15,  15,  31,  31,  31,  47,  47,  47,
     63,  63,  63,  75,  75,  75,  91,  91,  91, 107, 107, 107,
    123, 123, 123, 139, 139, 139, 155, 155, 155, 171, 171, 171,
    187, 187, 187, 203, 203, 203, 219, 219, 219, 235, 235, 235,
     15,  11,   7,  23,  15,  11,  31,  23,  11,  39,  27,  15,
     47,  35,  19,  55,  43,  23,  63,  47,  23,  75,  55,  27,
     83,  59,  27,  91,  67,  31,  99,  75,  31, 107,  83,  31,
    115,  87,  31, 123,  95,  35, 131, 103,  35, 143, 111,  35,
     11,  11,  15,  19,  19,  27,  27,  27,  39,  39,  39,  51,
     47,  47,  63,  55,  55,  75,  63,  63,  87,  71,  71, 103,
     79,  79, 115,  91,  91, 127,  99,  99, 139, 107, 107, 151,
    115, 115, 163, 123, 123, 175, 131, 131, 187, 139, 139, 203,
      0,   0,   0,   7,   7,   0,  11,  11,   0,  19,  19,   0,
     27,  27,   0,  35,  35,   0,  43,  43,   7,  47,  47,   7,
     55,  55,   7,  63,  63,   7,  71,  71,   7,  75,  75,  11,
     83,  83,  11,  91,  91,  11,  99,  99,  11, 107, 107,  15,
      7,   0,   0,  15,   0,   0,  23,   0,   0,  31,   0,   0,
     39,   0,   0,  47,   0,   0,  55,   0,   0,  63,   0,   0,
     71,   0,   0,  79,   0,   0,  87,   0,   0,  95,   0,   0,
    103,   0,   0, 111,   0,   0, 119,   0,   0, 127,   0,   0,
     19,  19,   0,  27,  27,   0,  35,  35,   0,  47,  43,   0,
     55,  47,   0,  67,  55,   0,  75,  59,   7,  87,  67,   7,
     95,  71,   7, 107,  75,  11, 119,  83,  15, 131,  87,  19,
    139,  91,  19, 151,  95,  27, 163,  99,  31, 175, 103,  35,
     35,  19,   7,  47,  23,  11,  59,  31,  15,  75,  35,  19,
     87,  43,  23,  99,  47,  31, 115,  55,  35, 127,  59,  43,
    143,  67,  51, 159,  79,  51, 175,  99,  47, 191, 119,  47,
    207, 143,  43, 223, 171,  39, 239, 203,  31, 255, 243,  27,
     11,   7,   0,  27,  19,   0,  43,  35,  15,  55,  43,  19,
     71,  51,  27,  83,  55,  35,  99,  63,  43, 111,  71,  51,
    127,  83,  63, 139,  95,  71, 155, 107,  83, 167, 123,  95,
    183, 135, 107, 195, 147, 123, 211, 163, 139, 227, 179, 151,
    171, 139, 163, 159, 127, 151, 147, 115, 135, 139, 103, 123,
    127,  91, 111, 119,  83,  99, 107,  75,  87,  95,  63,  75,
     87,  55,  67,  75,  47,  55,  67,  39,  47,  55,  31,  35,
     43,  23,  27,  35,  19,  19,  23,  11,  11,  15,   7,   7,
    187, 115, 159, 175, 107, 143, 163,  95, 131, 151,  87, 119,
    139,  79, 107, 127,  75,  95, 115,  67,  83, 107,  59,  75,
     95,  51,  63,  83,  43,  55,  71,  35,  43,  59,  31,  35,
     47,  23,  27,  35,  19,  19,  23,  11,  11,  15,   7,   7,
    219, 195, 187, 203, 179, 167, 191, 163, 155, 175, 151, 139,
    163, 135, 123, 151, 123, 111, 135, 111,  95, 123,  99,  83,
    107,  87,  71,  95,  75,  59,  83,  63,  51,  67,  51,  39,
     55,  43,  31,  39,  31,  23,  27,  19,  15,  15,  11,   7,
    111, 131, 123, 103, 123, 111,  95, 115, 103,  87, 107,  95,
     79,  99,  87,  71,  91,  79,  63,  83,  71,  55,  75,  63,
     47,  67,  55,  43,  59,  47,  35,  51,  39,  31,  43,  31,
     23,  35,  23,  15,  27,  19,  11,  19,  11,   7,  11,   7,
    255, 243,  27, 239, 223,  23, 219, 203,  19, 203, 183,  15,
    187, 167,  15, 171, 151,  11, 155, 131,   7, 139, 115,   7,
    123,  99,   7, 107,  83,   0,  91,  71,   0,  75,  55,   0,
     59,  43,   0,  43,  31,   0,  27,  15,   0,  11,   7,   0,
      0,   0, 255,  11,  11, 239,  19,  19, 223,  27,  27, 207,
     35,  35, 191,  43,  43, 175,  47,  47, 159,  47,  47, 143,
     47,  47, 127,  47,  47, 111,  47,  47,  95,  43,  43,  79,
     35,  35,  63,  27,  27,  47,  19,  19,  31,  11,  11,  15,
     43,   0,   0,  59,   0,   0,  75,   7,   0,  95,   7,   0,
    111,  15,   0, 127,  23,   7, 147,  31,   7, 163,  39,  11,
    183,  51,  15, 195,  75,  27, 207,  99,  43, 219, 127,  59,
    227, 151,  79, 231, 171,  95, 239, 191, 119, 247, 211, 139,
    167, 123,  59, 183, 155,  55, 199, 195,  55, 231, 227,  87,
    127, 191, 255, 171, 231, 255, 215, 255, 255, 103,   0,   0,
    139,   0,   0, 179,   0,   0, 215,   0,   0, 255,   0,   0,
    255, 243, 147, 255, 247, 199, 255, 255, 255, 159,  91,  83,
};
uint8_t mdl_hexen2_palette[] = {
    /* https://sourceforge.net/p/uhexen2/uhexen2/ci/master/tree/utils/texutils/palette/hexen2pal.h */
    /* Hexen II palette: data read from palette.lmp which is available in the Hexen II demo
    pak0.pak. Palette.lmp file of Hexen II can also be found at several websites such as
    www.raven-games.com. It is Copyright (C) Raven Software. */
    0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x08,0x10,0x10,0x10,0x18,0x18,0x18,0x20,
    0x20,0x20,0x28,0x28,0x28,0x30,0x30,0x30,0x38,0x38,0x38,0x40,0x40,0x40,0x48,0x48,
    0x48,0x50,0x50,0x50,0x54,0x54,0x54,0x58,0x58,0x58,0x60,0x60,0x60,0x68,0x68,0x68,
    0x70,0x70,0x70,0x78,0x78,0x78,0x80,0x80,0x80,0x88,0x88,0x88,0x94,0x94,0x94,0x9C,
    0x9C,0x9C,0xA8,0xA8,0xA8,0xB4,0xB4,0xB4,0xB8,0xB8,0xB8,0xC4,0xC4,0xC4,0xCC,0xCC,
    0xCC,0xD4,0xD4,0xD4,0xE0,0xE0,0xE0,0xE8,0xE8,0xE8,0xF0,0xF0,0xF0,0xFC,0xFC,0xFC,
    0x08,0x08,0x0C,0x10,0x10,0x14,0x18,0x18,0x1C,0x1C,0x20,0x24,0x24,0x24,0x2C,0x2C,
    0x2C,0x34,0x30,0x34,0x3C,0x38,0x38,0x44,0x40,0x40,0x48,0x4C,0x4C,0x58,0x5C,0x5C,
    0x68,0x6C,0x70,0x80,0x80,0x84,0x98,0x98,0x9C,0xB0,0xA8,0xAC,0xC4,0xBC,0xC4,0xDC,
    0x20,0x18,0x14,0x28,0x20,0x1C,0x30,0x24,0x20,0x34,0x2C,0x28,0x3C,0x34,0x2C,0x44,
    0x38,0x34,0x4C,0x40,0x38,0x54,0x48,0x40,0x5C,0x4C,0x48,0x64,0x54,0x4C,0x6C,0x5C,
    0x54,0x70,0x60,0x58,0x78,0x68,0x60,0x80,0x70,0x64,0x88,0x74,0x6C,0x90,0x7C,0x70,
    0x14,0x18,0x14,0x1C,0x20,0x1C,0x20,0x24,0x20,0x28,0x2C,0x28,0x2C,0x30,0x2C,0x30,
    0x38,0x30,0x38,0x40,0x38,0x40,0x44,0x40,0x44,0x4C,0x44,0x54,0x5C,0x54,0x68,0x70,
    0x68,0x78,0x80,0x78,0x8C,0x94,0x88,0x9C,0xA4,0x98,0xAC,0xB4,0xA8,0xBC,0xC4,0xB8,
    0x30,0x20,0x08,0x3C,0x28,0x08,0x48,0x30,0x10,0x54,0x38,0x14,0x5C,0x40,0x1C,0x64,
    0x48,0x24,0x6C,0x50,0x2C,0x78,0x5C,0x34,0x88,0x68,0x3C,0x94,0x74,0x48,0xA0,0x80,
    0x54,0xA8,0x88,0x5C,0xB4,0x90,0x64,0xBC,0x98,0x6C,0xC4,0xA0,0x74,0xCC,0xA8,0x7C,
    0x10,0x14,0x10,0x14,0x1C,0x14,0x18,0x20,0x18,0x1C,0x24,0x1C,0x20,0x2C,0x20,0x24,
    0x30,0x24,0x28,0x38,0x28,0x2C,0x3C,0x2C,0x30,0x44,0x30,0x34,0x4C,0x34,0x3C,0x54,
    0x3C,0x44,0x5C,0x40,0x4C,0x64,0x48,0x54,0x6C,0x4C,0x5C,0x74,0x54,0x64,0x80,0x5C,
    0x18,0x0C,0x08,0x20,0x10,0x08,0x28,0x14,0x08,0x34,0x18,0x0C,0x3C,0x1C,0x0C,0x44,
    0x20,0x0C,0x4C,0x24,0x10,0x54,0x2C,0x14,0x5C,0x30,0x18,0x64,0x38,0x1C,0x70,0x40,
    0x20,0x78,0x48,0x24,0x80,0x50,0x2C,0x90,0x5C,0x38,0xA8,0x70,0x48,0xC0,0x84,0x58,
    0x18,0x04,0x04,0x24,0x04,0x04,0x30,0x00,0x00,0x3C,0x00,0x00,0x44,0x00,0x00,0x50,
    0x00,0x00,0x58,0x00,0x00,0x64,0x00,0x00,0x70,0x00,0x00,0x84,0x00,0x00,0x98,0x00,
    0x00,0xAC,0x00,0x00,0xC0,0x00,0x00,0xD4,0x00,0x00,0xE8,0x00,0x00,0xFC,0x00,0x00,
    0x10,0x0C,0x20,0x1C,0x14,0x30,0x20,0x1C,0x38,0x28,0x24,0x44,0x34,0x2C,0x50,0x3C,
    0x38,0x5C,0x44,0x40,0x68,0x50,0x48,0x74,0x58,0x54,0x80,0x64,0x60,0x8C,0x6C,0x6C,
    0x98,0x78,0x74,0xA4,0x84,0x84,0xB0,0x90,0x90,0xBC,0x9C,0x9C,0xC8,0xAC,0xAC,0xD4,
    0x24,0x14,0x04,0x34,0x18,0x04,0x44,0x20,0x04,0x50,0x28,0x00,0x64,0x30,0x04,0x7C,
    0x3C,0x04,0x8C,0x48,0x04,0x9C,0x58,0x08,0xAC,0x64,0x08,0xBC,0x74,0x0C,0xCC,0x80,
    0x0C,0xDC,0x90,0x10,0xEC,0xA0,0x14,0xFC,0xB8,0x38,0xF8,0xC8,0x50,0xF8,0xDC,0x78,
    0x14,0x10,0x04,0x1C,0x18,0x08,0x24,0x20,0x08,0x2C,0x28,0x0C,0x34,0x30,0x10,0x38,
    0x38,0x10,0x40,0x40,0x14,0x44,0x48,0x18,0x48,0x50,0x1C,0x50,0x5C,0x20,0x54,0x68,
    0x28,0x58,0x74,0x2C,0x5C,0x80,0x34,0x5C,0x8C,0x34,0x5C,0x94,0x38,0x60,0xA0,0x40,
    0x3C,0x10,0x10,0x48,0x18,0x18,0x54,0x1C,0x1C,0x64,0x24,0x24,0x70,0x2C,0x2C,0x7C,
    0x34,0x30,0x8C,0x40,0x38,0x98,0x4C,0x40,0x2C,0x14,0x08,0x38,0x1C,0x0C,0x48,0x20,
    0x10,0x54,0x28,0x14,0x60,0x2C,0x1C,0x70,0x34,0x20,0x7C,0x38,0x28,0x8C,0x40,0x30,
    0x18,0x14,0x10,0x24,0x1C,0x14,0x2C,0x24,0x1C,0x38,0x2C,0x20,0x40,0x34,0x24,0x48,
    0x3C,0x2C,0x50,0x44,0x30,0x5C,0x4C,0x34,0x64,0x54,0x3C,0x70,0x5C,0x44,0x78,0x64,
    0x48,0x84,0x70,0x50,0x90,0x78,0x58,0x98,0x80,0x60,0xA0,0x88,0x68,0xA8,0x94,0x70,
    0x24,0x18,0x0C,0x2C,0x20,0x10,0x34,0x28,0x14,0x3C,0x2C,0x14,0x48,0x34,0x18,0x50,
    0x3C,0x1C,0x58,0x44,0x1C,0x68,0x4C,0x20,0x94,0x60,0x38,0xA0,0x6C,0x40,0xAC,0x74,
    0x48,0xB4,0x7C,0x50,0xC0,0x84,0x58,0xCC,0x8C,0x5C,0xD8,0x9C,0x6C,0x3C,0x14,0x5C,
    0x64,0x24,0x74,0xA8,0x48,0xA4,0xCC,0x6C,0xC0,0x04,0x54,0x04,0x04,0x84,0x04,0x00,
    0xB4,0x00,0x00,0xD8,0x00,0x04,0x04,0x90,0x10,0x44,0xCC,0x24,0x84,0xE0,0x58,0xA8,
    0xE8,0xD8,0x04,0x04,0xF4,0x48,0x00,0xFC,0x80,0x00,0xFC,0xAC,0x18,0xFC,0xFC,0xFC
};

static uint8_t *palette = mdl_quake_palette;

void mdl_set_palette(uint8_t *pal) {
    palette = pal;
}

static Bitmap *read_texture(FILE *f, int w, int h) {
    int x, y, i = 0;
    Bitmap *bmp = bm_create(w, h);
    uint8_t *bytes;
    if(!bmp)
        return NULL;
    bytes = fx_calloc(h, w);
    if(fread(bytes, w*h, 1, f) != 1) {
        fx_error("MDL: couldn't read texture\n");
        goto error;
    }
    for(y = 0; y < h; y++) {
        for(x = 0; x < w; x++) {
            uint8_t *rgb = palette + bytes[i++] * 3;
            unsigned int color = bm_rgb(rgb[0], rgb[1], rgb[2]);
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

#if MDL_VERBOSE
static void print_vec(const char *name, mdl_vec3 v) {
    if(name)
        printf("%s: <%g, %g, %g>\n", name, v[0], v[1], v[2]);
    else
        printf("<%g, %g, %g>\n", v[0], v[1], v[2]);
}

static void print_vert(struct mdl_vertex *vert) {
    printf(" * <%u %u %u>  [%u]\n", vert->v[0], vert->v[0], vert->v[0], vert->normalIndex);
}
#endif

static void mdl_postprocess(MDL_MESH *m);

static mdl_simpleframe *read_simple_frame(MDL_MESH *M, mdl_simpleframe *sframe, FILE *f) {
    sframe->verts = fx_calloc(M->header.num_verts, sizeof *sframe->verts);
    if(!sframe->verts) {
        fx_error("MDL: out of memory\n");
        return NULL;
    }

    if(fread(&sframe->bboxmin, sizeof sframe->bboxmin, 1, f) != 1 ||
        fread(&sframe->bboxmin, sizeof sframe->bboxmax, 1, f) != 1 ||
        fread(sframe->name, 16, 1, f) != 1) {
        fx_error("MDL: couldn't read simpleframe data: %s", strerror(errno));
        return NULL;
    }
    if(fread(sframe->verts, sizeof *sframe->verts, M->header.num_verts, f) != M->header.num_verts) {
        fx_error("MDL: couldn't read simpleframe vertices: %s", strerror(errno));
        return NULL;
    }
#if MDL_VERBOSE
    printf("SIMPLE FRAME: %s\n", sframe->name);
    print_vert(&sframe->bboxmin);
    print_vert(&sframe->bboxmax);
#endif
    return sframe;
}

MDL_MESH *mdl_load(const char *filename) {
    int i, j;
    MDL_MESH *M = fx_calloc(1, sizeof *M);
    if(!M) {
        fx_error("MDL: out of memory\n");
        return NULL;
    }

    FILE *f = fopen(filename, "rb");
    if(!f) {
        fx_error("MDL: Couldn't open %s: %s\n", filename, strerror(errno));
        return NULL;
    }

    if(fread(&M->header, sizeof M->header, 1, f) != 1) {
        fx_error("MDL: couldn't read header: %s\n", strerror(errno));
        return NULL;
    }

    if(memcmp(&M->header.ident, "IDPO", 4)) {
        fx_error("MDL: bad ident\n");
        return NULL;
    }

#if MDL_VERBOSE
    printf("ident: %08X; version: %d\n", M->header.ident, M->header.version);

    print_vec("scale", M->header.scale);
    print_vec("translate", M->header.translate);
    printf("boundingradius: %g\n", M->header.boundingradius);
    print_vec("eye_pos", M->header.eye_pos);

    printf("num_skins: %d; dimensions: %dx%d\n", M->header.num_skins, M->header.skinwidth, M->header.skinheight);

    printf("verts: %d; tris: %d; frames: %d\n", M->header.num_verts, M->header.num_tris, M->header.num_frames);

    printf("sync: %d; flags: %08X; size: %g\n", M->header.synctype, M->header.flags, M->header.size);
#endif

    M->skins = fx_calloc(M->header.num_skins, sizeof *M->skins);
    if(!M->skins) {
        fx_error("MDL: out of memory\n");
        return NULL;
    }

    for(j = 0; j < M->header.num_skins; j++) {
        mdl_skin *skin = &M->skins[j];

#if MDL_VERBOSE
        printf("Reading skin %d...\n", j);
#endif

        int32_t group;
        if(fread(&group, sizeof group, 1, f) != 1) {
            fx_error("MDL: couldn't read texture group: %s\n", strerror(errno));
            return NULL;
        }

        if(group) {
            if(fread(&skin->num_textures, sizeof skin->num_textures, 1, f) != 1) {
                fx_error("MDL: couldn't read texture count: %s\n", strerror(errno));
                return NULL;
            }
        } else {
            skin->num_textures = 1;
        }

#if MDL_VERBOSE
        printf("Number of textures: %d\n", skin->num_textures);
#endif

        skin->times = fx_calloc(skin->num_textures, sizeof *skin->times);
        if(!skin->times) {
            fx_error("MDL: out of memory\n");
            return NULL;
        }
        skin->textures = fx_calloc(skin->num_textures, sizeof *skin->textures);
        if(!skin->textures) {
            fx_error("MDL: out of memory\n");
            return NULL;
        }

        if(group) {
            if(fread(skin->times, sizeof *skin->times, skin->num_textures, f) != (unsigned)skin->num_textures) {
                fx_error("MDL: unable to read times: %s\n", strerror(errno));
                return NULL;
            }
            for(i = 0; i < skin->num_textures; i++) {
                skin->textures[i] = read_texture(f, M->header.skinwidth, M->header.skinheight);
                if(!skin->textures[i])
                    return NULL;
            }
        } else {
            skin->times[0] = 0;
            skin->textures[0] = read_texture(f, M->header.skinwidth, M->header.skinheight);
            if(!skin->textures[0])
                return NULL;
        }

#if MDL_VERBOSE
        for(i = 0; i < skin->num_textures; i++) {
            bm_savef(skin->textures[i], "skin-%d-%d.gif", j, i);
            printf("Skin %d-%d. Time: %g\n", j, i, skin->times[i]);
        }
#endif
    }

    M->tex_coords = fx_calloc(M->header.num_verts, sizeof *M->tex_coords);
    if(!M->tex_coords) {
        fx_error("MDL: out of memory\n");
        return NULL;
    }

#if MDL_VERBOSE
    printf("Texture Coordinates at %lX\n", ftell(f));
#endif

    if(fread(M->tex_coords, sizeof *M->tex_coords, M->header.num_verts, f) != M->header.num_verts) {
        fx_error("MDL: unable to read texture coordinates: %s\n", strerror(errno));
        return NULL;
    }

#if MDL_VERBOSE
    printf("Texture coordinates:\n");
    for(i = 0; i < M->header.num_verts; i++) {
        printf(" * %3d (%d, %d) %c\n", i, M->tex_coords[i].s, M->tex_coords[i].t, M->tex_coords[i].onseam ? '*' : ' ');
    }
#endif

    M->triangles = fx_calloc(M->header.num_tris, sizeof *M->triangles);
    if(!M->triangles) {
        fx_error("MDL: out of memory\n");
        return NULL;
    }

#if MDL_VERBOSE
    printf("Triangles at %lX\n", ftell(f));
#endif

    if(fread(M->triangles, sizeof *M->triangles, M->header.num_tris, f) != M->header.num_tris) {
        fx_error("MDL: unable to read triangles: %s\n", strerror(errno));
        return NULL;
    }
#if MDL_VERBOSE
    printf("Triangles:\n");
    for(i = 0; i < M->header.num_tris; i++) {
        printf(" * %3d (%d, %d, %d) %c\n", i, M->triangles[i].vertex[0], M->triangles[i].vertex[1], M->triangles[i].vertex[2], M->triangles[i].facesfront ? '*' : ' ');
    }
#endif

    M->frames = fx_calloc(M->header.num_frames, sizeof *M->frames);
    if(!M->frames) {
        fx_error("MDL: out of memory\n");
        return NULL;
    }

    for(i = 0; i < M->header.num_frames; i++) {

        mdl_frame *frame = &M->frames[i];
        if(fread(&frame->type, sizeof frame->type, 1, f) != 1) {
            fx_error("MDL: couldn't read frame type: %s\n", strerror(errno));
            return NULL;
        }

        if(frame->type != 0) {

            /* Some of the documentation I referenced missed this: */
            if(fread(&frame->num, sizeof frame->num, 1, f) != 1) {
                fx_error("MDL: couldn't read frame num: %s\n", strerror(errno));
                return NULL;
            }

            frame->times = fx_calloc(frame->num, sizeof *frame->times);
            frame->frames = fx_calloc(frame->num, sizeof *frame->frames);
            if(!frame->times || !frame->frames) {
                fx_error("MDL: out of memory\n");
                return NULL;
            }

            if(fread(&frame->bboxmin, sizeof frame->bboxmin, 1, f) != 1 ||
                fread(&frame->bboxmin, sizeof frame->bboxmax, 1, f) != 1) {
                fx_error("MDL: couldn't read frame data: %s", strerror(errno));
                return NULL;
            }

#if MDL_VERBOSE
            printf("FRAME GROUP %d: %d frames\n", i, frame->num);
            print_vert(&frame->bboxmin);
            print_vert(&frame->bboxmax);
#endif

            if(fread(frame->times, sizeof *frame->times, frame->num, f) != (unsigned)frame->num) {
                fx_error("MDL: unable to read frame times: %s\n", strerror(errno));
                return NULL;
            }

#if MDL_VERBOSE
            for(j = 0; j < frame->num; j++)
                printf("Frame time %d: %g\n", j, frame->times[j]);
#endif

            for(j = 0; j < frame->num; j++) {
                if(!read_simple_frame(M, &frame->frames[j], f))
                    return NULL;
            }

        } else {
            frame->num = 1;
            frame->times = fx_calloc(1, sizeof *frame->times);
            frame->frames = fx_calloc(1, sizeof *frame->frames);
            if(!frame->times || !frame->frames) {
                fx_error("MDL: out of memory\n");
                return NULL;
            }

            frame->times[0] = 0;
            if(!read_simple_frame(M, &frame->frames[0], f))
                return NULL;
        }

    }

    fclose(f);

    mdl_postprocess(M);

    return M;
}


/* TODO: Maybe it would be better to scale the model by a specific frame, rather than all frames.
    Otherwise a model gets scaled too much (like the Quake Shambler holds its hands over its head
    in one frame, which means it gets scaled according to that frame so it ends up smaller than
    it should be)
 */
static void mdl_postprocess(MDL_MESH *m) {
    int i, j, k, n, x;

    double mins[3] = {
        m->header.scale[0] * 255 + m->header.translate[0],
        m->header.scale[1] * 255 + m->header.translate[1],
        m->header.scale[2] * 255 + m->header.translate[2]};
    double maxs[3] = {
        m->header.translate[0],
        m->header.translate[1],
        m->header.translate[2]};

    m->total_frames = 0;
    for(n = 0; n < m->header.num_frames; n++) {
        m->total_frames += m->frames[n].num;
    }


#if MDL_VERBOSE
    printf("total frames: %d\n", m->total_frames);
#endif
    m->sframes = fx_calloc(m->total_frames, sizeof *m->sframes);
    m->times = fx_calloc(m->total_frames, sizeof *m->times);

    x = 0;
    for(n = 0; n < m->header.num_frames; n++) {
        mdl_frame *frame = &m->frames[n];
        for(k = 0; k < frame->num; k++) {
            mdl_simpleframe *sframe = &frame->frames[k];
            m->sframes[x] = sframe;
            m->times[x] = frame->times[k];
            x++;
            for(i = 0; i < m->header.num_tris; i++) {
                for(j = 0; j < 3; j++) {
                    int index = m->triangles[i].vertex[j];
                    mdl_vertex *vert = &sframe->verts[index];

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
    }

#if 0
    /* Normalize over what's the largest dimension */
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
    /* Normalize on the Y-dimension */
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
    int i, j;
    for(i = 0; i < m->header.num_skins; i++) {
        mdl_skin *skin = &m->skins[i];
        for(j = 0; j < skin->num_textures; j++)
            bm_free(skin->textures[j]);
        free(skin->times);
    }
    free(m->skins);
    for(i = 0; i < m->header.num_frames; i++) {
        mdl_frame *frame = &m->frames[i];
        int num = frame->type ? frame->type : 1;
        for(j = 0; j < num; j++)
            free(frame->frames[j].verts);
        free(frame->times);
        free(frame->frames);
    }
    free(m->tex_coords);
    free(m->triangles);
    free(m->frames);
    free(m->sframes);
    free(m->times);
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
    mdl_simpleframe *fr0 = m->sframes[frame0];
    mdl_simpleframe *fr1 = m->sframes[frame1];

    // TODO: Choose different skins and textures...
    fx_set_texture(m->skins[0].textures[0]);
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
    assert(frame >= 0 && frame < m->total_frames);
    int f0 = (int)frame;
    int f1 = (int)frame + 1;
    if(f1 >= m->header.num_frames) f1 = 0;
    mdl_draw_interpolate(m, f0, f1, frame - f0);
}

#endif

#ifdef MDL_TEST
/*
Compile like so:
$ gcc -g -O0 -DMDL_TEST ../mdl.c ../bmp.c ../fx.c
*/
int main(int argc, char *argv[]) {
    const char *filename = "SHAMBLER.MDL";
    if(argc > 1)
        filename = argv[1];

    MDL_MESH *m = mdl_load(filename);
    if(!m) {

        return 1;
    }

    mdl_free(m);

    printf("Done.\n");
    return 0;
}
#endif
