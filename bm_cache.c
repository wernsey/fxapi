#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"
#include "fx.h"
#include "bm_cache.h"

#define HASH_SIZE   8

struct HashElement {
    char *name;
    Bitmap *value;
    struct HashElement *next;
};

typedef struct HashElement HashElement;

struct BmCache {
    HashElement *table[HASH_SIZE];
};

BmCache *bc_create() {
    BmCache *ht = fx_calloc(1, sizeof *ht);
    return ht;
}

void bc_destroy(BmCache *ht) {
    int i;
    for(i = 0; i < HASH_SIZE; i++) {
        while(ht->table[i]) {
            HashElement* v = ht->table[i];
            ht->table[i] = v->next;
            free(v->name);
            bm_release(v->value);
            free(v);
        }
    }
    free(ht);
}

static unsigned int hash(const char *s) {
    /* DJB hash */
    unsigned int h = 5381;
    for(;s[0];s++)
        h = ((h << 5) + h) + s[0];
    return h;
}

Bitmap *bc_put(BmCache *ht, const char *name, Bitmap *b) {
    unsigned int h = hash(name) & (HASH_SIZE - 1);
    HashElement *v;
    for(v = ht->table[h]; v; v = v->next)
        if(!strcmp(v->name, name)) {
            bm_release(v->value);
            v->value = bm_retain(b);
            return b;
        }

    v = fx_malloc(sizeof *v);
    v->name = strdup(name);
    v->next = ht->table[h];
    v->value = bm_retain(b);
    ht->table[h] = v;

    return b;
}

Bitmap *bc_get(BmCache *ht, const char *name) {
    unsigned int h = hash(name) & (HASH_SIZE - 1);
    HashElement *v;
    for(v = ht->table[h]; v; v = v->next)
        if(!strcmp(v->name, name))
            return v->value;
    return NULL;
}

/*
static const char *ht_next(BmCache *ht, const char *name) {
    unsigned int h;
    HashElement *e;
    if(!name) {
        for(h = 0; h < HASH_SIZE; h++) {
            if((e = ht->table[h]))
                return e->name;
        }
    } else {
        h = hash(name) & (HASH_SIZE - 1);
        for(e = ht->table[h]; e; e = e->next) {
            if(!strcmp(e->name, name)) {
                if(e->next)
                    return e->next->name;
                for(h++; h < HASH_SIZE; h++) {
                    if(ht->table[h])
                        return ht->table[h]->name;
                }
            }
        }
    }
    return NULL;
}
*/
