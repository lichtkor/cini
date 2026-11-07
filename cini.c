#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "cini.h"

static char* trim(char* str) {
    char* end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

int ciniLoad(Cini *cini, const char* filename) {
    strncpy(cini->filename, filename, sizeof(cini->filename) - 1);
    cini->count = 0;

    FILE* f = fopen(filename, "r");
    if (!f) {
        printf("CINI_FAILED_OPENING_FILE");
        return 0;
    }

    char buffer[2048];
    while(fgets(buffer, sizeof(buffer), f)) {
        /* remove newlines */
        buffer[strcspn(buffer, "\r\n")] = 0;

        char* line = trim(buffer);

        /* parsing comments */
        if (line[0] == '\0' || line[0] == '#' || line[0] == ';') {
            continue;
        }

        /* finding the = */
        char* eq = strchr(line, '=');
        if (eq) {
            *eq = '\0'; // cut the string at the =
            char* key = trim(line);
            char* val = trim(eq + 1);

            if (cini->count < MAX_ITEMS) {
                strncpy(cini->items[cini->count].key, key, MAX_LINE_LEN - 1);
                strncpy(cini->items[cini->count].value, val, MAX_LINE_LEN - 1);
                cini->count++;
            }
        }
    }

    fclose(f);
    return 1;
}   

int ciniGetInt(const Cini *cini, const char *key, int fallback) {
    for (int i = 0; i < cini->count; i++) {
        if (strcmp(cini->items[i].key, key) == 0) {
            /* convert the string into an int using atoi */
            return atoi(cini->items[i].value);
        }
    }
    return fallback;
}

float ciniGetfloat(const Cini *cini, const char *key, float fallback) {
    for (int i = 0; i < cini->count; i++) {
        if (strcmp(cini->items[i].key, key) == 0) {
            return atof(cini->items[i].value);
        }
    }
    return fallback;
}

const char* ciniGet(const Cini *cini, const char* key, const char* fallback) {
    for (int i = 0; i < cini->count; i++) {
        if (strcmp(cini->items[i].key, key) == 0) {
            return cini->items[i].value;
        }
    }
    return fallback;
}

bool ciniGetBool(const Cini *cini, const char *key, bool fallback) {
    const char *val = ciniGet(cini, key, false);
    if (!val) return fallback;

    if (strcmp(val, "true") == 0 || strcmp(val, "yes") == 0 || strcmp(val, "on") == 0 || strcmp(val, "1") == 0) {
        return true;
    }
    if (strcmp(val, "false") == 0 || strcmp(val, "no") == 0 || strcmp(val, "off") == 0 || strcmp(val, "0") == 0) {
        return false;
    }
    return fallback;
}