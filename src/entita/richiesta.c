#include "richiesta.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* STRUTTURA INTERNA — nascosta, non visibile fuori da questo file */
struct Richiesta {
    char* codice;                   /* Codice identificativo univoco della richiesta */
    char* appartamento;             /* Appartamento o area interessata */
    char* tipologia;                /* Tipologia del problema (es. Idraulico) */
    char* descrizione;              /* Descrizione estesa del problema */
    char* data;                     /* Data della richiesta (DD/MM/YYYY) */
    int urgenza;                  /* Livello di urgenza (0=minima, 4=massima) */
    StatoRichiesta stato;           /* Stato attuale della richiesta */
    char* codiceTecnicoAssegnato;   /* Codice del tecnico assegnato (NULL se non assegnato) */
    char* dataInizioLavorazione;    /* Data inizio lavorazione (NULL se non iniziata) */
    char* dataChiusura;             /* Data di chiusura (NULL se non conclusa) */
    int isValidaInHeap;           /* Flag: 1 se la richiesta e' valida nell'heap, 0 se e' stata rimossa logicamente */
};
 
/* FUNZIONE INTERNA DI SUPPORTO */
static char* duplicaStringa(const char* src) {
    char* copia;
    if (src == NULL) return NULL;
    copia = (char*)malloc(strlen(src) + 1);
    if (copia != NULL) {
        strcpy(copia, src);
    }
    return copia;
}
 
/* CREAZIONE E DISTRUZIONE DELLE RICHIESTE*/
/* Alloca e inizializza una nuova Richiesta con i campi forniti.*/
Richiesta* creaRichiesta(const char* codice, const char* appartamento, const char* tipologia, 
    const char* descrizione, const char* data, int urgenza) {
    Richiesta* nuova = (Richiesta*)malloc(sizeof(Richiesta));
    if (nuova == NULL) return NULL;
 
    nuova->codice      = duplicaStringa(codice);
    nuova->appartamento = duplicaStringa(appartamento);
    nuova->tipologia   = duplicaStringa(tipologia);
    nuova->descrizione = duplicaStringa(descrizione);
    nuova->data        = duplicaStringa(data);
    nuova->urgenza     = urgenza;
    nuova->stato       = APERTA;
 
/* Campi non ancora valorizzati alla creazione */
    nuova->codiceTecnicoAssegnato  = NULL;
    nuova->dataInizioLavorazione   = NULL;
    nuova->dataChiusura            = NULL;
 
    nuova->isValidaInHeap = 1; /* Valida di default */
 
    return nuova;
}

/* Libera tutta la memoria allocata per la Richiesta e i suoi campi. */
void distruggiRichiesta(Richiesta* richiestaTarget) {
    if (richiestaTarget == NULL) return;
 
    free(richiestaTarget->codice);
    free(richiestaTarget->appartamento);
    free(richiestaTarget->tipologia);
    free(richiestaTarget->descrizione);
    free(richiestaTarget->data);
    free(richiestaTarget->codiceTecnicoAssegnato);
    free(richiestaTarget->dataInizioLavorazione);
    free(richiestaTarget->dataChiusura);
 
    free(richiestaTarget);
}