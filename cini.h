#ifndef CINI_H
#define CINI_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_ITEMS 100
#define MAX_LINE_LEN 256

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

#ifdef CINI_IMPLEMENTATION

static char* cini_trim(char* str) {
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
    cini->filename[sizeof(cini->filename) - 1] = '\0';
    cini->count = 0;

    FILE* f = fopen(filename, "r");
    if (!f) {
        return 0;
    }

    char current_section[MAX_LINE_LEN] = "";
    char buffer[2048];

    while(fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        char* line = cini_trim(buffer);

        if (line[0] == '\0' || line[0] == '#' || line[0] == ';') {
            continue;
        }

        if (line[0] == '[' && line[strlen(line) - 1] == ']') {
            strncpy(current_section, line + 1, sizeof(current_section) - 1);
            current_section[sizeof(current_section) - 1] = '\0';
            size_t len = strlen(current_section);
            if (len > 0 && current_section[len - 1] == ']') {
                current_section[len - 1] = '\0';
            }
            char* trimmed_sec = cini_trim(current_section);
            if (trimmed_sec != current_section) {
                memmove(current_section, trimmed_sec, strlen(trimmed_sec) + 1);
            }
            continue;
        }

        char* eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            char* key = cini_trim(line);
            char* val = cini_trim(eq + 1);

            size_t val_len = strlen(val);
            if (val_len >= 2 && val[0] == '"' && val[val_len - 1] == '"') {
                val[val_len - 1] = '\0';
                val++;
            }

            if (cini->count < MAX_ITEMS) {
                strncpy(cini->items[cini->count].section, current_section, MAX_LINE_LEN - 1);
                cini->items[cini->count].section[MAX_LINE_LEN - 1] = '\0';
                strncpy(cini->items[cini->count].key, key, MAX_LINE_LEN - 1);
                cini->items[cini->count].key[MAX_LINE_LEN - 1] = '\0';
                strncpy(cini->items[cini->count].value, val, MAX_LINE_LEN - 1);
                cini->items[cini->count].value[MAX_LINE_LEN - 1] = '\0';
                cini->count++;
            }
        }
    }

    fclose(f);
    return 1;
}

static int cini_find_item(const Cini *cini, const char *section, const char *key) {
    const char *sec = section ? section : "";
    for (int i = 0; i < cini->count; i++) {
        if (strcmp(cini->items[i].section, sec) == 0 && strcmp(cini->items[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

const char* ciniGet(const Cini *cini, const char *section, const char *key, const char* fallback) {
    int idx = cini_find_item(cini, section, key);
    if (idx != -1) {
        return cini->items[idx].value;
    }
    return fallback;
}

int ciniGetInt(const Cini *cini, const char *section, const char *key, int fallback) {
    const char *val = ciniGet(cini, section, key, NULL);
    if (!val) return fallback;
    return atoi(val);
}

float ciniGetFloat(const Cini *cini, const char *section, const char *key, float fallback) {
    const char *val = ciniGet(cini, section, key, NULL);
    if (!val) return fallback;
    return (float)atof(val);
}

bool ciniGetBool(const Cini *cini, const char *section, const char *key, bool fallback) {
    const char *val = ciniGet(cini, section, key, NULL);
    if (!val) return fallback;

    if (strcmp(val, "true") == 0 || strcmp(val, "yes") == 0 || strcmp(val, "on") == 0 || strcmp(val, "1") == 0) {
        return true;
    }
    if (strcmp(val, "false") == 0 || strcmp(val, "no") == 0 || strcmp(val, "off") == 0 || strcmp(val, "0") == 0) {
        return false;
    }
    return fallback;
}

void ciniSet(Cini *cini, const char *section, const char *key, const char *value) {
    const char *sec = section ? section : "";
    
    for (int i = 0; i < cini->count; i++) {
        if (strcmp(cini->items[i].section, sec) == 0 && strcmp(cini->items[i].key, key) == 0) {
            strncpy(cini->items[i].value, value, MAX_LINE_LEN - 1);
            cini->items[i].value[MAX_LINE_LEN - 1] = '\0';
            return;
        }
    }

    if (cini->count < MAX_ITEMS) {
        strncpy(cini->items[cini->count].section, sec, MAX_LINE_LEN - 1);
        cini->items[cini->count].section[MAX_LINE_LEN - 1] = '\0';
        strncpy(cini->items[cini->count].key, key, MAX_LINE_LEN - 1);
        cini->items[cini->count].key[MAX_LINE_LEN - 1] = '\0';
        strncpy(cini->items[cini->count].value, value, MAX_LINE_LEN - 1);
        cini->items[cini->count].value[MAX_LINE_LEN - 1] = '\0';
        cini->count++;
    }
}

void ciniSetInt(Cini *cini, const char *section, const char *key, int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    ciniSet(cini, section, key, buf);
}

int ciniSave(const Cini *cini, const char *filename) {
    const char *fname = filename ? filename : cini->filename;
    FILE *f = fopen(fname, "w");
    if (!f) {
        return 0;
    }

    char last_section[MAX_LINE_LEN] = "___NONE___";

    for (int i = 0; i < cini->count; i++) {
        if (strcmp(cini->items[i].section, last_section) != 0) {
            strcpy(last_section, cini->items[i].section);
            if (strlen(last_section) > 0) {
                fprintf(f, "\n[%s]\n", last_section);
            } else if (i > 0) {
                fprintf(f, "\n");
            }
        }
        fprintf(f, "%s = %s\n", cini->items[i].key, cini->items[i].value);
    }

    fclose(f);
    return 1;
}

#endif
