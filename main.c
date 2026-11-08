#define CINI_IMPLEMENTATION
#include "cini.h"
#include <stdio.h>

int main() {
    Cini cini;
    if (ciniLoad(&cini, "example.ini") != CINI_SUCCESS) {
        printf("Failed to load config!\n");
        return 1;
    }

    int width = ciniGetInt(&cini, "Video", "Width", 1920);
    
    ciniSetInt(&cini, "Video", "Width", 2560);
    ciniSave(&cini, "example.ini");

    return 0;
}