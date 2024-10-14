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

	int m;
} OBJ_FACE;

typedef struct {
	char *name;

	numeric_t Ka[3];
	numeric_t Kd[3];
	numeric_t Ks[3];
	numeric_t Ke[3];

	float Ns;
	float Ni;
	float d;
	int illum;

	char *map_Kd;
} OBJ_MTL;

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

	OBJ_DArray *materials;

} OBJ_MESH;

OBJ_MESH *obj_create();

void obj_free(OBJ_MESH *m);

OBJ_MESH *obj_load(const char *filename);

int obj_save(OBJ_MESH *m, const char *objfile, const char *mtlfile);

#ifndef OBJ_NODRAW
void obj_draw(OBJ_MESH *obj);
#endif

int obj_nfaces(OBJ_MESH *m);

/* gets the `n`th face of mesh `m` */
OBJ_FACE *obj_face(OBJ_MESH *m, int n);

int obj_new_face(OBJ_MESH *m);

int obj_face_add_vertex(OBJ_FACE *face, int v, int vt, int vn);

int obj_nverts(OBJ_MESH *m);

vec3_t obj_vert(OBJ_MESH *m, int n);

int obj_new_vert(OBJ_MESH *m, numeric_t x, numeric_t y, numeric_t z);

/* gets the vertex at <x,y,z>, creates it if it doesn't exist */
int obj_vert_at(OBJ_MESH *m, numeric_t x, numeric_t y, numeric_t z);

int obj_norms(OBJ_MESH *m);

vec3_t obj_norm(OBJ_MESH *m, int n);

int obj_new_norm(OBJ_MESH *m, numeric_t x, numeric_t y, numeric_t z);

int obj_norm_at(OBJ_MESH *m, numeric_t x, numeric_t y, numeric_t z);

int obj_ntexs(OBJ_MESH *m);

vec3_t obj_tex(OBJ_MESH *m, int n);

int obj_new_tex(OBJ_MESH *m, numeric_t u, numeric_t v);

int obj_tex_at(OBJ_MESH *m, numeric_t u, numeric_t v);

void obj_normalize_size(OBJ_MESH *obj);

OBJ_DArray *mtl_create();

OBJ_DArray *mtl_load(const char *filename, OBJ_DArray *materials);

void mtl_free(OBJ_DArray *materials);

int mtl_save(OBJ_DArray *materials, const char *filename);
