#include <stdio.h>
#include "cini.h"

int main(void) {
    Cini cini;

    if (!ciniLoad(&cini, "example.ini")) {
        printf("Error opening file\n");
        return 1;
    }

    ciniSetInt(&cini, "renderer", "width", 2560);
    ciniSet(&cini, "renderer", "vsync", "false");

    ciniSet(&cini, "audio", "device", "Headphones"); 

    printf("Updated width: %d\n", ciniGetInt(&cini, "renderer", "width", 800));
    printf("Updated audio device: %s\n", ciniGet(&cini, "audio", "device", "Default"));

    if (ciniSave(&cini, "example.ini")) {
        printf("\nSuccessfully saved changes to 'example.ini'!\n");
    }

    return 0;
}