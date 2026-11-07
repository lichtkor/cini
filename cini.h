#ifndef CINI_H
#define CINI_H

#define MAX_ITEMS 100
#define MAX_LINE_LEN 256

/* Store the key and value pairs */
typedef struct {
    char key[MAX_LINE_LEN];
    char value[MAX_LINE_LEN];
} CiniItem;

/* .ini structure */
typedef struct {
    char filename[256];
    CiniItem items[MAX_ITEMS];
    int count;
} Cini;

int ciniLoad(Cini* cini, const char* filename);
int ciniGetInt(const Cini *cini, const char *key, int fallback);

#endif