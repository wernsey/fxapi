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

MD5_MODEL *md5_load(const char *filename);

void md5_free(MD5_MODEL *m);

void md5_draw(MD5_MODEL *m, double frame);

#ifdef BMP_H
void md5_set_shader(const char *shader_name, Bitmap *texture);
#endif
