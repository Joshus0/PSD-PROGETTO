#include "../include/archivioRichieste.h"
#include <stdlib.h>


struct NodoLista 
{
    Richiesta* dato;      /* Puntatore alla richiesta contenuta nel nodo */
    NodoLista* next;      /* Puntatore al nodo successivo nella lista */
    NodoLista* prev;      /* Puntatore al nodo precedente nella lista */
};


struct ArchivioRichieste
{
    NodoLista* testa;     /* Puntatore al primo nodo della lista */
    NodoLista* coda;      /* Puntatore all'ultimo nodo della lista */
    int dimensione;       /* Numero di richieste attualmente nell'archivio */
};


ArchivioRichieste* creaArchivioRichieste()
{
    ArchivioRichieste* nuovoArchivio = (ArchivioRichieste*)malloc(sizeof(ArchivioRichieste));
    if(nuovoArchivio != NULL)
    {
        nuovoArchivio->testa = NULL;
        nuovoArchivio->coda = NULL;
        nuovoArchivio->dimensione = 0;
    }
    return nuovoArchivio;
}


void distruggiArchivioRichieste(ArchivioRichieste* archivioTarget)
{
    if(archivioTarget != NULL)
    {
        NodoLista* nodoCorrente = archivioTarget->testa;
        while(nodoCorrente != NULL)
        {
            NodoLista* nodoDaEliminare = nodoCorrente;
            nodoCorrente = nodoCorrente->next;
            distruggiRichiesta(nodoDaEliminare->dato); /* Libera la richiesta contenuta nel nodo */
            free(nodoDaEliminare); /* Libera il nodo stesso */
        }
        free(archivioTarget); /* Libera la struttura dell'archivio */
    }
}




void inserisciInCodaArchivio(ArchivioRichieste* archivioTarget, Richiesta* nuovaRichiesta) {
    if (archivioTarget == NULL || nuovaRichiesta == NULL) return;

    NodoLista* nuovoNodo = (NodoLista*)malloc(sizeof(NodoLista));
    if (nuovoNodo == NULL) return;

    nuovoNodo->dato = nuovaRichiesta;
    nuovoNodo->next = NULL;
    nuovoNodo->prev = archivioTarget->coda;

    if (archivioTarget->coda != NULL) {
        archivioTarget->coda->next = nuovoNodo;
    } else {
        archivioTarget->testa = nuovoNodo;
    }
    
    archivioTarget->coda = nuovoNodo;
    archivioTarget->dimensione++;
}

void rimuoviNodoDaArchivio(ArchivioRichieste* archivioTarget, NodoLista* nodoDaRimuovere) {
    if (archivioTarget == NULL || nodoDaRimuovere == NULL) return;

    if (nodoDaRimuovere->prev != NULL) {
        nodoDaRimuovere->prev->next = nodoDaRimuovere->next;
    } else {
        archivioTarget->testa = nodoDaRimuovere->next;
    }

    if (nodoDaRimuovere->next != NULL) {
        nodoDaRimuovere->next->prev = nodoDaRimuovere->prev;
    } else {
        archivioTarget->coda = nodoDaRimuovere->prev;
    }

    free(nodoDaRimuovere);
    archivioTarget->dimensione--;
}

NodoLista* getTestaArchivio(const ArchivioRichieste* archivioTarget) {
    return (archivioTarget != NULL) ? archivioTarget->testa : NULL;
}

NodoLista* getCodaArchivio(const ArchivioRichieste* archivioTarget) {
    return (archivioTarget != NULL) ? archivioTarget->coda : NULL;
}

NodoLista* getNextNodoLista(const NodoLista* nodoCorrente) {
    return (nodoCorrente != NULL) ? nodoCorrente->next : NULL;
}

NodoLista* getPrevNodoLista(const NodoLista* nodoCorrente) {
    return (nodoCorrente != NULL) ? nodoCorrente->prev : NULL;
}

Richiesta* getRichiestaDalNodoLista(const NodoLista* nodoCorrente) {
    return (nodoCorrente != NULL) ? nodoCorrente->dato : NULL;
}

int getDimensioneArchivio(const ArchivioRichieste* archivioTarget) {
    return (archivioTarget != NULL) ? archivioTarget->dimensione : 0;
}