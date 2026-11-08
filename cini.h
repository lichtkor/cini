#ifndef CINI_H
#define CINI_H

#define MAX_ITEMS 100
#define MAX_LINE_LEN 256

#include <stdbool.h>

/* Store the section, key and value pairs */
typedef struct {
    char section[MAX_LINE_LEN];
    char key[MAX_LINE_LEN];
    char value[MAX_LINE_LEN];
} CiniItem;

/* .ini structure */
typedef struct {
    char filename[256];
    CiniItem items[MAX_ITEMS];
    int count;
} Cini;

int ciniLoad(Cini *cini, const char *filename);
const char* ciniGet(const Cini *cini, const char *section, const char *key, const char *fallback);
int ciniGetInt(const Cini *cini, const char *section, const char *key, int fallback);
float ciniGetFloat(const Cini *cini, const char *section, const char *key, float fallback);
bool ciniGetBool(const Cini *cini, const char *section, const char *key, bool fallback);
void ciniSet(Cini *cini, const char *section, const char *key, const char *value);
void ciniSetInt(Cini *cini, const char *section, const char *key, int value);
int ciniSave(const Cini *cini, const char *filename);

#endif