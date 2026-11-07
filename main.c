#include <stdio.h>
#include "cini.h"

int main(void) {
    Cini cini;

    if (!ciniLoad(&cini, "example.ini")) {
        printf("Error: could not open example.ini\n");
        return 1;
    }

    printf("Loaded %s successfully and %d keys\n", cini.filename, cini.count);

    // no section
    const char* app_name = ciniGet(&cini, NULL, "app_name", "DefaultApp");

    // [renderer] section
    int width = ciniGetInt(&cini, "renderer", "width", 800);
    int height = ciniGetInt(&cini, "renderer", "height", 600);
    float pi = ciniGetFloat(&cini, "renderer", "pi", 3.14f);
    bool vsync = ciniGetBool(&cini, "renderer", "vsync", false);

    // [audio] section
    float volume = ciniGetFloat(&cini, "audio", "volume", 1.0f);
    bool muted = ciniGetBool(&cini, "audio", "muted", true);
    const char* debug_mode = ciniGet(&cini, "audio", "debug_mode", "off");

    printf("app_name: %s\n", app_name);
    printf("[Renderer]\n");
    printf("width: %d\n", width);
    printf("height: %d\n", height);
    printf("pi: %f\n", pi);
    printf("vsync: %d\n", vsync);
    printf("[Audio]\n");
    printf("volume: %f\n", volume);
    printf("muted: %d\n", muted);
    printf("debug_mode: %s\n", debug_mode);

    return 0;
}