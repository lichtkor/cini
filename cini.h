#ifndef CINI_H
#define CINI_H

#include <stdio.h>

const char* ciniLoad(const char* filename);
int getInt(const char *key, const char *value, int fallback);

#endif CINI_H