typedef struct BmCache BmCache;

BmCache *bc_create();

void bc_destroy(BmCache *ht);

Bitmap *bc_put(BmCache *ht, const char *name, Bitmap *j);

Bitmap *bc_get(BmCache *ht, const char *name);