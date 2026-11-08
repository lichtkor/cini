/*
 * cini.h
 * A simple, lightweight .ini file parser and writer for C and C++.
 *
 * This software is available under the MIT License.
 * Copyright (c) 2026 lichtkor
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * USAGE
 * In one C or C++ source file, define CINI_IMPLEMENTATION before including:
 *     #define CINI_IMPLEMENTATION
 *     #include "cini.h"
 *
 * In all other files, just include the header normally:
 *     #include "cini.h"
*/

#ifndef CINI_H
#define CINI_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_ITEMS 100
#define MAX_LINE_LEN 256

#define CINI_SUCCESS 0
#define CINI_ERR_FILE_NOT_FOUND -1
#define CINI_ERR_SYNTAX -2
#define CINI_ERR_FULL -3

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

/*
 * Helper function: Trim leading and trailing whitespace from a string in place.
 * Return type: char* (Pointer to the first non whitespace character)
 * Use case: Used internally during parsing to clean up sections, keys, and values.
 */
static char* cini_trim(char* str) {
    char* end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}
/*
 * Helper function: Search for a specific key within a section in memory.
 * Return type: int (Array index of the item if found, -1 otherwise)
 * Use case: Used internally by getter and setter functions.
 */
static int cini_find_item(const Cini *cini, const char *section, const char *key) {
    const char *sec = section ? section : "";
    for (int i = 0; i < cini->count; i++) {
        if (strcmp(cini->items[i].section, sec) == 0 && strcmp(cini->items[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

/*
 * Loads and parses an .ini file into a Cini structure.
 * Return type: int (CINI_SUCCESS on success, CINI_ERR_FILE_NOT_FOUND or CINI_ERR_SYNTAX on failure)
 * Use case: Call this at application startup to populate settings from disk.
 */
int ciniLoad(Cini *cini, const char* filename) {
    strncpy(cini->filename, filename, sizeof(cini->filename) - 1);
    cini->filename[sizeof(cini->filename) - 1] = '\0';
    cini->count = 0;

    FILE* f = fopen(filename, "r");
    if (!f) {
        return CINI_ERR_FILE_NOT_FOUND;
    }

    char current_section[MAX_LINE_LEN] = "";
    char buffer[2048];

    while(fgets(buffer, sizeof(buffer), f)) {
        buffer[strcspn(buffer, "\r\n")] = 0;
        char* line = cini_trim(buffer);

        if (line[0] == '\0' || line[0] == '#' || line[0] == ';') {
            continue;
        }

        if (line[0] == '[') {
            size_t line_len = strlen(line);
            if (line[line_len - 1] != ']') {
                fclose(f);
                return CINI_ERR_SYNTAX;
            }

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
        } else {
            fclose(f);
            return CINI_ERR_SYNTAX;
        }
    }

    fclose(f);
    return CINI_SUCCESS;
}

/*
 * Retrieves a string value for a given section and key.
 * Return type: const char* (The parsed string value, or fallback if not found)
 * Use case: Reading raw string configuration entries.
 */
const char* ciniGet(const Cini *cini, const char *section, const char *key, const char* fallback) {
    int idx = cini_find_item(cini, section, key);
    if (idx != -1) {
        return cini->items[idx].value;
    }
    return fallback;
}

/*
 * Retrieves an integer value for a given section and key.
 * Return type: int (Parsed integer, or fallback if not found)
 * Use case: Reading numerical settings such as window widths, ports, or limits.
 */
int ciniGetInt(const Cini *cini, const char *section, const char *key, int fallback) {
    const char *val = ciniGet(cini, section, key, NULL);
    if (!val) return fallback;
    return atoi(val);
}

/*
 * Retrieves a float value for a given section and key.
 * Return type: float (Parsed floating point number, or fallback if not found)
 * Use case: Reading decimal settings such as audio volume or scaling factors.
 */
float ciniGetFloat(const Cini *cini, const char *section, const char *key, float fallback) {
    const char *val = ciniGet(cini, section, key, NULL);
    if (!val) return fallback;
    return (float)atof(val);
}

/*
 * Retrieves a boolean value for a given section and key.
 * Return type: bool (True for "true"/"yes"/"on"/"1", False for "false"/"no"/"off"/"0", or fallback)
 * Use case: Checking feature flags or toggle options.
 */
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

/*
 * Sets or updates a string entry in the structure.
 * Return type: void
 * Use case: Modifying or dynamically adding configuration parameters before saving.
 */
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

/*
 * Sets or updates an integer entry in the structure.
 * Return type: void
 * Use case: Convenience wrapper to store numeric settings as formatted strings.
 */
void ciniSetInt(Cini *cini, const char *section, const char *key, int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    ciniSet(cini, section, key, buf);
}

/*
 * Writes the in memory Cini items back out to a formatted file.
 * Return type: int (CINI_SUCCESS on success, CINI_ERR_FILE_NOT_FOUND on write failure)
 * Use case: Persisting modified configuration parameters to disk.
 */
int ciniSave(const Cini *cini, const char *filename) {
    const char *fname = filename ? filename : cini->filename;
    FILE *f = fopen(fname, "w");
    if (!f) {
        return CINI_ERR_FILE_NOT_FOUND;
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
    return CINI_SUCCESS;
}

#endif