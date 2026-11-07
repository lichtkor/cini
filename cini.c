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