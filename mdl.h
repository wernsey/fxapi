typedef float mdl_vec3[3];

#pragma pack(push, 1)
typedef struct mdl_header {
	int32_t ident;
	int32_t version;
	
	mdl_vec3 scale;
	mdl_vec3 translate;
	
	float boundingradius;
	mdl_vec3 eye_pos;
	
	int32_t num_skins;
	int32_t skinwidth;
	int32_t skinheight;
	
	int32_t num_verts;
	int32_t num_tris;
	int32_t num_frames;
	
	int32_t synctype;
	int32_t flags;
	float size;
	
} mdl_header;

typedef struct mdl_texcoord {
	int32_t onseam;
	int32_t s, t;
} mdl_texcoord;

typedef struct mdl_triangle {
	int32_t facesfront;
	int32_t vertex[3];
} mdl_triangle;

typedef struct mdl_vertex {
	uint8_t v[3];
	uint8_t normalIndex;
} mdl_vertex;

typedef struct mdl_simpleframe {
	mdl_vertex bboxmin;
	mdl_vertex bboxmax;
	char name[16];
	mdl_vertex *verts;
} mdl_simpleframe;
#pragma pack(pop)

typedef struct MDL_MESH {
	mdl_header header;
	
	int num_textures;
	float *times;
	Bitmap **textures;
	
	mdl_texcoord *tex_coords;
	mdl_triangle *triangles;
	mdl_simpleframe *frames;
} MDL_MESH;

MDL_MESH *mdl_load(const char *filename);

void mdl_free(MDL_MESH *m);

void mdl_draw(MDL_MESH *m, double frame);

void mdl_draw_interpolate(MDL_MESH *m, int frame0, int frame1, double frac);
