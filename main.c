#define CINI_IMPLEMENTATION
#include "cini.h"

#include <stdio.h>

int main(void) {
    Cini cini;

    if (!ciniLoad(&cini, "example.ini")) {
        printf("Error loading file\n");
        return 1;
    }

    int width = ciniGetInt(&cini, "renderer", "width", 800);
    printf("Loaded width: %d\n", width);

    return 0;
}