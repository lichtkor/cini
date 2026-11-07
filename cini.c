#include "cini.h"

const char* ciniLoad(const char* filename) {
    FILE* f = fopen(filename, "r");
    char buffer[2048];

    while (fgets(buffer, 2048, f)) {
        printf("%s", buffer);
    }
    
    fclose(f);
    return "e";
}   

// int getInt(const char *key, const char *value, int fallback) {
//     FILE* file = 
// }

int main(void) {
    const char* init = ciniLoad("example.ini");
    printf("\ninit: %s", init);
    // int width = getInt("", "", 0);
    return 0;
}