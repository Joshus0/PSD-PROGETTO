#include "codaPriorita.h"
#include <stdlib.h>

struct CodaPriorita {
    Richiesta** array;
    int capacita;
    int dimensione;
};

static void scambia(Richiesta** a, Richiesta** b) {
    Richiesta* temp = *a;
    *a = *b;
    *b = temp;
}

static void heapifyUp(CodaPriorita* coda, int indice) {
    if (indice == 0) return;
    int padre = (indice - 1) / 2;
    
    if (getLivelloUrgenzaRichiesta(coda->array[indice]) > getLivelloUrgenzaRichiesta(coda->array[padre])) {
        scambia(&coda->array[indice], &coda->array[padre]);
        heapifyUp(coda, padre);
    }
}

static void heapifyDown(CodaPriorita* coda, int indice) {
    int massimo = indice;
    int figlioSinistro = 2 * indice + 1;
    int figlioDestro = 2 * indice + 2;

    if (figlioSinistro < coda->dimensione && 
        getLivelloUrgenzaRichiesta(coda->array[figlioSinistro]) > getLivelloUrgenzaRichiesta(coda->array[massimo])) {
        massimo = figlioSinistro;
    }

    if (figlioDestro < coda->dimensione && 
        getLivelloUrgenzaRichiesta(coda->array[figlioDestro]) > getLivelloUrgenzaRichiesta(coda->array[massimo])) {
        massimo = figlioDestro;
    }

    if (massimo != indice) {
        scambia(&coda->array[indice], &coda->array[massimo]);
        heapifyDown(coda, massimo);
    }
}

CodaPriorita* creaCodaPriorita(int capacitaIniziale) {
    if (capacitaIniziale <= 0) capacitaIniziale = 10;
    
    CodaPriorita* nuovaCoda = (CodaPriorita*)malloc(sizeof(CodaPriorita));
    if (nuovaCoda != NULL) {
        nuovaCoda->array = (Richiesta**)malloc(capacitaIniziale * sizeof(Richiesta*));
        if (nuovaCoda->array == NULL) {
            free(nuovaCoda);
            return NULL;
        }
        nuovaCoda->capacita = capacitaIniziale;
        nuovaCoda->dimensione = 0;
    }
    return nuovaCoda;
}

void distruggiCodaPriorita(CodaPriorita* codaTarget) {
    if (codaTarget != NULL) {
        if (codaTarget->array != NULL) {
            free(codaTarget->array);
        }
        free(codaTarget);
    }
}

void inserisciInCodaPriorita(CodaPriorita* codaTarget, Richiesta* richiestaDaAccodare) {
    if (codaTarget == NULL || richiestaDaAccodare == NULL) return;

    if (codaTarget->dimensione == codaTarget->capacita) {
        int nuovaCapacita = codaTarget->capacita * 2;
        Richiesta** nuovoArray = (Richiesta**)realloc(codaTarget->array, nuovaCapacita * sizeof(Richiesta*));
        if (nuovoArray == NULL) return; 
        codaTarget->array = nuovoArray;
        codaTarget->capacita = nuovaCapacita;
    }

    codaTarget->array[codaTarget->dimensione] = richiestaDaAccodare;
    heapifyUp(codaTarget, codaTarget->dimensione);
    codaTarget->dimensione++;
}

Richiesta* estraiMaxDaCodaPriorita(CodaPriorita* codaTarget) {
    if (codaTarget == NULL || codaTarget->dimensione == 0) return NULL;

    Richiesta* maxRichiesta = codaTarget->array[0];
    
    codaTarget->dimensione--;
    codaTarget->array[0] = codaTarget->array[codaTarget->dimensione];
    codaTarget->array[codaTarget->dimensione] = NULL;

    if (codaTarget->dimensione > 0) {
        heapifyDown(codaTarget, 0);
    }

    return maxRichiesta;
}

int isCodaPrioritaVuota(const CodaPriorita* codaTarget) {
    return (codaTarget == NULL || codaTarget->dimensione == 0) ? 1 : 0;
}

int getDimensioneCodaPriorita(const CodaPriorita* codaTarget) {
    return (codaTarget != NULL) ? codaTarget->dimensione : 0;
}