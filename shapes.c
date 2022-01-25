/*
 * Renders the meshes from Philip Rideout's shapes module
 * https://prideout.net/shapes
 * https://github.com/prideout/par/blob/master/par_shapes.h
 */

#include <stddef.h>
#define PAR_SHAPES_IMPLEMENTATION

#include "fx.h"
#include "shapes.h"

par_shapes_mesh *shape_swap_yz(par_shapes_mesh* mesh) {
    int i;
    // Swap the y and z coordinates of each point
    for(i = 0; i < mesh->npoints; i++) {
        float t;
        t = mesh->points[i * 3 + 1];
        mesh->points[i * 3 + 1] = mesh->points[i * 3 + 2];
        mesh->points[i * 3 + 2] = t;

        if(mesh->normals) {
            t = mesh->normals[i * 3 + 1];
            mesh->normals[i * 3 + 1] = mesh->normals[i * 3 + 2];
            mesh->normals[i * 3 + 2] = t;
        }
    }
    par_shapes_invert(mesh, 0, 0);
    return mesh;
}

void shape_draw(par_shapes_mesh* mesh) {
    int i, j, v;
    fx_begin(FX_TRIANGLES);
    for(i = 0; i < mesh->ntriangles; i++) {
        for(j = 0; j < 3; j++) {
            v = mesh->triangles[i * 3 + j];
            numeric_t vec[3];

            vec[0] = mesh->points[v * 3 + 0];
            vec[1] = mesh->points[v * 3 + 1];
            vec[2] = mesh->points[v * 3 + 2];
            fx_vertex_v3(vec);

            if(mesh->normals) {
                vec[0] = mesh->normals[v * 3 + 0];
                vec[1] = mesh->normals[v * 3 + 1];
                vec[2] = mesh->normals[v * 3 + 2];
                fx_normal_v3(vec);
            }

            if(mesh->tcoords) {
                vec[0] = mesh->tcoords[v * 2 + 0];
                vec[1] = mesh->tcoords[v * 2 + 1];
                fx_texcoord(vec[0],vec[1]);
            }
        }
    }
    fx_end();
}