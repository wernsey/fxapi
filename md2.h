/*
 * http://en.wikipedia.org/wiki/MD2_(file_format)
 * http://tfc.duke.free.fr/old/models/md2.htm
 */
#pragma pack(push, 1)
typedef struct md2_header
{
	char ident[4];
	int version;
	int skin_w, skin_h;
	
	int framesize;  /* size of a single frame */
	
	int n_skins;    /* Number of skins */	
	int n_xyz;      /* Number of vertices */
	int n_st;		/* Number of texture coordinates */
	int n_tri;		/* Number of triangles */
	int n_glcmds;
	int n_frames;
	
	int ofs_skins;
	int ofs_st;
	int ofs_tris;
	int ofs_frames;
	int ofs_glcmds;
	int ofs_end;
} md2_header;

typedef char md2_skin_name[64];

/* Conversion from shorts to floats are done internally */
typedef struct md2_tex_coord {
	float s, t;
} md2_tex_coord;

typedef struct md2_tri {
	short vertexindex[3];
	short textureindex[3];
} md2_tri;

typedef struct md2_frame_tri {
	unsigned char v[3];
	unsigned char normal_i; /* normal index */
} md2_frame_tri;

typedef struct md2_frame_head {
	float scale[3], translate[3];
	char name[16];
} md2_frame_head;

typedef struct md2_frame {
	md2_frame_head head; /* header */
	md2_frame_tri *tris; /* individual triangle positions */	
} md2_frame;
#pragma pack(pop)

typedef struct MD2_MESH {
	md2_header header;	
	md2_skin_name *skin_names; /* May be NULL */
	
	md2_tex_coord *tex_coords;
		
	md2_tri *triangles;
	
	md2_frame *frames;
	
	int *glcmds;	
	
	float minx, maxx;
	float miny, maxy;
	float minz, maxz;
} MD2_MESH;

const char *md2_last_error();

MD2_MESH *md2_load(const char *filename);

void md2_free(MD2_MESH *m);

double *md2_get_normal(int i);

void md2_draw(MD2_MESH *m, double frame);

void md2_draw_interpolate(MD2_MESH *m, int frame0, int frame1, double frac);
