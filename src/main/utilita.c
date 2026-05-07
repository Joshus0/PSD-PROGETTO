#include "../include/main/utilita.h"
#include <stdlib.h>
#include <string.h>

char* duplicaStringa(const char* src) {
    char* copia;
    if (src == NULL) return NULL;
    copia = (char*)malloc(strlen(src) + 1);
    if (copia != NULL) strcpy(copia, src);
    return copia;
}