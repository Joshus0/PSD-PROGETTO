#include "main/utilita.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* duplicaStringa(const char* src) {
    char* copia;
    if (src == NULL) return NULL;
    
    copia = (char*)malloc(strlen(src) + 1);
    if (copia != NULL) {
        strcpy(copia, src);
    }
    return copia;
}

int validaData(const char* data) {
    int giorno, mese, anno;
    int bisestile;
    
    /* Controllo lunghezza e presenza degli slash */
    if (data == NULL || strlen(data) != 10) return 0;
    if (data[2] != '/' || data[5] != '/') return 0;

    /* Estrazione dei numeri */
    if (sscanf(data, "%2d/%2d/%4d", &giorno, &mese, &anno) != 3) {
        return 0; /* Non sono numeri validi */
    }

    /* Controlli base su anno e mese */
    if (anno < 1900 || anno > 2100) return 0;
    if (mese < 1 || mese > 12) return 0;
    if (giorno < 1 || giorno > 31) return 0;

    /* Controllo giorni massimi in base al mese */
    if ((mese == 4 || mese == 6 || mese == 9 || mese == 11) && giorno > 30) {
        return 0;
    }
    
    /* Controllo specifico per Febbraio e anni bisestili */
    if (mese == 2) {
        bisestile = (anno % 4 == 0 && (anno % 100 != 0 || anno % 400 == 0));
        if (giorno > 28 + bisestile) {
            return 0;
        }
    }

    return 1; /* La data e' perfetta */
}