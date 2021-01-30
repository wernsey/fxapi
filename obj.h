/**
 * obj.h
 * =====
 * Some utilities to work with 3D meshes in .OBJ files
 *
 * References:
 * * https://en.wikipedia.org/wiki/Wavefront_.obj_file
 * * http://paulbourke.net/dataformats/obj/
 * * http://paulbourke.net/dataformats/mtl/
 */

typedef struct OBJ_DArray OBJ_DArray;

typedef struct OBJ_FACE_VERTEX {
	int v, vt, vn;
} OBJ_FACE_VERTEX;

typedef struct OBJ_FACE {

	OBJ_FACE_VERTEX *fv;
	int n;
	int a;

    char *g;
    int s;
} OBJ_FACE;

typedef struct OBJ_MESH {

	OBJ_DArray *verts; /* Vertices */
	OBJ_DArray *norms; /* Normals */
	OBJ_DArray *texs;  /* Texture coordinates */

	OBJ_DArray *faces;  /* Faces */

    OBJ_DArray *groups;

	char * name;

	double xmin, xmax;
	double ymin, ymax;
	double zmin, zmax;

} OBJ_MESH;

OBJ_MESH *obj_create();

void obj_free(OBJ_MESH *m);

OBJ_MESH *obj_load(const char *filename);

int obj_save(OBJ_MESH *m, const char *filename);

#ifndef OBJ_NODRAW
void obj_draw(OBJ_MESH *obj);
#endif

int obj_nfaces(OBJ_MESH *m);

/* gets the `n`th face of mesh `m` */
OBJ_FACE *obj_face(OBJ_MESH *m, int n);

int obj_nverts(OBJ_MESH *m);
double *obj_vert(OBJ_MESH *m, int n);

int obj_norms(OBJ_MESH *m);
double *obj_norm(OBJ_MESH *m, int n);

int obj_ntexs(OBJ_MESH *m);
double *obj_tex(OBJ_MESH *m, int n);

void obj_normalize_size(OBJ_MESH *obj);
