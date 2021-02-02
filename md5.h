typedef struct {
    char *name;
    int parent;
    numeric_t position[3];
    numeric_t orientation[4];
} MD5_JOINT;

typedef struct {
    numeric_t st[2];
    int startWeight;
    int countWeight;
    numeric_t normal[3];
} MD5_VERT;

typedef struct {
    int vert[3];
} MD5_TRI;

typedef struct {
    int joint;
    numeric_t bias;
    numeric_t pos[3];
    numeric_t normal[3];
} MD5_WEIGHT;

typedef struct {
    char *shader;
    int numverts;
    MD5_VERT *verts;
    int numtris;
    MD5_TRI *tris;
    int numweights;
    MD5_WEIGHT *weights;
} MD5_MESH;

typedef struct MD5_MESH {
    int MD5Version;
    //char *commandline;
    int numJoints;
    MD5_JOINT *joints;

    int numMeshes;
    MD5_MESH *meshes;
} MD5_MODEL;


typedef struct {
    char *name;
    int parent;
    int flags;
    int startIndex;
} MD5_HIERARCHY;

typedef struct {
    numeric_t min[3];
    numeric_t max[3];
} MD5_BOUND;

typedef struct {
    numeric_t position[3];
    numeric_t orientation[4];
} MD5_FRAME_JOINT;

typedef struct {
    numeric_t *components;
} MD5_FRAME;

typedef struct MD5_ANIM {
    int MD5Version;
    //char *commandline;

    int numFrames;
    int numJoints;
    int frameRate;
    int numAnimatedComponents;

    MD5_HIERARCHY *hierarachy;
    MD5_BOUND *bounds;
    MD5_FRAME_JOINT *baseframe;

    MD5_FRAME *frames;

} MD5_ANIM;


MD5_MODEL *md5_load_mesh(const char *filename);

void md5_free(MD5_MODEL *m);

MD5_ANIM *md5_load_anim(const char *filename);

void md5_free_anim(MD5_ANIM *a);

void md5_draw(MD5_MODEL *m);

void md5_draw_frame(MD5_MODEL *m, MD5_ANIM *a, double frame);

#ifdef BMP_H
void md5_set_shader(const char *shader_name, Bitmap *texture);
#endif