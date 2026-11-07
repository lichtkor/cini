#include <stdio.h>
#include "cini.h"

int main(void) {
    Cini cini;

    // load the .ini
    if (!ciniLoad(&cini, "example.ini")) {
        printf("Error: could not opepn example.ini\n");
        return 1;
    }

    printf("loaded %s and %d keys found\n", cini.filename, cini.count);

    int width = ciniGetInt(&cini, "width", 800);
    int height = ciniGetInt(&cini, "height", 800);
    int framerate = ciniGetInt(&cini, "fps", 60);
    float pi = ciniGetfloat(&cini, "pi", 3.14);
    bool vsync = ciniGetBool(&cini, "vsync", true);
    const char* debug = ciniGet(&cini, "debug", "on");

    printf("width: %d\n", width);
    printf("height: %d\n", height);
    printf("fps: %d\n", framerate);
    printf("pi: %f\n", pi);
    printf("vsync: %d\n", vsync);
    printf("debug: %s\n", debug);
    
    return 0;
}