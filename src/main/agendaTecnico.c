#include "agendaTecnico.h"
#include "main/utilita.h"
#include <stdlib.h>
#include <string.h>

struct NodoAgenda {
    char* data;
    char* fasciaOraria;
    char* codiceRichiesta;
    NodoAgenda* sinistro;
    NodoAgenda* destro;
};

struct AgendaTecnico {
    NodoAgenda* radice;
};

static int confrontaAppuntamenti(const char* data1, const char* ora1, const char* data2, const char* ora2) {
    int cmpData = strcmp(data1, data2);
    if (cmpData != 0) {
        return cmpData;
    }
    return strcmp(ora1, ora2);
}

static NodoAgenda* inserisciNodoRicorsivo(NodoAgenda* radice, const char* data, const char* fasciaOraria, const char* codiceRichiesta, int* successo) {
    if (radice == NULL) {
        NodoAgenda* nuovoNodo = (NodoAgenda*)malloc(sizeof(NodoAgenda));
        if (nuovoNodo == NULL) {
            *successo = 0;
            return NULL;
        }
        
        nuovoNodo->data = malloc(strlen(data) + 1); 
        strcpy(nuovoNodo->data, data);
        nuovoNodo->fasciaOraria = malloc(strlen(fasciaOraria) + 1); 
        strcpy(nuovoNodo->fasciaOraria, fasciaOraria);
        nuovoNodo->codiceRichiesta = malloc(strlen(codiceRichiesta) + 1); 
        strcpy(nuovoNodo->codiceRichiesta, codiceRichiesta);
        
        nuovoNodo->sinistro = NULL;
        nuovoNodo->destro = NULL;
        
        *successo = 1;
        return nuovoNodo;
    }

    int cmp = confrontaAppuntamenti(data, fasciaOraria, radice->data, radice->fasciaOraria);

    if (cmp < 0) {
        radice->sinistro = inserisciNodoRicorsivo(radice->sinistro, data, fasciaOraria, codiceRichiesta, successo);
    } else if (cmp > 0) {
        radice->destro = inserisciNodoRicorsivo(radice->destro, data, fasciaOraria, codiceRichiesta, successo);
    } else {
        *successo = 0; 
    }

    return radice;
}

static void distruggiNodiRicorsivo(NodoAgenda* nodo) {
    if (nodo != NULL) {
        distruggiNodiRicorsivo(nodo->sinistro);
        distruggiNodiRicorsivo(nodo->destro);
        free(nodo->data);
        free(nodo->fasciaOraria);
        free(nodo->codiceRichiesta);
        free(nodo);
    }
}

AgendaTecnico* creaAgendaTecnico() {
    AgendaTecnico* nuovaAgenda = (AgendaTecnico*)malloc(sizeof(AgendaTecnico));
    if (nuovaAgenda != NULL) {
        nuovaAgenda->radice = NULL;
    }
    return nuovaAgenda;
}

void distruggiAgendaTecnico(AgendaTecnico* agendaTarget) {
    if (agendaTarget != NULL) {
        distruggiNodiRicorsivo(agendaTarget->radice);
        free(agendaTarget);
    }
}

int inserisciInterventoInAgenda(AgendaTecnico* agendaTarget, const char* data, const char* fasciaOraria, const char* codiceRichiesta) {
    if (agendaTarget == NULL || data == NULL || fasciaOraria == NULL || codiceRichiesta == NULL) {
        return 0;
    }
    
    int successo = 0;
    agendaTarget->radice = inserisciNodoRicorsivo(agendaTarget->radice, data, fasciaOraria, codiceRichiesta, &successo);
    
    return successo;
}

NodoAgenda* getRadiceAgenda(const AgendaTecnico* agendaTarget) {
    if (agendaTarget == NULL) return NULL;
    return agendaTarget->radice;
}

NodoAgenda* getFiglioSinistroAgenda(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->sinistro;
}

NodoAgenda* getFiglioDestroAgenda(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->destro;
}

const char* getDataIntervento(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->data;
}

const char* getFasciaOrariaIntervento(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->fasciaOraria;
}

const char* getCodiceRichiestaInAgenda(const NodoAgenda* nodoCorrente) {
    if (nodoCorrente == NULL) return NULL;
    return nodoCorrente->codiceRichiesta;
}