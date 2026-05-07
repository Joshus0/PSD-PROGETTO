#include "../include/alberoTecnici.h"
#include "entita/tecnico.h"
#include <stdlib.h>
#include <string.h>

/* Struttura del nodo dell'albero */
struct NodoAlberoTecnici {
    Tecnico* dato;                  /* Puntatore al Tecnico contenuto nel nodo */
    NodoAlberoTecnici* sinistro;    /* Figlio sinistro */
    NodoAlberoTecnici* destro;      /* Figlio destro */
};

/* Struttura wrapper per l'albero */
struct AlberoTecnici {
    NodoAlberoTecnici* radice;      /* Radice dell'albero */
};

/* --- Funzioni di supporto interne --- */

/* Funzione ricorsiva per l'inserimento ordinato in base al codice del tecnico */
static NodoAlberoTecnici* inserisciNodoRicorsivo(NodoAlberoTecnici* nodoCorrente, Tecnico* nuovoTecnico) {
    int confronto;

    /* Caso base: se l'albero/sottoalbero è vuoto, crea un nuovo albero con un solo elemento */
    if (nodoCorrente == NULL) {
        NodoAlberoTecnici* nuovoNodo = (NodoAlberoTecnici*)malloc(sizeof(NodoAlberoTecnici));
        if (nuovoNodo != NULL) {
            nuovoNodo->dato = nuovoTecnico;
            nuovoNodo->sinistro = NULL;
            nuovoNodo->destro = NULL;
        }
        return nuovoNodo;
    }

    /* Confronto alfanumerico tra i codici per determinare il percorso */
    confronto = strcmp(getCodiceTecnico(nuovoTecnico), getCodiceTecnico(nodoCorrente->dato));
    
    /* Se l'elemento è minore della radice, lo inserisce nel sottoalbero sinistro */
    if (confronto < 0) {
        nodoCorrente->sinistro = inserisciNodoRicorsivo(nodoCorrente->sinistro, nuovoTecnico);
    } 
    /* Se l'elemento è maggiore della radice, lo inserisce nel sottoalbero destro */
    else if (confronto > 0) {
        nodoCorrente->destro = inserisciNodoRicorsivo(nodoCorrente->destro, nuovoTecnico);
    }
    /* Se confronto == 0, il nodo coincide con la radice e non fa niente (no duplicati) */

    return nodoCorrente;
}

/* Funzione ricorsiva per la ricerca di un tecnico tramite il suo codice */
static Tecnico* cercaTecnicoRicorsivo(NodoAlberoTecnici* nodoCorrente, const char* codiceDaCercare) {
    int confronto;

    /* Se l'albero è vuoto restituisce NULL */
    if (nodoCorrente == NULL) {
        return NULL;
    }

    confronto = strcmp(codiceDaCercare, getCodiceTecnico(nodoCorrente->dato));

    /* Se l'elemento cercato coincide con la radice restituisce il dato */
    if (confronto == 0) {
        return nodoCorrente->dato; 
    } 
    /* Se l'elemento cercato è minore, restituisce il risultato della ricerca nel sottoalbero sinistro */
    else if (confronto < 0) {
        return cercaTecnicoRicorsivo(nodoCorrente->sinistro, codiceDaCercare);
    } 
    /* Se l'elemento cercato è maggiore, restituisce il risultato della ricerca nel sottoalbero destro */
    else {
        return cercaTecnicoRicorsivo(nodoCorrente->destro, codiceDaCercare);
    }
}

/* Funzione ricorsiva per liberare la memoria (attraversamento post-ordine) */
static void distruggiNodiRicorsivo(NodoAlberoTecnici* nodoCorrente) {
    if (nodoCorrente != NULL) {
        /* Distrugge prima i sottoalberi */
        distruggiNodiRicorsivo(nodoCorrente->sinistro);
        distruggiNodiRicorsivo(nodoCorrente->destro);
        
        /* Libera la memoria allocata per l'entità Tecnico e per il nodo stesso */
        distruggiTecnico(nodoCorrente->dato); 
        free(nodoCorrente);                   
    }
}

/* --- Implementazione delle funzioni dichiarate in alberoTecnici.h --- */

AlberoTecnici* creaAlberoTecnici() {
    AlberoTecnici* nuovoAlbero = (AlberoTecnici*)malloc(sizeof(AlberoTecnici));
    if (nuovoAlbero != NULL) {
        nuovoAlbero->radice = NULL;
    }
    return nuovoAlbero;
}

void distruggiAlberoTecnici(AlberoTecnici* alberoTarget) {
    if (alberoTarget != NULL) {
        distruggiNodiRicorsivo(alberoTarget->radice);
        free(alberoTarget);
    }
}

void inserisciInAlberoTecnici(AlberoTecnici* alberoTarget, Tecnico* nuovoTecnico) {
    if (alberoTarget == NULL || nuovoTecnico == NULL) return;
    alberoTarget->radice = inserisciNodoRicorsivo(alberoTarget->radice, nuovoTecnico);
}

Tecnico* cercaTecnicoInAlbero(const AlberoTecnici* alberoTarget, const char* codiceDaCercare) {
    if (alberoTarget == NULL || codiceDaCercare == NULL) return NULL;
    return cercaTecnicoRicorsivo(alberoTarget->radice, codiceDaCercare);
}

/* --- Funzioni Getter esposte (Modificate con if-else) --- */

NodoAlberoTecnici* getRadiceAlberoTecnici(const AlberoTecnici* alberoTarget) {
    if (alberoTarget != NULL) {
        return alberoTarget->radice;
    } else {
        return NULL;
    }
}

NodoAlberoTecnici* getFiglioSinistroTecnici(const NodoAlberoTecnici* nodoCorrente) {
    if (nodoCorrente != NULL) {
        return nodoCorrente->sinistro;
    } else {
        return NULL;
    }
}

NodoAlberoTecnici* getFiglioDestroTecnici(const NodoAlberoTecnici* nodoCorrente) {
    if (nodoCorrente != NULL) {
        return nodoCorrente->destro;
    } else {
        return NULL;
    }
}

Tecnico* getTecnicoDalNodoAlbero(const NodoAlberoTecnici* nodoCorrente) {
    if (nodoCorrente != NULL) {
        return nodoCorrente->dato;
    } else {
        return NULL;
    }
}

/* Altre funzioni aggiunte in alberoTecnici.h*/
static void visitaRicorsivo(NodoAlberoTecnici* nodo, void (*visitatore)(Tecnico*)) {
    if (nodo == NULL) return;
    visitaRicorsivo(nodo->sinistro, visitatore);
    visitatore(nodo->dato);
    visitaRicorsivo(nodo->destro, visitatore);
}

void visitaAlberoTecnici(const AlberoTecnici* alberoTarget, void (*visitatore)(Tecnico*)) {
    if (alberoTarget == NULL || visitatore == NULL) return;
    visitaRicorsivo(alberoTarget->radice, visitatore);
}