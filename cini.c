#include "cini.h"

int ciniLoad(Cini *cini, const char* filename) {
    FILE* f = fopen(filename, "r");
    char buffer[2048];

    while (fgets(buffer, 2048, f)) {
        printf("%s", buffer);
    }
    
    fclose(f);
    return 0;
}   

int ciniGetInt(const Cini *cini, const char *value, int fallback) {
    
}

int main(void) {
    Cini cini;
    ciniLoad(&cini, "example.ini");
    return 0;
}